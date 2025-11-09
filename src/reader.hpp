
#pragma once

#include "objects.hpp"

namespace reader {
std::shared_ptr<Object> read(std::istream& input);
std::shared_ptr<Object> read_whitespace(std::istream& input);
std::shared_ptr<Object> read_comment(std::istream& input);
std::shared_ptr<Object> read_list(std::istream& input);
std::shared_ptr<Object> read_quote(std::istream& input);
std::shared_ptr<Object> read_quasiquote(std::istream& input);
std::shared_ptr<Object> read_unquote(std::istream& input);
std::shared_ptr<Object> read_string(std::istream& input);
std::shared_ptr<Object> read_token(std::istream& input);
}
