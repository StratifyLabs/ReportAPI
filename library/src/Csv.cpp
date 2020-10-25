#include <printer/Printer.hpp>

#include "report/Csv.hpp"

namespace printer {
class Printer;

Printer &operator<<(Printer &printer, const report::Csv &a) {
  printer.array(a.prefix(), a.row());
  return printer;
}

} // namespace printer

using namespace report;

Csv::Csv(
  printer::Printer &printer,
  const var::StringView name,
  const var::StringViewList &header_list) {
  set_prefix(generate_prefix(get_class_type(), name));
  printer.array(prefix(), header_list);
}
