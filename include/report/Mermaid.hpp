#ifndef REPORTAPI_REPORT_MERMAID_HPP
#define REPORTAPI_REPORT_MERMAID_HPP

#include "Writer.hpp"

namespace report {

class Mermaid : public Writer
{
public:

	class Options {
		API_AC(Options,var::String,diagram);
	};


	Mermaid(const Options & options);

	static var::String get_class_type(){
		return "mmd";
	}
};

class MermaidNode {
public:
	enum shapes {
		shape_square,
		shape_round,
		shape_stadium,
		shape_subroutine,
		shape_cylinder,
		shape_circle,
		shape_asymmetric,
		shape_rhombus,
		shape_hexagon,
		shape_parallelogram,
		shape_parallelogram_alternate,
		shape_trapezoid,
		shape_trapezoid_alternate
	};

	MermaidNode(const var::String & text, enum shapes shape);

private:
	API_RAC(MermaidNode,var::String,node);
	static u32 m_count;
};

class MermaidGraph : public Mermaid
{
public:

	enum links {
		link_arrow,
		link_open,
		link_dotted,
		link_thick
	};


	class Options {
		API_AC(Options,var::String,direction);
	};

	MermaidGraph(const Options & options);

	class TransitionOptions {
		API_AC(TransitionOptions,MermaidNode,first);
		API_AC(TransitionOptions,MermaidNode,second);
		API_AC(TransitionOptions,var::String,message);
		API_AF(TransitionOptions,enum links,link,link_arrow);
	};

	MermaidGraph& write_transition(const TransitionOptions & options);

private:
	u32 m_sequence_count;

};

}

#endif // REPORTAPI_REPORT_MERMAID_HPP
