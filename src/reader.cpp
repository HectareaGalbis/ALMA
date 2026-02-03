
#include "reader.hpp"
#include "alma.hpp"
#include "cons.hpp"
#include "debug.hpp"
#include "integer.hpp"
#include "package.hpp"
#include "string.hpp"
#include <iostream>
#include <regex>

#define ithrow(MSG)                                                                         \
    {                                                                                       \
        std::stringstream gensym(ss);                                                       \
        gensym(ss) << MSG;                                                                  \
        gensym(ss) << this->name << "(" << this->line << ":" << this->pos << ") | " << MSG; \
        throw std::runtime_error(gensym(ss).str());                                         \
    }

#define iassert(COND, MSG) \
    if (!(COND))           \
    ithrow(MSG)

bool Reader::Input::eof() const
{
    return this->input.eof();
}

int Reader::Input::read_char()
{
    int c = this->input.get();
    iassert(c != EOF, "Unexpected EOF");
    this->pos++;
    this->changed_line = (c == '\n');
    if (this->changed_line)
        this->line++;
    return c;
}

void Reader::Input::unread_char()
{
    this->input.unget();
    iassert(!this->input.fail(), "Error unreading the last character");
    if (this->changed_line) {
        this->line--;
        this->changed_line = false;
        this->pos--;
    }
}

bool Reader::Input::read_whitespace()
{
    bool found = false;
    while (true) {
        int c = this->read_char();
        if (!(c == ' ' || c == '\n' || c == '\t')) {
            this->unread_char();
            break;
        }
        found = true;
    }

    return found;
}

bool Reader::Input::read_comment()
{
    int c = this->read_char();
    if (c != ';') {
        this->unread_char();
        return false;
    }
    while (this->read_char() != '\n') { }
    return true;
}

void Reader::Input::read_blank()
{
    while (this->read_whitespace() || this->read_comment()) { }
}

