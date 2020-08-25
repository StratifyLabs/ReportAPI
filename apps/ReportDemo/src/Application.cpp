
#include <mcu/types.h>
#include <sapi/var.hpp>
#include <cmath>
#include "Application.hpp"

Application::Application() :
	m_csv(Csv::Options().set_header("time,sin,cos").set_name("csv graph")),
				m_callgraph("callgraph")
{

}


int Application::execute(const sys::Cli & cli){
	CallGraphNode node(m_callgraph, __FUNCTION__);

	MicroTime duration = Seconds(6);
	if( cli.get_option("duration").is_empty() == false ){
		duration = Seconds(cli.get_option("duration").to_integer());
	}

	m_timer.start();
	u32 last_seconds = 0;

	while( m_timer < duration ){
		if( m_timer.seconds() != last_seconds ){
			last_seconds = m_timer.seconds();
			printf("Seconds is " F32U "\n", last_seconds);
		}
		update_csv_table();
		wait(Milliseconds(100));
	}


	return 0;
}


void Application::update_csv_table(){
	CallGraphNode node(m_callgraph, __FUNCTION__);
	m_csv.write_row(String().format("%0.6f,%0.4f,%0.4f",
									m_timer.microseconds() / 1000000.0f,
									sinf(2 * MCU_PI_FLOAT * 1.0f * m_timer.microseconds() / 1000000.0f),
									cosf(2 * MCU_PI_FLOAT * 1.0f * m_timer.microseconds() / 1000000.0f)
									));
}
