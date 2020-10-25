

#include "report/Mermaid.hpp"

namespace printer {
class Printer;

Printer &operator<<(Printer &printer, const report::MermaidGraph &a) {
  printer.array(a.prefix(), a.get_transition_string());
  return printer;
} // namespace printer

} // namespace printer

using namespace report;

u32 Mermaid::Node::m_count = 0;

Mermaid::Mermaid(printer::Printer &printer, const Construct &options) {
  set_prefix(generate_prefix(get_class_type(), options.name()));
  printer.object(prefix(), options.diagram());
}

MermaidGraph::MermaidGraph(
  printer::Printer &printer,
  const var::StringView name,
  const var::StringView direction)
  : Mermaid(
    printer,
    Mermaid::Construct().set_name(name).set_diagram(
      var::KeyString("graph ").append(direction))) {}

Mermaid::Node::Node(const var::StringView text, Shape shape) {
  switch (shape) {
  case Shape::square:
    m_node = var::KeyString().format("n%d[%s]", m_count++, text);
    return;
  case Shape::round:
    m_node = var::KeyString().format("n%d(%s)", m_count++, text);
    return;
  case Shape::stadium:
    m_node = var::KeyString().format("n%d([%s])", m_count++, text);
    return;
  case Shape::subroutine:
    m_node = var::KeyString().format("n%d[[%s]]", m_count++, text);
    return;
  case Shape::cylinder:
    m_node = var::KeyString().format("n%d[(%s)]", m_count++, text);
    return;
  case Shape::circle:
    m_node = var::KeyString().format("n%d((%s))", m_count++, text);
    return;
  case Shape::asymmetric:
    m_node = var::KeyString().format("n%d>%s]", m_count++, text);
    return;
  case Shape::rhombus:
    m_node = var::KeyString().format("n%d{%s}", m_count++, text);
    return;
  case Shape::hexagon:
    m_node = var::KeyString().format("n%d{{%s}}", m_count++, text);
    return;
  case Shape::parallelogram:
    m_node = var::KeyString().format("n%d[/%s/]", m_count++, text);
    return;
  case Shape::parallelogram_alternate:
    m_node = var::KeyString().format("n%d[\\%s\\]", m_count++, text);
    return;
  case Shape::trapezoid:
    m_node = var::KeyString().format("n%d[/%s\\]", m_count++, text);
    return;
  case Shape::trapezoid_alternate:
    m_node = var::KeyString().format("n%d[\\%s/]", m_count++, text);
    return;
  }
}

var::StringView MermaidGraph::get_link_string(Link link) const {
  switch (link) {
  case Link::arrow:
    return "-->";
  case Link::open:
    return " --- ";
  case Link::dotted:
    return "-.->";
  case Link::thick:
    return "==>";
  }
  return "-->";
}

MermaidGraph &MermaidGraph::set_transition(
  const Node &from,
  Link link_arrow,
  const Node &to,
  const var::StringView message) {
  API_ASSERT(prefix().is_empty() == false);
  return set_from(from).set_to(to).set_link(link_arrow).set_message(message);
}

var::GeneralString MermaidGraph::get_transition_string() const {
  API_ASSERT(prefix().is_empty() == false);
  return std::move(var::GeneralString("  ")
                     .append(m_from.node())
                     .append(get_link_string(m_link))
                     .append(
                       m_message.is_empty()
                         ? var::KeyString()
                         : (var::KeyString("|").append(m_message).append("|")))
                     .append(m_to.node()));
}

CallGraph &
CallGraph::push(printer::Printer &printer, const var::StringView name) {
  Mermaid::Node node(name, Mermaid::Shape::round);
  if (m_call_stack.count()) {
    printer << m_mermaid_graph.set_transition(
      m_call_stack.top(),
      MermaidGraph::Link::arrow,
      node,
      var::NumberString(m_transition_counter++));
  }
  m_call_stack.push(node);
  return *this;
}

CallGraph &
CallGraph::data(printer::Printer &printer, const var::StringView data) {
  if (m_call_stack.count()) {
    printer << m_mermaid_graph.set_transition(
      m_call_stack.top(),
      report::MermaidGraph::Link::arrow,
      Mermaid::Node(data, Mermaid::Shape::cylinder),
      var::NumberString(m_transition_counter++));
  }
  return *this;
}
