#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <sapi/chrono.hpp>
#include <sapi/sys/Cli.hpp>
#include "../../../../ReportAPI/include/report.hpp"

class Application
{
public:
	Application();

	int execute(const sys::Cli & cli);

private:

	report::Csv m_csv;
	report::CallGraph m_callgraph;

	Timer m_timer;

	void update_csv_table();





};


#endif // APPLICATION_HPP
