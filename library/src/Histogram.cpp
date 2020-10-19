#include "report/Histogram.hpp"

using namespace report;

Histogram::Histogram(const Options &options) {
  set_name(options.name());
  set_type(var::String(get_class_type()));
  set_prefix(get_prefix());
  write(options.labels());
}

Histogram &Histogram::write_row(const var::String &row) {
  write(row);
  return *this;
}
