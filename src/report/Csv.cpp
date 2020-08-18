#include "report/Csv.hpp"

using namespace report;

Csv::Csv(const Options & options){
	if( options.name().is_empty() ){
		set_name( get_unique_name() );
	} else {
		set_name(options.name());
	}
	set_type( get_class_type() );
	set_prefix( get_prefix() );

	write(options.header());
}
