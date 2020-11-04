#ifndef REPORTAPI_REPORT_SECTION_HPP
#define REPORTAPI_REPORT_SECTION_HPP

#include <chrono/ClockTime.hpp>
#include <cstdio>
#include <fs/File.hpp>
#include <printer/Printer.hpp>
#include <var/String.hpp>

namespace report {

class Section {
public:

  Section();

  chrono::ClockTime::UniqueString get_unique_name() const {
    return chrono::ClockTime::get_unique_string();
  }

  static var::StringView get_type(const var::StringView prefix) {
    auto list = prefix.split("/");
    API_ASSERT(list.count() == 2);
    return list.at(0);
  }

  static var::StringView get_name(const var::StringView prefix) {
    auto list = prefix.split("/");
    API_ASSERT(list.count() == 2);
    return list.at(1);
  }

  static var::KeyString
  generate_prefix(const var::StringView type, const var::StringView name) {
    return var::KeyString(type).append("/").append(name);
  }

protected:
private:
  API_AC(Section, var::KeyString, prefix);
};

template <class T> class SectionAccess : public Section {
public:
  API_ADC(Section, T, var::KeyString, prefix)
};

} // namespace report

#endif // REPORTAPI_REPORT_SECTION_HPP
