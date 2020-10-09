#ifndef REPORTAPI_REPORT_KATEX_HPP
#define REPORTAPI_REPORT_KATEX_HPP

#include "Writer.hpp"

namespace report {

class Katex : public Writer {
public:
  Katex();

  static var::StringView get_class_type() { return "kt"; }
};

} // namespace report

#endif // REPORTAPI_REPORT_KATEX_HPP