std::optional<ObjectWeakRef<String>> Reader::Input::read_string()
{
    int q = this->read_char();
    if (q != '"') {
        this->unread_char();
        return std::nullopt;
    }
    std::string content;
    bool next_special = false;
    while (input) {
        int d = this->read_char();
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
    return this->alma.make<String>(content);
}

std::optional<ObjectWeakRef<Object>> Reader::Input::read_list()
{
    // Read left paren
    {
        int lp = this->read_char();
        if (lp != '(') {
            this->unread_char();
            return std::nullopt;
        }
    }

    // Read list objects
    std::vector<ObjectWeakRef<Object>> objects;
    while (input) {
        std::optional<ObjectWeakRef<Object>> object = this->read_next_object();
        if (object)
            objects.push_back(*object);
        else
            break;
    }

    // Read dot
    bool proper_list = true;
    {
        int dot = this->read_char();
        if (dot == '.')
            proper_list = false;
        else
            this->unread_char();
    }

    // Read non proper object
    std::optional<ObjectWeakRef<Object>> non_proper_object;
    if (!proper_list) {
        non_proper_object = this->read_next_object();
        iassert(non_proper_object, "Expected an object after the dot character");
        this->read_blank();
    }

    // Read right paren
    {
        int rp = this->read_char();
        iassert(rp == ')', "Expected the character ')' but found '" << static_cast<char>(rp) << "'");
    }

    // Make result
    if (objects.empty())
        return this->alma.find_alma_symbol("nil");
    else if (non_proper_object) {
        return this->alma.make<Cons>(objects, *non_proper_object);
    } else {
        return this->alma.make<Cons>(objects);
    }
}

std::optional<ObjectWeakRef<Object>> Reader::Input::read_quote()
{
    int q = this->read_char();
    if (q != '\'') {
        this->unread_char();
        return std::nullopt;
    }
    std::optional<ObjectWeakRef<Object>> object = this->read_next_object();
    iassert(object, "Expected an object after the quote");
    ObjectWeakRef<Object> qs = this->alma.find_alma_symbol("quote");

    return this->alma.make<Cons>(std::vector<ObjectWeakRef<Object>> { qs, *object });
}

std::optional<ObjectWeakRef<Object>> Reader::Input::read_quasiquote()
{
    int q = this->read_char();
    if (q != '`') {
        this->unread_char();
        return std::nullopt;
    }
    this->quasiquote_level++;
    std::optional<ObjectWeakRef<Object>> object = this->read_next_object();
    iassert(object, "Expected an object after the backquote");
    this->quasiquote_level--;
    ObjectWeakRef<Object> qs = this->alma.find_alma_symbol("quasiquote");

    return this->alma.make<Cons>(std::vector<ObjectWeakRef<Object>> { qs, *object });
}

std::optional<ObjectWeakRef<Object>> Reader::Input::read_unquote()
{
    int q = this->read_char();
    if (q != ',') {
        this->unread_char();
        return std::nullopt;
    }
    bool slice = true;
    int s = this->read_char();
    if (s != '@') {
        slice = false;
        this->unread_char();
    }
    iassert(quasiquote_level != 0, (slice ? "slice-unquote" : "unquote") << " outside quasiquote");

    quasiquote_level--;
    std::optional<ObjectWeakRef<Object>> object = this->read_next_object();
    iassert(object, "Expected an object after the " << (slice ? "slice-unquote" : "unquote"));
    quasiquote_level++;
    ObjectWeakRef<Object> qs = this->alma.find_alma_symbol(slice ? "slice-unquote" : "unquote");

    return this->alma.make<Cons>(std::vector<ObjectWeakRef<Object>> { qs, *object });
}

static bool is_token_character(int c)
{
    return c == '!' || (c >= '#' && c <= '&') || (c >= '*' && c <= '+') || (c >= '-' && c <= ':')
        || (c >= '<' && c <= 'Z') || c == '_' || (c >= 'a' && c <= 'z');
}

static std::optional<ObjectWeakRef<Integer>> parse_number(const std::string& token, Alma& alma)
{
    std::regex int_regex(R"(^[+-]?\d+$)");
    if (std::regex_match(token, int_regex)) {
        int64_t value = std::stoll(token);
        return alma.make<Integer>(value);
    }
    return std::nullopt;
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
    return splitString(token, { ":" }); // Order matters. Most specific first
}

// TODO: Hacer 1-lisp
// TODO: Crear arrays [m 0 1]
// TODO: Hacer diccionarios {a 3 b 4 c 5}

ObjectWeakRef<Symbol> Reader::Input::findSymbol(const std::vector<std::string>& splittedTokens)
{
    ObjectWeakRef<Package> packageIt = alma.get_current_package();
    for (size_t i = 0; i < splittedTokens.size() - 1; i++) {
        ObjectWeakRef<Symbol> packageSymbol = packageIt->intern_symbol(splittedTokens[i]);
        std::optional<ObjectWeakRef<Package>> next_package = packageSymbol->get_package();
        if (!next_package) {
            std::string currentSymbol;
            for (size_t j = 0; j < i; j++)
                currentSymbol += splittedTokens[j] + ":";
            currentSymbol += splittedTokens[i];
            ithrow("The symbol " << currentSymbol << " does not denote a package.");
        }
        packageIt = *next_package;
    }
    return packageIt->intern_symbol(splittedTokens.back());
}

std::optional<ObjectWeakRef<Object>> Reader::Input::read_token()
{
    int c = this->input.peek();
    if (!is_token_character(c))
        return std::nullopt;
    std::string token;
    while (input) {
        int d = this->read_char();
        if (!is_token_character(d)) {
            this->unread_char();
            break;
        }
        token.push_back(d);
    }

    std::optional<ObjectWeakRef<Integer>> number = parse_number(token, this->alma);
    if (number)
        return number;

    std::vector<std::string> splittedTokens = parse_token(token);
    return findSymbol(splittedTokens);
}

#define maybe(EXPR)            \
    if (auto __obj__ = EXPR) { \
        return __obj__;        \
    }

std::optional<ObjectWeakRef<Object>> Reader::Input::read_next_object()
{
    this->read_blank();
    maybe(this->read_string());
    maybe(this->read_list());
    maybe(this->read_quote());
    maybe(this->read_quasiquote());
    maybe(this->read_unquote());
    maybe(this->read_token());
    ithrow("Unexpected token " << this->input.peek());
}

std::optional<ObjectWeakRef<Object>> Reader::read(bool eof)
{
    try {
        return this->input.read_next_object();
    } catch (const std::runtime_error& e) {
        if (this->input.eof() && eof)
            return std::nullopt;
        throw e;
    }
}
