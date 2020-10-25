#ifndef REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP
#define REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP

#include "Section.hpp"

namespace report {

class MessageSequenceDiagram : public Section {
public:
  MessageSequenceDiagram() {}
  MessageSequenceDiagram(printer::Printer &printer, const var::StringView name);

  static var::StringView get_class_type() { return "mmd"; }

  class MessageOptions {
    API_AC(MessageOptions, var::StringView, source);
    API_AC(MessageOptions, var::StringView, destination);
    API_AC(MessageOptions, var::StringView, message);
  };

  MessageSequenceDiagram &set_participant(const var::StringView name) {
    return set_value(var::PathString("  participant ").append(name));
  }

  MessageSequenceDiagram &set_message(const MessageOptions &options) {
    return set_value(var::PathString("  ")
                       .append(options.source())
                       .append("-->")
                       .append(options.destination())
                       .append(":")
                       .append(options.message()));
  }

  MessageSequenceDiagram &set_message_arrow(const MessageOptions &options) {
    return set_value(var::PathString("  ")
                       .append(options.source())
                       .append("->>")
                       .append(options.destination())
                       .append(":")
                       .append(options.message()));
  }

  MessageSequenceDiagram &set_message_dotted(const MessageOptions &options) {
    return set_value(var::PathString("  ")
                       .append(options.source())
                       .append("-->")
                       .append(options.destination())
                       .append(":")
                       .append(options.message()));
  }

private:
  API_AC(MessageSequenceDiagram, var::PathString, value);
};

} // namespace report

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const report::MessageSequenceDiagram &a);
} // namespace printer

#endif // REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP
