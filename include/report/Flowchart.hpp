#ifndef REPORTAPI_REPORT_FLOWCHART_HPP
#define REPORTAPI_REPORT_FLOWCHART_HPP

#include "Writer.hpp"

namespace report {


class Flowchart : public Writer
{
public:
	Flowchart();

	static var::String get_class_type(){
		return "fc";
	}

};

}

#endif // REPORTAPI_REPORT_FLOWCHART_HPP
