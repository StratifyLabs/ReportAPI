#ifndef REPORTAPI_REPORT_MERMAID_HPP
#define REPORTAPI_REPORT_MERMAID_HPP

#include <var/Stack.hpp>
#include <var/StringView.hpp>

#include "Section.hpp"

namespace report {

class Mermaid : public Section {
public:
  enum class Link { arrow, open, dotted, thick };

  enum class Shape {
    square,
    round,
    stadium,
    subroutine,
    cylinder,
    circle,
    asymmetric,
    rhombus,
    hexagon,
    parallelogram,
    parallelogram_alternate,
    trapezoid,
    trapezoid_alternate
  };

  class Node {
  public:
    Node() {}
    Node(const var::StringView text, Shape shape = Shape::round);

  private:
    API_RAC(Node, var::KeyString, node);
    static u32 m_count;
  };

  class Construct {
    API_AC(Construct, var::StringView, name);
    API_AC(Construct, var::StringView, diagram);
    API_AF(Construct, Section *, writer, nullptr);
  };

  Mermaid() {}
  Mermaid(printer::Printer &printer, const Construct &options);

  static var::StringView get_class_type() { return "mmd"; }
};

class MermaidGraph : public Mermaid {
public:
  MermaidGraph() {}

  MermaidGraph(
    printer::Printer &printer,
    const var::StringView name,
    const var::StringView direction = "LR");

  class Transition {
    API_AC(Transition, Node, first);
    API_AC(Transition, Node, second);
    API_AC(Transition, var::String, message);
    API_AF(Transition, Link, link, Link::arrow);
  };

  MermaidGraph &set_transition(
    const Node &from,
    Link link_arrow,
    const Node &to,
    const var::StringView message = var::StringView());

  var::GeneralString get_transition_string() const;

private:
  u32 m_sequence_count;
  API_AC(MermaidGraph, Node, from);
  API_AC(MermaidGraph, Node, to);
  API_AC(MermaidGraph, var::KeyString, message);
  API_AF(MermaidGraph, Link, link, Link::arrow);

  var::StringView get_link_string(Link link) const;
};

class CallGraph {
public:
  CallGraph() {}
  explicit CallGraph(printer::Printer &printer, const var::StringView name)
    : m_mermaid_graph(printer, name, "LR") {}

  CallGraph &push(printer::Printer &printer, const var::StringView name);
  CallGraph &data(printer::Printer &printer, const var::StringView data);

  CallGraph &pop() {
    if (m_call_stack.count()) {
      m_call_stack.pop();
    }
    return *this;
  }

  MermaidGraph &mermaid_graph() { return m_mermaid_graph; }

private:
  var::Stack<Mermaid::Node> m_call_stack;
  MermaidGraph m_mermaid_graph;
  u32 m_transition_counter = 0;
};

class CallGraphNode {
public:
  CallGraphNode(
    printer::Printer &printer,
    CallGraph &graph,
    const var::StringView name)
    : m_graph(graph) {
    graph.push(printer, name);
  }

  ~CallGraphNode() { m_graph.pop(); }

private:
  CallGraph &m_graph;
};

} // namespace report

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const report::MermaidGraph &a);
} // namespace printer

#define CALL_GRAPH_TRACE_FUNCTION(printer, graph)                              \
  report::CallGraphNode call_graph_node                                        \
    = report::CallGraphNode(printer, graph, __FUNCTION__)

#endif // REPORTAPI_REPORT_MERMAID_HPP
