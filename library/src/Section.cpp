#include "report/Section.hpp"

using namespace report;

Section::Section() { set_prefix(generate_prefix("raw", "")); }
