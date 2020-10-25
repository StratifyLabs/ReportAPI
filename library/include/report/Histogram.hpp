#ifndef REPORTAPI_REPORT_HISTOGRAM_HPP
#define REPORTAPI_REPORT_HISTOGRAM_HPP

#include "Section.hpp"

namespace report {

class Histogram : public Section {
public:
  Histogram() {} // used as a placeholder

  Histogram(
    printer::Printer &printer,
    const var::StringView name,
    const var::StringViewList header_list);

  static var::StringView get_class_type() { return "hist"; }

private:
  API_AC(Histogram, var::StringViewList, row);
};

} // namespace report

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const report::Histogram &a);
} // namespace printer

#endif // REPORTAPI_REPORT_HISTOGRAM_HPP
