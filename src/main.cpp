
#include "function.hpp"
#include "parser.hpp"
#include "special_operator.hpp"
#include <elfio/elfio.hpp>

void showUsage()
{
    std::string message;
    message = R"END(
ALMA  Copyright (C) 2025  Héctor Galbis Sanchis

This is the ALMA compiler. This is for now a proof of concept.

Usage:

  alma input output
)END";

    std::cout << message << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Need two arguments" << std::endl;
        showUsage();
        exit(1);
    }

    intern_special_operators();
    intern_functions();

    std::filesystem::path file(argv[1]);
    std::filesystem::path output(argv[2]);

    try {
        Compiler comp;
        lexical_environment lex_env;
        ast ast;
        ast.read(file);
        // ast.print();
        ast.eval(comp, lex_env);

        comp.compile(output);
        // std::cout << std::endl;
        // for (unsigned char byte : comp.compile())
        //     std::cout << std::hex << std::setw(2) << std::setfill('0') << "0x" << (int)byte << " ";
        // std::cout << std::endl;
    } catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
