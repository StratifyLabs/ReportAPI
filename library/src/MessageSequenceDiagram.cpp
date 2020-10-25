#include "report/MessageSequenceDiagram.hpp"

namespace printer {
class Printer;

Printer &operator<<(Printer &printer, const report::MessageSequenceDiagram &a) {
  printer.key(a.prefix(), a.value());
  return printer;
}
} // namespace printer

using namespace report;

MessageSequenceDiagram::MessageSequenceDiagram(
  printer::Printer &printer,
  const var::StringView name) {
  set_prefix(generate_prefix(get_class_type(), name));
  printer.key(prefix(), "sequenceDiagram");
}
