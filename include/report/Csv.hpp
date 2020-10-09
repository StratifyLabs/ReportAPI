#ifndef REPORTAPI_REPORT_CSV_HPP
#define REPORTAPI_REPORT_CSV_HPP

#include "Writer.hpp"

namespace report {

class Csv : public WriterAccess<Csv> {
public:
  class Options {
    API_AC(Options, var::String, name);
    API_AC(Options, var::String, header);
  };

  static var::StringView get_class_type() { return "csv"; }

  explicit Csv(const Options &options);

  Csv &write_row(const var::String &value) {
    write(value);
    return *this;
  }

private:
};

} // namespace report

#endif // REPORTAPI_REPORT_CSV_HPP
