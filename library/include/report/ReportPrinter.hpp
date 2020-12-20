// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef REPORTAPI_REPORT_REPORTPRINTER_HPP
#define REPORTAPI_REPORT_REPORTPRINTER_HPP

#include <chrono/ClockTime.hpp>
#include <printer/Printer.hpp>

namespace report {

class ReportPrinter : public printer::Printer {
public:
  ReportPrinter();

protected:
  void print_open_object(Level verbose_level, var::StringView key) override;
  void print_close_object() override;

  void print_open_array(Level verbose_level, var::StringView key) override;
  void print_close_array() override;

  void print(
    Level level,
    var::StringView key,
    var::StringView value,
    IsNewline is_newline = IsNewline::yes) override;

private:
  enum class ContainerType { array, object };
  using Container = ContainerAccess<ContainerType>;

  bool m_is_newline = true;
  Container m_container;
  chrono::ClockTime m_start;
  chrono::ClockTime::UniqueString get_timestamp() const;
};

} // namespace report

#endif // REPORTAPI_REPORT_REPORTPRINTER_HPP
