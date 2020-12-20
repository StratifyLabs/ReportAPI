// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef REPORTAPI_REPORT_PARSER_HPP
#define REPORTAPI_REPORT_PARSER_HPP

#include <fs/File.hpp>
#include <var/Queue.hpp>
#include <var/StackString.hpp>
#include <var/Tokenizer.hpp>

#include "Section.hpp"

namespace report {

class Parser : public api::ExecutionContext {
public:
  Parser(const fs::FileObject &input);
  Parser &create_report(const fs::FileObject &output);

private:
  class IntermediateData {
  public:
    class Entry {
      API_AS(Entry, timestamp);
      API_AS(Entry, content);
    };

    IntermediateData(const var::StringView line) {
      const size_t position = line.find(":");
      if (position == var::StringView::npos) {
        set_prefix(Section::generate_prefix("raw", "text"));
      } else {
        set_prefix(line.get_substring_with_length(position));
        if (prefix().find("/") == var::StringView::npos) {
          set_prefix(Section::generate_prefix("", "text"));
        }
      }
    }

    bool operator==(const IntermediateData &a) const {
      return prefix() == a.prefix();
    }

    IntermediateData &push(const var::StringView line) {
      if (line.find(prefix()) != 0) {
        return *this;
      }
      var::Tokenizer list(
        line,
        var::Tokenizer::Construct()
          .set_delimeters(":")
          .set_maximum_delimeter_count(2));

      if (list.count() == 3) {
        entry_list().push_back(Entry()
                                 .set_timestamp(list.at(1).to_string())
                                 .set_content(list.at(2).to_string()));
      }

      return *this;
    }

    void generate(const fs::FileObject &output) const;

  private:
    API_AS(IntermediateData, prefix);
    API_AC(IntermediateData, var::Vector<Entry>, entry_list);

    const var::StringView type() const { return Section::get_type(prefix()); }
    const var::StringView name() const { return Section::get_name(prefix()); }

    const var::StringView timestamp(const var::StringView &entry) const {
      const size_t position = entry.find(":");
      if (position == var::StringView::npos) {
        return var::StringView();
      }
      return entry.get_substring_with_length(position);
    }

    const var::StringView content(const var::StringView &entry) const {
      const size_t position = entry.find(":");
      if (position == var::StringView::npos) {
        return entry;
      }
      return entry.get_substring_at_position(position + 1);
    }

    bool is_type_valid() const;

    void generate_csv_chart(const fs::FileObject &output) const;
    void generate_csv_table(const fs::FileObject &output) const;
    void generate_histogram_chart(const fs::FileObject &output) const;
    void generate_mermaid(const fs::FileObject &output) const;
    void generate_passthrough(const fs::FileObject &output) const;
    void generate_raw(const fs::FileObject &output) const;
  };
  var::Queue<IntermediateData> m_data_list;
};

} // namespace report

#endif // REPORTAPI_REPORT_PARSER_HPP
