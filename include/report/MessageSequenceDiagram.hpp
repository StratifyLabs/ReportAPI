#ifndef REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP
#define REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP

#include "Writer.hpp"

namespace report {

class MessageSequenceDiagram: public Writer {
public:
	MessageSequenceDiagram();

	static var::String get_class_type(){
		return "msd";
	}

	class MessageOptions {
		API_AC(MessageOptions,var::String,source);
		API_AC(MessageOptions,var::String,destination);
		API_AC(MessageOptions,var::String,message);
	};

	MessageSequenceDiagram& write_message(const MessageOptions & options){
		write(options.source() + "->" + options.destination() + ":" + options.message());
		return *this;
	}

	MessageSequenceDiagram& write_message_alt(const MessageOptions & options){
		write(options.source() + "->>" + options.destination() + ":" + options.message());
		return *this;
	}

	MessageSequenceDiagram& write_message_dotted(const MessageOptions & options){
		write(options.source() + "-->" + options.destination() + ": " + options.message());
		return *this;
	}

};


}

#endif // REPORTAPI_REPORT_MESSAGESEQUENCEDIAGRAM_HPP
