#ifndef REPORTAPI_REPORT_PARSER_HPP
#define REPORTAPI_REPORT_PARSER_HPP

#include <sapi/var/Vector.hpp>
#include <sapi/var/String.hpp>
#include <sapi/fs/File.hpp>

namespace report {

class Parser {
public:

	class Options {
		API_AC(Options,var::String,path); //path to a directory or file
	};

	Parser(const Options & options);

	Parser& parse(const var::String & input);

	int create_report();


private:

	class IntermediateData {
	public:

		bool operator == (const IntermediateData & a) const {
			return type() == a.type() && name() == a.name();
		}

		var::String get_file_path(const var::String & directory_path){
			return directory_path + "/" + type() + "_" + name() + ".txt";
		}

	private:
		API_AC(IntermediateData,var::String,type);
		API_AC(IntermediateData,var::String,name);
		API_AC(IntermediateData,var::StringList,entry_list);
	};

	API_RAB(Parser,valid,false); //used if path is a empty
	API_AC(Parser,var::String,output); //used if path is a empty
	var::Vector<IntermediateData> m_intermediate_data_list; //used if path is a file or empty
	var::String m_directory_path;
	var::String m_file_name;

	bool is_use_intermediate_data() const {
		return m_directory_path.is_empty();
	}

	int generate_csv_chart(const fs::File * output, const IntermediateData & data);
	int generate_passthrough(const fs::File * output, const IntermediateData & data);

};

}

#endif // REPORTAPI_REPORT_PARSER_HPP
