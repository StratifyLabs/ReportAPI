#ifndef REPORTAPI_REPORT_HISTOGRAM_HPP
#define REPORTAPI_REPORT_HISTOGRAM_HPP

#include "Writer.hpp"

namespace report {

class Histogram : public Writer {
public:
  class Options {
    API_AC(Options, var::String, name);
    API_AC(Options, var::String, labels);
  };

  Histogram(const Options &options);

  static var::StringView get_class_type() { return "hist"; }

  Histogram &write_row(const var::String &row);
};

} // namespace report

#endif // REPORTAPI_REPORT_HISTOGRAM_HPP
