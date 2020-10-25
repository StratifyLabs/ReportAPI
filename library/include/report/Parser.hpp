#ifndef REPORTAPI_REPORT_PARSER_HPP
#define REPORTAPI_REPORT_PARSER_HPP

#include <fs/File.hpp>
#include <var/Queue.hpp>
#include <var/StackString.hpp>

#include "Section.hpp"

namespace report {

class Parser : public api::ExecutionContext {
public:
  Parser(const fs::File &input);
  Parser &create_report(const fs::File &output);

private:
  class IntermediateData {
  public:
    IntermediateData(const var::StringView line) {
      const size_t position = line.find(":");
      set_prefix(line.get_substring_with_length(position));
      if (prefix().is_empty()) {
        set_prefix(Section::generate_prefix("raw", "text"));
      }
    }

    bool operator==(const IntermediateData &a) const {
      return prefix() == a.prefix();
    }

    IntermediateData &push(const var::StringView line) {
      if (line.find(prefix()) != 0) {
        return *this;
      }
      const size_t position = line.find(":");
      if (line.length() > position + 1) {
        entry_list().push_back(
          line.get_substring_at_position(position + 1).pop_back().get_string());
      }
      return *this;
    }

    void generate(const fs::File &output) const;

  private:
    API_AS(IntermediateData, prefix);
    API_AC(IntermediateData, var::StringList, entry_list);

    const var::StringView type() const { return Section::get_type(prefix()); }
    const var::StringView name() const { return Section::get_name(prefix()); }

    const var::StringView timestamp(const var::StringView &entry) const {
      const size_t position = entry.find(":");
      return entry.get_substring_with_length(position);
    }
    const var::StringView content(const var::StringView &entry) const {
      const size_t position = entry.find(":");
      return entry.get_substring_at_position(position + 1);
    }

    bool is_type_valid() const;

    void generate_csv_chart(const fs::File &output) const;
    void generate_csv_table(const fs::File &output) const;
    void generate_histogram_chart(const fs::File &output) const;
    void generate_passthrough(const fs::File &output) const;
    void generate_raw(const fs::File &output) const;
  };
  var::Queue<IntermediateData> m_data_list;
};

} // namespace report

#endif // REPORTAPI_REPORT_PARSER_HPP
