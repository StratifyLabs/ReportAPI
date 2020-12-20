// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <chrono.hpp>

#include "report/ReportPrinter.hpp"

using namespace report;

ReportPrinter::ReportPrinter()
  : m_container(Level::info, ContainerType::array) {
  m_start = ClockTime::get_system_time();
}

ClockTime::UniqueString ReportPrinter::get_timestamp() const {
  ClockTime age = m_start.get_age();
  return ClockTime::UniqueString().format(
    "%ld.%06ld",
    age.seconds(),
    age.nanoseconds() / 1000);
}

void ReportPrinter::print_open_object(Level level, var::StringView key) {
  m_container = Container(level, ContainerType::object);
  if (m_container.verbose_level() > verbose_level()) {
    return;
  }
  interface_print_final(key);
  interface_print_final(":");
  interface_print_final(get_timestamp().string_view());
  interface_print_final(":");
  m_is_newline = true;
}

void ReportPrinter::print_close_object() {
  if (m_container.verbose_level() > verbose_level()) {
    return;
  }
  interface_print_final("\n");
}

void ReportPrinter::print_open_array(Level level, var::StringView key) {
  m_container = Container(level, ContainerType::array);
  if (m_container.verbose_level() > verbose_level()) {
    return;
  }
  interface_print_final(key);
  interface_print_final(":");
  interface_print_final(get_timestamp().string_view());
  interface_print_final(":");
  m_is_newline = true;
}

void ReportPrinter::print_close_array() {
  if (m_container.verbose_level() > verbose_level()) {
    return;
  }
  interface_print_final("\n");
}

void ReportPrinter::print(
  Level level,
  var::StringView key,
  var::StringView value,
  IsNewline is_newline) {

  if (level > verbose_level()) {
    return;
  }

  if (m_is_newline == false) {
    interface_print_final(",");
  } else {
    m_is_newline = false;
  }

  if (m_container.type() == ContainerType::object && key.is_null() == false) {
    interface_print_final(key);
    interface_print_final(":");
  }

  if (value.is_null() == false) {
    interface_print_final(value);
  }
}
