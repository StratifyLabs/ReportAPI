
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
public:
  UnitTest(var::StringView name) : test::Test(name) {}

  bool execute_class_api_case() {

    TEST_ASSERT(csv_case());
    TEST_ASSERT(hist_case());
    TEST_ASSERT(table_case());

    printf(
      "ouput\n%s",
      DataFile().write(File("report.txt")).data().add_null_terminator());

    TEST_ASSERT(parser_case());

    return true;
  }

  bool parser_case() {
    TEST_ASSERT(Parser(File("report.txt"))
                  .create_report(File(File::IsOverwrite::yes, "report.md"))
                  .is_success());
    return true;
  }

  bool hist_case() {
    Histogram histogram(
      report_printer,
      "histo",
      StringViewList({"col0", "col1", "col2"}));
    report_printer << histogram.set_row({"10", "20", "5"});
    return true;
  }

  bool table_case() {
    Table trig(report_printer, "trig", StringViewList({"x", "y"}));
    for (u32 i = 0; i < 20; i++) {
      float time = 2.0f * i * MCU_PI_FLOAT / 20.0f;
      report_printer << trig.set_row(
        String().format("%0.3f,%0.3f", sinf(time), cosf(time)).split(","));
    }
    return true;
  }

  bool csv_case() {

    Csv dummy0(report_printer, "dummy", StringViewList({"x", "col1", "col2"}));

    report_printer << dummy0.set_row({"1", "1", "2"});
    report_printer << dummy0.set_row({"2", "4", "5"});
    report_printer << dummy0.set_row({"3", "7", "8"});

    Csv trig(report_printer, "trig", StringViewList({"t", "sin", "cos"}));
    for (u32 i = 0; i < 20; i++) {
      float time = 2.0f * i * MCU_PI_FLOAT / 20.0f;
      report_printer << trig.set_row(
        String()
          .format("%0.3f,%0.3f,%0.3f", time, sinf(time), cosf(time))
          .split(","));
      report_printer << dummy0.set_row(
        NumberString()
          .format("%d,%d,%d", i + 4, i * 2, i * 3)
          .string_view()
          .split(","));
    }

    return true;
  }

private:
  class TestPrinter : public report::ReportPrinter {
  public:
    TestPrinter() : m_file(File::IsOverwrite::yes, "report.txt") {}
    void interface_print_final(const StringView value) override {
      m_file.write(value);
    }

  private:
    File m_file;
  };
  TestPrinter report_printer;
};
