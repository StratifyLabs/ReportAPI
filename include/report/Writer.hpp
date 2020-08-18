#ifndef REPORTAPI_REPORT_WRITER_HPP
#define REPORTAPI_REPORT_WRITER_HPP

#include <cstdio>
#include <sapi/var/String.hpp>
#include <sapi/chrono/Clock.hpp>

namespace report {

class PosixWriter {
public:
	static int write_function(void * context, const char * buffer, int nbyte){
		return reinterpret_cast<const PosixWriter*>(context)->write_to_file(buffer, nbyte);
}
private:
	API_AF(PosixWriter,int,fileno,-1);
	int write_to_file(const char * buffer, int nbyte) const {
		return ::write(fileno(), buffer, nbyte);
	}
};

class FileWriter {
public:
	static int write_function(void * context, const char * buffer, int nbyte){
		return reinterpret_cast<const FileWriter*>(context)->write_to_file(buffer, nbyte);
}
private:
	API_AF(FileWriter,FILE*,file,nullptr);
	int write_to_file(const char * buffer, int nbyte) const {
		return ::fwrite(buffer, nbyte, 1, file());
	}
};

class Writer {
public:

	typedef int (*write_function_t)(void * context, const char * buffer, int nbyte);
	Writer();

	//write to output

	Writer& set_writer(FileWriter * value){
		set_context(static_cast<void*>(value));
		set_write_function(FileWriter::write_function);
		return *this;
	}

	Writer& set_writer(PosixWriter * value){
		set_context(static_cast<void*>(value));
		set_write_function(PosixWriter::write_function);
		return *this;
	}

	var::String get_prefix() const {
		return type() + ":" + name() + ":";
	}

	var::String get_unique_name() const {
		return chrono::Clock::get_time().get_unique_string();
	}

protected:

	int write(const var::String & str) const {
		return write(str.cstring(), str.length());
	}

	int write(const char * buffer, int nbyte) const {
		if( m_write_function ){
			int result = 0;
			result += m_write_function(context(), prefix().cstring(), prefix().length());
			result +=  m_write_function(context(), buffer, nbyte);
			result += m_write_function(context(), terminator().cstring(), terminator().length());
			return result;
		}
		return -1;
	}

private:
	API_AC(Writer,var::String,name);
	API_AC(Writer,var::String,type);
	API_AC(Writer,var::String,prefix);
	API_AC(Writer,var::String,terminator);
	API_AF(Writer,void *,context,nullptr);
	API_AF(Writer,write_function_t,write_function,nullptr);

};

template <class T> class WriterAccess : public Writer {
public:
	API_ADC(Writer,T,var::String,name)
	API_ADC(Writer,T,var::String,type)
	API_ADC(Writer,T,var::String,prefix)

};

}



#endif // REPORTAPI_REPORT_WRITER_HPP
