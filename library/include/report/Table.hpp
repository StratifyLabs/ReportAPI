// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef REPORTAPI_REPORT_TABLE_HPP
#define REPORTAPI_REPORT_TABLE_HPP

#include "Section.hpp"

namespace report {

class Table : public Section {
public:
  Table() {} // used as a placeholder

  Table(
    printer::Printer &printer,
    const var::StringView name,
    const var::StringViewList header_list);

  static var::StringView get_class_type() { return "tab"; }

private:
  API_AC(Table, var::StringViewList, row);
};

} // namespace report

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const report::Table &a);
} // namespace printer

#endif // REPORTAPI_REPORT_TABLE_HPP
