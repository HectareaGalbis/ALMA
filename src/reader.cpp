
#include "reader.hpp"
#include "package.hpp"
#include <iostream>
#include <regex>

#define maybe(EXPR)                             \
    if (std::shared_ptr<Object> __obj__ = EXPR) \
        return __obj__;

std::shared_ptr<Object> reader::read(std::istream& input)
{
    maybe(reader::read_whitespace(input)); // Consume whitespaces
    maybe(reader::read_comment(input));
    maybe(reader::read_list(input));
    maybe(reader::read_quote(input));
    maybe(reader::read_string(input));
    maybe(reader::read_token(input));
    return nullptr;
}

std::shared_ptr<Object> reader::read_whitespace(std::istream& input)
{
    while (true) {
        int c = input.get();
        if (c == EOF)
            return nullptr;
        if (!(c == ' ' || c == '\n' || c == '\t')) {
            input.unget();
            return nullptr;
        }
    }
}

std::shared_ptr<Object> reader::read_comment(std::istream& input)
{
    int c = input.get();
    if (c != ';') {
        input.unget();
        return nullptr;
    }
    while (char d = input.get()) {
        if (d == EOF || d == '\n')
            break;
    }
    return nullptr;
}

std::shared_ptr<Object> reader::read_list(std::istream& input)
{
    int lp = input.get();
    if (lp != '(') {
        input.unget();
        return nullptr;
    }
    std::vector<std::shared_ptr<Object>> objects;
    while (input) {
        std::shared_ptr<Object> object = reader::read(input);
        if (!object)
            break;
        else
            objects.push_back(std::move(object));
    }
    int rp = input.get();
    if (rp != ')')
        throw std::runtime_error("Expected the character ')'");

    return std::make_shared<List>(objects);
}

std::shared_ptr<Object> reader::read_quote(std::istream& input)
{
    int q = input.get();
    if (q != '\'') {
        input.unget();
        return nullptr;
    }
    std::shared_ptr<Object> object = reader::read(input);
    std::shared_ptr<Symbol> qs = *package.find_symbol("quote");

    return std::make_shared<List>(std::vector<std::shared_ptr<Object>> { qs, object });
}

std::shared_ptr<Object> reader::read_string(std::istream& input)
{
    int q = input.get();
    if (q != '"') {
        input.unget();
        return nullptr;
    }
    std::string content;
    bool next_special = false;
    while (input) {
        int d = input.get();
        if (next_special) {
            switch (d) {
            case 'n':
                content.push_back('\n');
                break;
            case 'b':
                content.push_back(' ');
                break;
            case 't':
                content.push_back('\t');
                break;
            case '"':
                content.push_back('"');
                break;
            case '\\':
                content.push_back('\\');
                break;
            }
            next_special = false;
        } else {
            if (d == '\\')
                next_special = true;
            else if (d == '"')
                break;
            else
                content.push_back(d);
        }
    }
    return std::make_shared<String>(content);
}

static bool is_token_character(int c)
{
    return c == '!' || (c >= '#' && c <= '&') || (c >= '*' && c <= '+') || (c >= '-' && c <= '9')
        || (c >= '<' && c <= 'Z') || c == '_' || (c >= 'a' && c <= 'z');
}

static std::shared_ptr<Integer> parse_number(const std::string& token)
{
    std::regex int_regex(R"(^[+-]?\d+$)");
    if (std::regex_match(token, int_regex)) {
        int64_t value = std::stoll(token);
        return std::make_shared<Integer>(value);
    }
    return nullptr;
}

std::shared_ptr<Object> reader::read_token(std::istream& input)
{
    int c = input.peek();
    if (!is_token_character(c))
        return nullptr;
    std::string token;
    while (input) {
        int d = input.get();
        if (!is_token_character(c)) {
            input.unget();
            break;
        }
        token.push_back(d);
    }

    std::shared_ptr<Integer> number = parse_number(token);
    if (number)
        return number;

    return package.intern_symbol(token);
}
