#include <sapi/fs.hpp>
#include <sapi/var.hpp>
#include <ChartAPI/chart.hpp>

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

Parser& Parser::parse(const var::String & line){
	if( is_valid() == false ){
		return *this;
	}

	StringList input_list = Tokenizer(line,
											Tokenizer::ParseOptions().set_delimeters(":")
											.set_maximum_token_count(3)
											).list();

	if( input_list.count() != 3 ){
		return *this;
	}

	const String & value = input_list.at(2);
	IntermediateData data =
			IntermediateData().set_name(input_list.at(0)).set_type(input_list.at(1));

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

			if( data.type() == "csv"){

			} else {
				generate_passthrough(file, data);
			}

		}

	} else {
		//load the files

	}

	m_output = String(output_data_file.data());

	return 0;
}

int Parser::generate_passthrough(const fs::File * output, const IntermediateData & data){
	//most formats are just passthrough data

	output->write("**" + data.name() + "**\n\n");

	output->write("```" + data.type() + "\n");
	for(const String & entry: data.entry_list()){
		output->write(entry);
	}
	output->write("```\n\n");
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

	u32 color = 0;
	for (u32 i=1; i < header_list.count(); i++) {

		ChartJsDataSet data_set;

		data_set
				.set_label(header_list.at(i))
				.set_border_color(ChartJsColor::get_standard(color++).set_alpha(128));

		for(u32 j=1; j < data.entry_list().count(); j++) {
			data_set.append(
				ChartJsStringDataPoint()
					.set_x(data.entry_list().at(0))
					.set_y(data.entry_list().at(j))
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
						.set_scale_label(
							ChartJsScaleLabel().set_display(true).set_label(header_list.at(0))))
				.append_y_axis(
					ChartJsAxis()
						.set_type(ChartJsAxis::type_linear)
						.set_display(true)
						.set_ticks(
							ChartJsAxisTicks().set_minimum(0).set_maximum(100).set_step_size(
								10))
						.set_scale_label(ChartJsScaleLabel().set_display(true).set_label(
							"Utilization Percentage"))))
		.set_legend(ChartJsLegend().set_display(true))
		.set_title(ChartJsTitle().set_display(true).set_text("Task Analysis"));

	output->write(JsonDocument().stringify(chart.to_object()));

	return 0;
}
