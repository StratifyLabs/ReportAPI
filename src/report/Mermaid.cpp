#include "report/Mermaid.hpp"

using namespace report;

u32 MermaidNode::m_count = 0;

Mermaid::Mermaid(const Options & options){
	set_name(options.name());
	set_type(get_class_type());
	set_prefix( get_prefix() );

	write(options.diagram());
}

MermaidGraph::MermaidGraph(const Options & options) :
	Mermaid(Mermaid::Options().set_name(options.name()).set_diagram("graph " + options.direction())){
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

var::String MermaidGraph::get_link_string(enum links link){
	switch(link){
		case link_arrow: return "-->";
		case link_open: return " --- ";
		case link_dotted: return "-.->";
		case link_thick: return "==>";
	}
	return "-->";
}

MermaidGraph& MermaidGraph::transition(
		const MermaidNode & from,
		enum links link_arrow,
		const MermaidNode & to,
		const var::String & message){
	write("  " + from.node() + get_link_string(link_arrow) +
				(message.is_empty() ? var::String() : ("|" + message + "|")
															+ to.node()));
	return *this;
}
