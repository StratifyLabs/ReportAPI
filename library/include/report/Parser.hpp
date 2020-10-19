#ifndef REPORTAPI_REPORT_PARSER_HPP
#define REPORTAPI_REPORT_PARSER_HPP

#include <fs/File.hpp>
#include <var/String.hpp>
#include <var/Vector.hpp>

namespace report {

class Parser {
public:
  class Options {
    API_AC(Options, var::String, path); // path to a directory or file
  };

  Parser(const Options &options);

  Parser &parse(const var::String &input);

  int create_report();

private:
  class IntermediateData {
  public:
    bool operator==(const IntermediateData &a) const {
      return type() == a.type() && name() == a.name();
    }

    var::String get_file_path(const var::String &directory_path) {
      return directory_path + "/" + type() + "_" + name() + ".txt";
    }

  private:
    API_AC(IntermediateData, var::String, type);
    API_AC(IntermediateData, var::String, name);
    API_AC(IntermediateData, var::StringList, entry_list);
  };

  API_RAB(Parser, valid, false);       // used if path is a empty
  API_AC(Parser, var::String, output); // used if path is a empty
  var::Vector<IntermediateData>
    m_intermediate_data_list; // used if path is a file or empty
  var::String m_directory_path;
  var::String m_file_name;
  var::String m_partial_line;

  bool is_use_intermediate_data() const { return m_directory_path.is_empty(); }

  bool is_type_valid(const var::StringView class_type) const;

  Parser &parse_line(const var::String &input);
  int generate_csv_chart(const fs::File *output, const IntermediateData &data);
  int generate_csv_table(const fs::File *output, const IntermediateData &data);
  int generate_histogram_chart(
    const fs::File *output,
    const IntermediateData &data);
  int generate_passthrough(
    const fs::File *output,
    const IntermediateData &data);
  int generate_raw(const fs::File *output, const IntermediateData &data);
};

} // namespace report

#endif // REPORTAPI_REPORT_PARSER_HPP
