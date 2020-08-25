#include <sapi/fs.hpp>
#include <sapi/var.hpp>
#include <ChartAPI/chart.hpp>

#include "report/Csv.hpp"
#include "report/Table.hpp"
#include "report/Flowchart.hpp"
#include "report/Katex.hpp"
#include "report/MessageSequenceDiagram.hpp"
#include "report/Mermaid.hpp"
#include "report/Table.hpp"
#include "report/Parser.hpp"

using namespace report;

Parser::Parser(const Options& options){
	if( options.path().is_empty() ){
		//just use internal data
		m_is_valid = true;
	} else {
		FileInfo info = File::get_info(options.path());
		if( info.is_directory() ){
			m_file_name = FileInfo::name(options.path()) + ".md";
			m_directory_path = options.path();
			m_is_valid = true;
		} else {
			m_file_name = options.path();
			m_is_valid = true;
		}
	}
}

bool Parser::is_type_valid(const var::String class_type) const {
	return class_type ==	Mermaid::get_class_type()
			|| class_type == Flowchart::get_class_type()
			|| class_type == MessageSequenceDiagram::get_class_type()
			|| class_type == Katex::get_class_type()
			|| class_type == Csv::get_class_type()
			|| class_type == Table::get_class_type()
			|| class_type == "";
}

Parser& Parser::parse(const var::String & input){
	if( input.length() == 0 ){
		return *this;
	}

	StringList line_list = input.split("\r\n");
	u32 count = line_list.count();

	if( input.back() == '\n' || count > 1 ){
		parse_line(m_partial_line + line_list.at(0) + "\n");
		m_partial_line.clear();
	}

	if( input.back() != '\n' ){
		//last line is partial
		m_partial_line = line_list.back();
		count--;
	}

	for(u32 i=1; i < count; i++){
		parse_line(line_list.at(i) + "\n");
	}

	return *this;
}

Parser& Parser::parse_line(const var::String & line){
	if( is_valid() == false ){
		return *this;
	}

	StringList input_list =
			Tokenizer(line,
								Tokenizer::ParseOptions().set_delimeters(":")
								.set_maximum_delimeter_count(2)
								).list();

	if( input_list.count() == 0 ){
		return *this;
	}

	String name;
	String type;
	String value;

	if( input_list.count() != 3 ||
			!is_type_valid(input_list.at(0)) ){

		if( line.length() > 1 ){
			name = "raw";
			type = "";
			value = line;
		}
	} else {
		type = input_list.at(0);
		name = input_list.at(1);
		value = input_list.at(2);
	}

	IntermediateData data =
			IntermediateData().set_name(name).set_type(type);

	if( is_use_intermediate_data() ){
		u32 offset = m_intermediate_data_list.find(data);
		if( offset == m_intermediate_data_list.count() ){
			//data was not found -- add it to the end
			m_intermediate_data_list.push_back(data);
		}
		m_intermediate_data_list.at(offset).entry_list().push_back(value);
	} else {
		//write the entry to a txt file
		String path = data.get_file_path(m_directory_path);
		File output_file;
		if( File::exists(path) ){
			output_file.open(path, OpenFlags::append_write_only());
		} else {
			output_file.create(path, File::IsOverwrite(false));
		}
		output_file.write(value);
	}

	return *this;
}

int Parser::create_report(){

	File output_file;
	DataFile output_data_file(OpenFlags::append_read_write());

	File * file;
	if( m_file_name.is_empty() ){
		file = &output_data_file;
	} else {
		file = &output_file;
		String output_file_path =
				m_directory_path.is_empty() ?
					m_file_name :
					m_directory_path + "/" + m_file_name + ".md";

		if( output_file.create(output_file_path, File::IsOverwrite(false)) < 0 ){
			return -1;
		}
	}

	if( is_use_intermediate_data() ){

		for(const IntermediateData & data: m_intermediate_data_list){

			if( data.type() == Csv::get_class_type() ){
				generate_csv_chart(file, data);
			} else if( data.type() == Table::get_class_type() ){
				generate_csv_table(file, data);
			} else {
				generate_passthrough(file, data);
			}

			output_file.write("\n");

		}

	} else {
		//load the files

	}

	m_output = String(output_data_file.data());

	return 0;
}

int Parser::generate_passthrough(const fs::File * output, const IntermediateData & data){
	//most formats are just passthrough data

	if( !data.name().is_empty() || !data.type().is_empty() ){

		output->write("**" + data.name() + "**\n\n");

		output->write("```" + data.type() + "\n");
		for(const String & entry: data.entry_list()){
			output->write(entry);
		}
		output->write("```\n\n");
	}
	return 0;
}


int Parser::generate_csv_table(const fs::File * output, const IntermediateData & data){

	if( data.entry_list().count() == 0 ){
		output->write("> No data is available\n\n");
		return 0;
	}

	StringList header_list = data.entry_list().front().split(",");

	{
		String line = "| ";
		for(const String& item: header_list){
			line += item + " |";
		}
		line += "\n";
		output->write( line );
	}

	{
		String line = "|";
		for(const String& item: header_list){
			line +=  "-------|";
		}
		line += "\n";
		output->write( line );
	}

	for(u32 i=1; i < data.entry_list().count(); i++){
		String line = "|";
		StringList item_list = data.entry_list().at(i).split(",");
		for(const String& item: item_list){
			line += item + " |";
		}
		line += "\n\n";
		output->write( line );
	}


	return 0;
}


int Parser::generate_csv_chart(const fs::File * output, const IntermediateData & data){
	//csv is not trivially handled -- needs some parsing

	//no data here
	if( data.entry_list().count() == 0 ){
		output->write("> No data is available\n\n");
		return 0;
	}

	StringList header_list = data.entry_list().front().split(",");

	if( header_list.count() < 2 ){
		output->write("> No data is available\n\n");
		return 0;
	}

	ChartJs chart;
	chart.set_type(ChartJs::type_line);

	Array<String, 2> x_extrema;

	u32 color = 0;
	for (u32 i=1; i < header_list.count(); i++) {

		ChartJsDataSet data_set;

		data_set
				.set_label(header_list.at(i))
				.set_border_color(ChartJsColor::get_standard(color++).set_alpha(128))
				.set_background_color(ChartJsColor().set_alpha(0));

		for(u32 j=1; j < data.entry_list().count(); j++) {
			StringList data_list = data.entry_list().at(j).split(",");
			if( x_extrema.at(0).is_empty() ){
				x_extrema.at(0) = data_list.at(0);
			}

			x_extrema.at(1) = data_list.at(0);

			data_set.append(
						ChartJsStringDataPoint()
						.set_x(data_list.at(0))
						.set_y(data_list.at(i))
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
					.set_scale_label(
						ChartJsScaleLabel().set_display(true).set_label(header_list.at(0))))
				.append_y_axis(
					ChartJsAxis()
					.set_type(ChartJsAxis::type_linear)
					.set_display(true)))
			.set_legend(ChartJsLegend().set_display(true))
			.set_title(ChartJsTitle().set_display(true).set_text(data.name() + "(" + data.type() +")"));

	output->write("\n\n**" + data.name() + "**\n\n```chartjs\n");
	output->write(JsonDocument().stringify(chart.to_object()));
	output->write("\n```\n\n");

	return 0;
}
