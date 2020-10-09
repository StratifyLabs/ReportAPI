#ifndef REPORTAPI_REPORT_TABLE_HPP
#define REPORTAPI_REPORT_TABLE_HPP

#include "Writer.hpp"

namespace report {

class Table : public Writer {
public:
  Table();

  class Options {
    API_AC(Options, var::String, name);
    API_AC(Options, var::String, header);
  };

  static var::StringView get_class_type() { return "tab"; }

  explicit Table(const Options &options);

  Table &write_row(const var::String &value) {
    write(value);
    return *this;
  }
};

} // namespace report

#endif // REPORTAPI_REPORT_TABLE_HPP
