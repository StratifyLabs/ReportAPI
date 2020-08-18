#ifndef REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP
#define REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP

#include "Writer.hpp"

namespace report {

class MessageSequenceDiagram
{
public:
	MessageSequenceDiagram();

	static var::String get_class_type(){
		return "msd";
	}
};

}

#endif // REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP
