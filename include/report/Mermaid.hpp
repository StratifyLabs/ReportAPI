#ifndef REPORTAPI_REPORT_MERMAID_HPP
#define REPORTAPI_REPORT_MERMAID_HPP

#include <sapi/var/Stack.hpp>

#include "Writer.hpp"

namespace report {

class Mermaid : public Writer {
public:

	class Options {
		API_AC(Options,var::String,name);
		API_AC(Options,var::String,diagram);
		API_AF(Options,Writer*,writer,nullptr);
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
		API_AC(Options,var::String,name);
	};

	MermaidGraph(const Options & options);

	class TransitionOptions {
		API_AC(TransitionOptions,MermaidNode,first);
		API_AC(TransitionOptions,MermaidNode,second);
		API_AC(TransitionOptions,var::String,message);
		API_AF(TransitionOptions,enum links,link,link_arrow);
	};

	MermaidGraph& transition(
			const MermaidNode & from,
			enum links link_arrow,
			const MermaidNode & to,
			const var::String & message);

private:
	u32 m_sequence_count;

	var::String get_link_string(enum links link);

};


class CallGraph {
public:
	explicit CallGraph(const var::String& name) :
		m_mermaid_graph(MermaidGraph::Options().set_name(name).set_direction("LR")){}

	void push(const char * name){
		printf("-------------------pushing node: %d:%s\n", m_call_stack.count(), name);
		MermaidNode node(name, MermaidNode::shape_round);
		if( m_call_stack.count() ){
			m_mermaid_graph.transition(
						m_call_stack.top(),
						MermaidGraph::link_arrow,
						node,
						var::String::number(m_transition_counter++)
						);
		}
		m_call_stack.push(node);
	}

	void data(const var::String & data){
		if( m_call_stack.count() ){
			m_mermaid_graph.transition(
						m_call_stack.top(),
						report::MermaidGraph::link_arrow,
						MermaidNode(data, MermaidNode::shape_cylinder),
						var::String::number(m_transition_counter++)
						);
		}
	}

	void pop(){
		if( m_call_stack.count() ){
			printf("-------------------popping node %s\n", m_call_stack.top().node().cstring());
			m_call_stack.pop();
		}
	}

	MermaidGraph & mermaid_graph(){ return m_mermaid_graph; }


private:
	var::Stack<MermaidNode> m_call_stack;
	MermaidGraph m_mermaid_graph;
	u32 m_transition_counter = 0;
};

class CallGraphNode {
public:
	CallGraphNode(CallGraph & graph, const char * name) : m_graph(graph){
		graph.push(name);
	}

	~CallGraphNode(){
		m_graph.pop();
	}

private:
	CallGraph & m_graph;
};

}

#define CALL_GRAPH_TRACE_FUNCTION(graph) (report::CallGraphNode call_graph_node = report::CallGraphNode(graph,__FUNCTION__))


#endif // REPORTAPI_REPORT_MERMAID_HPP
