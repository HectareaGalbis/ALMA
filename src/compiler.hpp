
#pragma once

#include <filesystem>
#include <string>
#include <xbyak/xbyak.h>

class Compiler : public Xbyak::CodeGenerator {
private:
    std::string data;
    std::string code;

private:
    std::string get_byte_code() const;

public:
    void compile(const std::filesystem::path& path);
};
