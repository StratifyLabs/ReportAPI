#include <ChartAPI/chart.hpp>
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

Parser::Parser(const Options &options) {
  if (options.path().is_empty()) {
    // just use internal data
    m_is_valid = true;
  } else {
    FileInfo info = FileSystem().get_info(options.path());
    if (info.is_directory()) {
      m_file_name = Path::name(options.path()) + ".md";
      m_directory_path = String(options.path());
      m_is_valid = true;
    } else {
      m_file_name = String(options.path());
      m_is_valid = true;
    }
  }
}

bool Parser::is_type_valid(const var::StringView class_type) const {
  return class_type == Mermaid::get_class_type()
         || class_type == Flowchart::get_class_type()
         || class_type == MessageSequenceDiagram::get_class_type()
         || class_type == Katex::get_class_type()
         || class_type == Csv::get_class_type()
         || class_type == Histogram::get_class_type()
         || class_type == Table::get_class_type() || class_type == "";
}

Parser &Parser::parse(const var::StringView input) {
  if (input.length() == 0) {
    return *this;
  }

  StringViewList line_list = input.split("\r\n");
  u32 count = line_list.count();

  if (input.back() == '\n' || count > 1) {
    parse_line(m_partial_line + line_list.at(0) + "\n");
    m_partial_line.clear();
  }

  if (input.back() != '\n') {
    // last line is partial
    m_partial_line = String(line_list.back());
    count--;
  }

  for (u32 i = 1; i < count; i++) {
    parse_line(line_list.at(i) + "\n");
  }

  return *this;
}

Parser &Parser::parse_line(const var::String &line) {
  if (is_valid() == false) {
    return *this;
  }

  StringViewList input_list
    = Tokenizer(
        line,
        Tokenizer::Construct().set_delimeters(":").set_maximum_delimeter_count(
          2))
        .list();

  if (input_list.count() == 0) {
    return *this;
  }

  StringView name;
  StringView type;
  StringView value;

  if (input_list.count() != 3 || !is_type_valid(input_list.at(0))) {

    if (line.length() > 1) {
      name = "raw";
      type = "";
      value = line;
    }
  } else {
    type = input_list.at(0);
    name = input_list.at(1);
    value = input_list.at(2);
  }

  IntermediateData data
    = IntermediateData().set_name(String(name)).set_type(String(type));

  if (is_use_intermediate_data()) {

    const size_t offset = m_intermediate_data_list.find_offset(data);
    if (offset == m_intermediate_data_list.count()) {
      // data was not found -- add it to the end
      m_intermediate_data_list.push_back(data);
    }

    m_intermediate_data_list.at(offset).entry_list().push_back(String(value));

  } else {
    // write the entry to a txt file
    PathString path = data.get_file_path(m_directory_path);
    if (FileSystem().exists(path)) {
      File(path, OpenMode::append_write_only()).write(value);
    } else {
      File(File::IsOverwrite(false), path).write(value);
    }
  }

  return *this;
}

int Parser::create_report() {

  const String output_file_path
    = m_directory_path.is_empty()
        ? m_file_name
        : m_directory_path + "/" + m_file_name + ".md";

  DataFile output_data_file(OpenMode::append_read_write());

  File output_file = File(File::IsOverwrite::no, output_file_path);

  const File *file;
  if (output_file.is_error()) {
    output_file.reset_error();
    file = &output_data_file;
  } else {
    file = &output_file;
  }

  if (is_use_intermediate_data()) {
    for (const IntermediateData &data : m_intermediate_data_list) {
      if (data.type() == Csv::get_class_type()) {
        generate_csv_chart(file, data);
      } else if (data.type() == Table::get_class_type()) {
        generate_csv_table(file, data);
      } else if (data.type() == Histogram::get_class_type()) {
        generate_histogram_chart(file, data);
      } else {
        generate_passthrough(file, data);
      }
      output_file.write("\n");
    }

  } else {
    // load the files
  }

  m_output = String(output_data_file.data());

  return 0;
}

