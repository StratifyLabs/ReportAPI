// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include "report/Section.hpp"

using namespace report;

Section::Section() { set_prefix(generate_prefix("raw", "")); }
