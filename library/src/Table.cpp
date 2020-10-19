#include "report/Table.hpp"

using namespace report;

Table::Table(const Options &options) {
  if (options.name().is_empty()) {
    set_name(var::String(get_unique_name().cstring()));
  } else {
    set_name(options.name());
  }
  set_type(var::String(get_class_type()));
  set_prefix(var::String(get_prefix()));

  write(options.header());
}
