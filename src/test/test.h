#pragma once

#include <ostream>
#include <sstream>
#include "catch.hpp"
#include "runtime/Key.h"
#include "runtime/Stage.h"

KeySequence parse_input(const char* input);
KeySequence parse_output(const char* output);
KeySequence parse_sequence(const char* it, const char* const end);

template<size_t N>
KeySequence parse_sequence(const char(&input)[N]) {
  return parse_sequence(input, input + N - 1);
}
std::string format_sequence(const KeySequence& sequence);
std::string format_list(const std::vector<Key>& keys);

Stage create_stage(const char* string);

KeyEvent make_timeout_ms(int timeout_ms);
KeyEvent make_not_timeout_ms(int timeout_ms);
KeyEvent make_output_timeout_ms(int timeout_ms);
