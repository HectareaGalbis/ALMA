
#include "reader.hpp"
#include "package.hpp"
#include <iostream>
#include <regex>

#define maybe(EXPR)                               \
    if (std::shared_ptr<Object> __obj__ = EXPR) { \
        return __obj__;                           \
    }

std::shared_ptr<Object> reader::read(std::istream& input)
{
    maybe(reader::read_whitespace(input)); // Consume whitespaces
    maybe(reader::read_comment(input));
    maybe(reader::read_list(input));
    maybe(reader::read_quote(input));
    maybe(reader::read_quasiquote(input));
    maybe(reader::read_unquote(input));
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
    return reader::read_whitespace(input);
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
        throw std::runtime_error("Expected the character ')' but found '" + std::string(1, (char)rp) + "'");

    if (objects.empty())
        return std::make_shared<Nil>();
    else
        return std::make_shared<Cons>(objects);
}

std::shared_ptr<Object> reader::read_quote(std::istream& input)
{
    int q = input.get();
    if (q != '\'') {
        input.unget();
        return nullptr;
    }
    std::shared_ptr<Object> object = reader::read(input);
    std::shared_ptr<Symbol> qs = *Package::almaPackage->find_symbol("quote");

    return std::make_shared<Cons>(std::vector<std::shared_ptr<Object>> { qs, object });
}

static size_t quasiquote_level = 0;

std::shared_ptr<Object> reader::read_quasiquote(std::istream& input)
{
    int q = input.get();
    if (q != '`') {
        input.unget();
        return nullptr;
    }
    quasiquote_level++;
    std::shared_ptr<Object> object = reader::read(input);
    quasiquote_level--;
    std::shared_ptr<Symbol> qs = *Package::almaPackage->find_symbol("quasiquote");

    return std::make_shared<Cons>(std::vector<std::shared_ptr<Object>> { qs, object });
}

std::shared_ptr<Object> reader::read_unquote(std::istream& input)
{
    int q = input.get();
    if (q != ',') {
        input.unget();
        return nullptr;
    }
    bool slice = true;
    int s = input.get();
    if (s != '@') {
        slice = false;
        input.unget();
    }
    if (quasiquote_level == 0)
        throw std::runtime_error(std::string(slice ? "slice-unquote" : "unquote") + " outside quasiquote");

    quasiquote_level--;
    std::shared_ptr<Object> object = reader::read(input);
    quasiquote_level++;
    std::shared_ptr<Symbol> qs = *Package::almaPackage->find_symbol(slice ? "slice-unquote" : "unquote");

    return std::make_shared<Cons>(std::vector<std::shared_ptr<Object>> { qs, object });
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
    return c == '!' || (c >= '#' && c <= '&') || (c >= '*' && c <= '+') || (c >= '-' && c <= ':')
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

static std::pair<size_t, size_t> find_next_delimiter(const std::string& s, const std::vector<std::string>& delimiters, size_t start)
{
    for (const std::string& delimiter : delimiters) {
        size_t end = s.find(delimiter, start);
        if (end != std::string::npos) {
            return { end, delimiter.size() };
        }
    }
    return { std::string::npos, 0 };
}

static std::vector<std::string> splitString(const std::string& s, const std::vector<std::string>& delimiters)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    while (true) {
        auto [end, delSize] = find_next_delimiter(s, delimiters, start);
        tokens.push_back(s.substr(start, end - start));
        start = end + delSize;
        if (end == std::string::npos)
            break;
    }

    return tokens;
}

static std::vector<std::string> parse_token(const std::string& token)
{
    return splitString(token, { "::", ":" }); // Order matters. Most specific first
}

static std::shared_ptr<Symbol> findSymbol(const std::vector<std::string>& splittedTokens)
{
    std::shared_ptr<Package> packageIt = Package::currentPackage;
    for (size_t i = 0; i < splittedTokens.size() - 1; i++) {
        std::shared_ptr<Symbol> packageSymbol = packageIt->intern_symbol(splittedTokens[i]);
        if (!packageSymbol->package) {
            std::string currentSymbol;
            for (size_t j = 0; j < i; j++)
                currentSymbol += splittedTokens[j] + "::";
            currentSymbol += splittedTokens[i];
            throw std::runtime_error("The symbol " + currentSymbol + " does not denote a package.");
        }
        packageIt = packageSymbol->package;
    }
    return packageIt->intern_symbol(splittedTokens.back());
}

std::shared_ptr<Object> reader::read_token(std::istream& input)
{
    int c = input.peek();
    if (!is_token_character(c))
        return nullptr;
    std::string token;
    while (input) {
        int d = input.get();
        if (!is_token_character(d)) {
            input.unget();
            break;
        }
        token.push_back(d);
    }

    std::shared_ptr<Integer> number = parse_number(token);
    if (number)
        return number;

    std::vector<std::string> splittedTokens = parse_token(token);
    return findSymbol(splittedTokens);
}
