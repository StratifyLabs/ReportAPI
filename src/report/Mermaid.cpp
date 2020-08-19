#include "report/Mermaid.hpp"

using namespace report;

u32 MermaidNode::m_count = 0;

Mermaid::Mermaid(const Options & options){

}


MermaidNode::MermaidNode(const var::String & text, enum shapes shape){
	switch(shape){
		case shape_square:
			m_node = var::String().format("n%d[%s]", m_count++,text.cstring());
			return;
		case shape_round:
			m_node = var::String().format("n%d(%s)", m_count++,text.cstring());
			return;
		case shape_stadium:
			m_node = var::String().format("n%d([%s])", m_count++,text.cstring());
			return;
		case shape_subroutine:
			m_node = var::String().format("n%d[[%s]]", m_count++,text.cstring());
			return;
		case shape_cylinder:
			m_node = var::String().format("n%d[(%s)]", m_count++,text.cstring());
			return;
		case shape_circle:
			m_node = var::String().format("n%d((%s))", m_count++,text.cstring());
			return;
		case shape_asymmetric:
			m_node = var::String().format("n%d>%s]", m_count++,text.cstring());
			return;
		case shape_rhombus:
			m_node = var::String().format("n%d{%s}", m_count++,text.cstring());
			return;
		case shape_hexagon:
			m_node = var::String().format("n%d{{%s}}", m_count++,text.cstring());
			return;
		case shape_parallelogram:
			m_node = var::String().format("n%d[/%s/]", m_count++,text.cstring());
			return;
		case shape_parallelogram_alternate:
			m_node = var::String().format("n%d[\\%s\\]", m_count++,text.cstring());
			return;
		case shape_trapezoid:
			m_node = var::String().format("n%d[/%s\\]", m_count++,text.cstring());
			return;
		case shape_trapezoid_alternate:
			m_node = var::String().format("n%d[\\%s/]", m_count++,text.cstring());
			return;
	}
}