int Parser::generate_passthrough(
  const fs::File *output,
  const IntermediateData &data) {
  // most formats are just passthrough data

  if (!data.name().is_empty() || !data.type().is_empty()) {

    output->write(String("**") + data.name() + "**\n\n")
      .write(String("```") + data.type() + "\n");
    for (const String &entry : data.entry_list()) {
      output->write(entry);
    }
    output->write(String("```\n\n"));
  }
  return 0;
}

int Parser::generate_csv_table(
  const fs::File *output,
  const IntermediateData &data) {

  if (data.entry_list().count() == 0) {
    output->write(StringView("> No data is available\n\n"));
    return 0;
  }

  const StringViewList header_list = data.entry_list().front().split(",");

  {
    String line = String("| ");
    for (const auto item : header_list) {
      line += item + " |";
    }
    line += "\n";
    output->write(line);
  }

  {
    String line = String("|");
    for (const auto item : header_list) {
      MCU_UNUSED_ARGUMENT(item);
      line += "-------|";
    }
    line += "\n";
    output->write(line);
  }

  for (u32 i = 1; i < data.entry_list().count(); i++) {
    String line = String("|");
    const StringViewList item_list = data.entry_list().at(i).split(",");
    for (const auto item : item_list) {
      line += item + " |";
    }
    line += "\n\n";
    output->write(line);
  }

  return 0;
}

int Parser::generate_csv_chart(
  const fs::File *output,
  const IntermediateData &data) {
  // csv is not trivially handled -- needs some parsing

  // no data here
  if (data.entry_list().count() == 0) {
    output->write(StringView("> No data is available\n\n"));
    return 0;
  }

  StringViewList header_list = data.entry_list().front().split(",");

  if (header_list.count() < 2) {
    output->write(StringView("> No data is available\n\n"));
    return 0;
  }

  ChartJs chart;
  chart.set_type(ChartJs::type_line);

  Array<String, 2> x_extrema;

  u32 color = 0;
  for (u32 i = 1; i < header_list.count(); i++) {

    ChartJsDataSet data_set;

    data_set.set_label(String(header_list.at(i)))
      .set_border_color(ChartJsColor::get_standard(color++).set_alpha(128))
      .set_background_color(ChartJsColor().set_alpha(0));

    for (u32 j = 1; j < data.entry_list().count(); j++) {
      const StringViewList data_list = data.entry_list().at(j).split(",");
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
    .set_title(ChartJsTitle().set_display(true).set_text(
      data.name() + "(" + data.type() + ")"));

  output->write(String("\n\n**") + data.name() + "**\n\n```chartjs\n");
  output->write(View(json::JsonDocument().stringify(chart.to_object())));
  output->write("\n```\n\n");

  return 0;
}

int Parser::generate_histogram_chart(
  const fs::File *output,
  const IntermediateData &data) {
  // no data here
  if (data.entry_list().count() < 1) {
    output->write(StringView("> No data is available\n\n"));
    return 0;
  }

  StringViewList header_list = data.entry_list().front().split(",");
  if (header_list.count() < 1) {
    output->write(StringView("> No data is available\n\n"));
    return 0;
  }

  ChartJs chart;
  chart.set_type(ChartJs::type_bar);
  for (const auto value : header_list) {
    chart.data().label_list().push_back(String(value));
  }

  for (u32 i = 1; i < data.entry_list().count(); i++) {
    ChartJsDataSet data_set;
    data_set.set_label(data.name())
      .set_border_color(ChartJsColor::get_standard(i).set_alpha(255))
      .set_background_color(ChartJsColor::get_standard(i).set_alpha(128));
    StringViewList value_list = data.entry_list().at(i).split(",");

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
    .set_title(ChartJsTitle().set_display(true).set_text(
      data.name() + "(" + data.type() + ")"));

  output->write(String("\n\n**") + data.name() + "**\n\n```chartjs\n");
  output->write(View(json::JsonDocument().stringify(chart.to_object())));
  output->write(String("\n```\n\n"));

  return 0;
}