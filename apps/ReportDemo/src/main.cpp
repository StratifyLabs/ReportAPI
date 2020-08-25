
#include <cstdio>
#include <sapi/sys/Cli.hpp>

#include "Application.hpp"

int main(int argc, char * argv[]){
	sys::Cli cli(argc, argv);
#if defined __link
	DataFileWriter writer;
#else
	PosixWriter writer;
	writer.set_fileno(stdout->_file);
#endif

	Writer::set_writer(&writer);


	//Writer callback needs to be setup for constructing any writer classes


	printf("Starting Application\n"); //any printf statement is OK
	int result = Application().execute(cli);

#if defined __link
	printf("%s", var::String(writer.data_file().data()).cstring());
#endif


	return result;
}

