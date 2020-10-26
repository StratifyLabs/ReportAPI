
#include <cmath>
#include <cstdio>

#include "chrono.hpp"
#include "fs.hpp"
#include "printer.hpp"
#include "report.hpp"
#include "sys.hpp"
#include "var.hpp"

#include "json.hpp"

#include "test/Test.hpp"

class UnitTest : public test::Test {

  using Msd = MessageSequenceDiagram;
  using Mg = MermaidGraph;

  class TestPrinter : public report::ReportPrinter {
  public:
    TestPrinter() : m_file(File::IsOverwrite::yes, "report.txt") {}
    void interface_print_final(const StringView value) override {
      m_file.write(value);
    }

    const File &file() const { return m_file; }

  private:
    File m_file;
  };
  TestPrinter m_report_printer;

  CallGraph m_call_graph;

public:
  UnitTest(var::StringView name) : test::Test(name) {}

  bool execute_class_api_case() {

    m_report_printer.file().write("Hello World\n");
    m_call_graph = CallGraph(m_report_printer, "callgraph");

    CALL_GRAPH_TRACE_FUNCTION(m_report_printer, m_call_graph);

    TEST_ASSERT(csv_case());
    TEST_ASSERT(hist_case());
    TEST_ASSERT(table_case());
    TEST_ASSERT(mermaid_case());
    TEST_ASSERT(msd_case());

    printf(
      "ouput\n%s",
      DataFile().write(File("report.txt")).data().add_null_terminator());

    TEST_ASSERT(parser_case());

    return true;
  }

  bool msd_case() {

    MessageSequenceDiagram msd(m_report_printer, "msd");

    m_report_printer << msd.set_participant("jane")
                     << msd.set_participant("sally");

    m_report_printer << msd.set_message(Msd::MessageOptions()
                                          .set_source("jane")
                                          .set_destination("sally")
                                          .set_message("hello"));

    m_report_printer << msd.set_message_arrow(Msd::MessageOptions()
                                                .set_source("sally")
                                                .set_destination("jane")
                                                .set_message("hi"));

    m_report_printer << msd.set_message(Msd::MessageOptions()
                                          .set_source("jane")
                                          .set_destination("sally")
                                          .set_message("how are you?"));

    m_report_printer << msd.set_message_arrow(Msd::MessageOptions()
                                                .set_source("sally")
                                                .set_destination("jane")
                                                .set_message("good, and you?"));

    m_report_printer << msd.set_message_dotted(Msd::MessageOptions()
                                                 .set_source("jane")
                                                 .set_destination("sally")
                                                 .set_message("great. k bye"));

    m_report_printer << msd.set_message_arrow(Msd::MessageOptions()
                                                .set_source("sally")
                                                .set_destination("jane")
                                                .set_message("bye"));

    return true;
  }

  bool mermaid_case() {
    CALL_GRAPH_TRACE_FUNCTION(m_report_printer, m_call_graph);

    MermaidGraph graph(m_report_printer, "mmd");

    Mg::Node home("Home");
    Mg::Node first("First");
    Mg::Node second("Second");
    Mg::Node third("Home");

    m_report_printer << graph.set_transition(home, Mg::Link::arrow, first);

    m_report_printer << graph.set_transition(first, Mg::Link::dotted, second);

    m_report_printer << graph.set_transition(second, Mg::Link::open, third);

    m_report_printer << graph.set_transition(third, Mg::Link::thick, home);

    return true;
  }

  bool parser_case() {
    CALL_GRAPH_TRACE_FUNCTION(m_report_printer, m_call_graph);

    TEST_ASSERT(Parser(File("report.txt"))
                  .create_report(File(File::IsOverwrite::yes, "report.md"))
                  .is_success());
    return true;
  }

  bool hist_case() {
    CALL_GRAPH_TRACE_FUNCTION(m_report_printer, m_call_graph);
    Histogram histogram(
      m_report_printer,
      "histo",
      StringViewList({"col0", "col1", "col2"}));
    m_report_printer << histogram.set_row({"10", "20", "5"});
    return true;
  }

  bool table_case() {
    CALL_GRAPH_TRACE_FUNCTION(m_report_printer, m_call_graph);
    Table trig(m_report_printer, "trig", StringViewList({"x", "y"}));
    for (u32 i = 0; i < 20; i++) {
      float time = 2.0f * i * MCU_PI_FLOAT / 20.0f;
      m_report_printer << trig.set_row(
        String().format("%0.3f,%0.3f", sinf(time), cosf(time)).split(","));
    }
    return true;
  }

  bool csv_case() {
    CALL_GRAPH_TRACE_FUNCTION(m_report_printer, m_call_graph);

    Csv dummy0(
      m_report_printer,
      "dummy",
      StringViewList({"x", "col1", "col2"}));

    m_report_printer << dummy0.set_row({"1", "1", "2"});
    m_report_printer << dummy0.set_row({"2", "4", "5"});
    m_report_printer << dummy0.set_row({"3", "7", "8"});

    Csv trig(m_report_printer, "trig", StringViewList({"t", "sin", "cos"}));
    for (u32 i = 0; i < 20; i++) {
      float time = 2.0f * i * MCU_PI_FLOAT / 20.0f;
      m_report_printer << trig.set_row(
        String()
          .format("%0.3f,%0.3f,%0.3f", time, sinf(time), cosf(time))
          .split(","));
      m_report_printer << dummy0.set_row(
        NumberString()
          .format("%d,%d,%d", i + 4, i * 2, i * 3)
          .string_view()
          .split(","));
    }

    return true;
  }

private:
};
