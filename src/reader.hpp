
#pragma once

#include "object.hpp"
#include <iostream>
#include <optional>
#include <vector>

class Alma;
class Symbol;
class String;

class Reader {
private:
    class Input {
    private:
        std::string name;
        Alma& alma;
        std::istream& input;
        size_t pos;
        size_t line;
        bool changed_line;

        size_t quasiquote_level;

    private:
        ObjectWeakRef<Symbol> findSymbol(const std::vector<std::string>& splittedTokens);

    public:
        template <typename... AS>
        Input(Alma& alma, const std::string& name, AS&&... args);

        bool eof() const;

        int read_char();
        void unread_char();
        bool read_whitespace();
        bool read_comment();
        void read_blank();
        std::optional<ObjectWeakRef<String>> read_string();
        std::optional<ObjectWeakRef<Object>> read_list();
        std::optional<ObjectWeakRef<Object>> read_quote();
        std::optional<ObjectWeakRef<Object>> read_quasiquote();
        std::optional<ObjectWeakRef<Object>> read_unquote();
        std::optional<ObjectWeakRef<Object>> read_token();
        std::optional<ObjectWeakRef<Object>> read_next_object();
    } input;

public:
    template <typename... AS>
    Reader(Alma& alma, const std::string& name, AS&&... args);

    std::optional<ObjectWeakRef<Object>> read(bool eof = true);
};

template <typename... AS>
Reader::Input::Input(Alma& _alma, const std::string& _name, AS&&... args)
    : name(_name)
    , alma(_alma)
    , input(std::forward<AS>(args)...)
    , pos(0)
    , line(0)
    , changed_line(false)

    , quasiquote_level(0)
{
}

template <typename... AS>
Reader::Reader(Alma& _alma, const std::string& name, AS&&... args)
    : input(_alma, name, std::forward<AS>(args)...)
{
}
