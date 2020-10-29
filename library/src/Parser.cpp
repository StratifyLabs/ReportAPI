#include <chart.hpp>
#include <fs.hpp>
#include <json.hpp>
#include <var.hpp>

#include "report/Csv.hpp"
#include "report/Flowchart.hpp"
#include "report/Histogram.hpp"
#include "report/Katex.hpp"
#include "report/Mermaid.hpp"
#include "report/MessageSequenceDiagram.hpp"
#include "report/Parser.hpp"
#include "report/Table.hpp"

using namespace report;

Parser::Parser(const fs::File &input) {
  DataFile f = DataFile().reserve(input.size()).write(input).seek(0).move();
  bool is_done = false;
  do {
    const String line = f.gets();
    const IntermediateData line_data(line);
    if (m_data_list.find_offset(line_data) == m_data_list.count()) {
      m_data_list.push(line_data);
    }

    for (auto &data : m_data_list) {
      data.push(line);
    }
    is_done = line.is_empty();
  } while (is_done == false);
}

Parser &Parser::create_report(const fs::File &output) {
  printf("gen report\n");
  for (const auto &data : m_data_list) {
    data.generate(output);
  }
  return *this;
}

bool Parser::IntermediateData::is_type_valid() const {
  return type() == Mermaid::get_class_type()
         || type() == Flowchart::get_class_type()
         || type() == MessageSequenceDiagram::get_class_type()
         || type() == Katex::get_class_type() || type() == Csv::get_class_type()
         || type() == Histogram::get_class_type()
         || type() == Table::get_class_type() || type() == "";
}

void Parser::IntermediateData::generate(const fs::File &output) const {
  if (is_type_valid() == false) {
    return;
  }

  if (type() == Csv::get_class_type()) {
    generate_csv_chart(output);
  } else if (type() == Table::get_class_type()) {
    generate_csv_table(output);
  } else if (type() == Histogram::get_class_type()) {
    generate_histogram_chart(output);
  } else if (type() == "mmd") {
    generate_mermaid(output);
  } else {
    generate_passthrough(output);
  }
}

void Parser::IntermediateData::generate_mermaid(const fs::File &output) const {
  output.write(String("**") + name() + "**\n\n").write("```mermaid\n");

  for (const auto entry : entry_list()) {
    output.write(content(entry));
    output.write("\n");
  }

  output.write(String("```\n\n"));
}

void Parser::IntermediateData::generate_passthrough(
  const fs::File &output) const {
  output.write(String("**") + name() + "**\n\n")
    .write(String("```") + type() + "\n");

  for (const auto entry : entry_list()) {
    output.write(content(entry));
    output.write("\n");
  }

  output.write(String("```\n\n"));
}

void Parser::IntermediateData::generate_csv_table(
  const fs::File &output) const {

  output.write(String("**") + name() + "**\n\n");

  bool is_header_written = false;
  String line;

  for (const auto &entry : entry_list()) {
    const StringViewList list = content(entry).split(",");
    if (is_header_written == false) {
      is_header_written = true;
      {
        String line = String("| ");
        for (const auto item : list) {
          line += item + " |";
        }
        line += "\n";
        output.write(line);
      }

      {
        String line = String("|");
        for (const auto item : list) {
          MCU_UNUSED_ARGUMENT(item);
          line += "-------|";
        }
        line += "\n";
        output.write(line);
      }
    } else {
      line = "| ";
      for (const auto item : list) {
        line += item + " |";
      }
      line += "\n";
      output.write(line);
    }
  }

  output.write("\n\n");
}

void Parser::IntermediateData::generate_csv_chart(
  const fs::File &output) const {
  // csv is not trivially handled -- needs some parsing

  StringViewList header_list = content(entry_list().front()).split(",");

  ChartJs chart;
  chart.set_type(ChartJs::type_line);

  Array<String, 2> x_extrema;

  u32 color = 0;

  for (u32 i = 1; i < header_list.count(); i++) {

    ChartJsDataSet data_set;

    data_set.set_label(String(header_list.at(i)))
      .set_border_color(ChartJsColor::get_standard(color++).set_alpha(128))
      .set_background_color(ChartJsColor().set_alpha(0));

    for (u32 j = 1; j < entry_list().count(); j++) {
      const StringViewList data_list = content(entry_list().at(j)).split(",");
      if (x_extrema.at(0).is_empty()) {
        x_extrema.at(0) = String(data_list.at(0));
      }

      x_extrema.at(1) = String(data_list.at(0));

      data_set.append(ChartJsStringDataPoint()
                        .set_x(var::String(data_list.at(0)))
                        .set_y(var::String(data_list.at(i)))
                        .to_object());
    }

    chart.data().append(data_set);
  }

  chart.options()
    .set_scales(
      ChartJsScales()
        .append_x_axis(
          ChartJsAxis()
            .set_display(true)
            .set_type(ChartJsAxis::type_linear)
            .set_ticks(ChartJsAxisTicks()
                         .set_minimum(x_extrema.at(0).to_float())
                         .set_maximum(x_extrema.at(1).to_float())
                         .set_step_size(0.1f))
            .set_scale_label(ChartJsScaleLabel().set_display(true).set_label(
              String(header_list.at(0)))))
        .append_y_axis(
          ChartJsAxis().set_type(ChartJsAxis::type_linear).set_display(true)))
    .set_legend(ChartJsLegend().set_display(true))
    .set_title(
      ChartJsTitle().set_display(true).set_text(name() + "(" + type() + ")"));

  output.write(String("\n\n**") + name() + "**\n\n```chart\n");
  output.write(View(json::JsonDocument().stringify(chart.to_object())));
  output.write("\n```\n\n");
}

void Parser::IntermediateData::generate_histogram_chart(
  const fs::File &output) const {
  // no data here
  if (entry_list().count() < 1) {
    output.write(StringView("> No data is available\n\n"));
    return;
  }

  StringViewList header_list = content(entry_list().front()).split(",");
  if (header_list.count() < 1) {
    output.write(StringView("> No data is available\n\n"));
    return;
  }

  ChartJs chart;
  chart.set_type(ChartJs::type_bar);
  for (const auto value : header_list) {
    chart.data().label_list().push_back(String(value));
  }

  for (u32 i = 1; i < entry_list().count(); i++) {
    ChartJsDataSet data_set;
    data_set.set_label(name())
      .set_border_color(ChartJsColor::get_standard(i).set_alpha(255))
      .set_background_color(ChartJsColor::get_standard(i).set_alpha(128));
    StringViewList value_list = content(entry_list().at(i)).split(",");

    for (const auto value : value_list) {
      data_set.append(JsonString(value));
    }
    chart.data().append(data_set);
  }

  chart.options()
    .set_scales(
      ChartJsScales()
        .append_x_axis(
          ChartJsAxis()
            .set_display(true)
            .set_type(ChartJsAxis::type_category)
            .set_scale_label(
              ChartJsScaleLabel().set_display(true).set_label(String("bin"))))
        .append_y_axis(
          ChartJsAxis().set_type(ChartJsAxis::type_linear).set_display(true)))
    .set_legend(ChartJsLegend().set_display(true))
    .set_title(
      ChartJsTitle().set_display(true).set_text(name() + "(" + type() + ")"));

  output.write(String("\n\n**") + name() + "**\n\n```chart\n");
  output.write(View(json::JsonDocument().stringify(chart.to_object())));
  output.write(String("\n```\n\n"));
}
