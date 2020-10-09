#include "report/Writer.hpp"

using namespace report;

void *Writer::m_context = nullptr;
Writer::write_function_t Writer::m_write_function = nullptr;

Writer::Writer() {
  set_type(var::String("raw"));
  set_terminator(var::String("\n"));
}
