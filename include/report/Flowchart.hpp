#ifndef REPORTAPI_REPORT_FLOWCHART_HPP
#define REPORTAPI_REPORT_FLOWCHART_HPP

#include "Writer.hpp"

namespace report {

class Flowchart : public Writer {
public:
  Flowchart();

  static var::StringView get_class_type() { return "fc"; }
};

} // namespace report

#endif // REPORTAPI_REPORT_FLOWCHART_HPP
