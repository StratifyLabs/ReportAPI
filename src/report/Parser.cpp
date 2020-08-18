#include <sapi/fs.hpp>
#include <sapi/var.hpp>

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
		u32 offset = m_intermediate_data.find(data);
		if( offset == m_intermediate_data.count() ){
			//data was not found -- add it to the end
			m_intermediate_data.push_back(data);
		}
		m_intermediate_data.at(offset).entry_list().push_back(value);
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
