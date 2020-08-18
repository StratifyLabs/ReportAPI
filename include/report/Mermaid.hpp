#ifndef REPORTAPI_REPORT_MERMAID_HPP
#define REPORTAPI_REPORT_MERMAID_HPP

#include "Writer.hpp"

namespace report {

class Mermaid : public Writer
{
public:
	Mermaid();

	static var::String get_class_type(){
		return "mmd";
	}
};

}

#endif // REPORTAPI_REPORT_MERMAID_HPP
