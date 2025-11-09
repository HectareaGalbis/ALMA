

#include "emitter.hpp"
#include "function.hpp"
#include "macro.hpp"
#include "parser.hpp"
#include "special_operator.hpp"
#include "symbol.hpp"
#include <filesystem>
#include <fstream>

void showUsage()
{
    std::string message;
    message = R"END(
 ALMA  Copyright (C) 2025  Héctor Galbis Sanchis

 This is the ALMA compiler. This is for now a proof of concept.

 Usage:

   alma [input [output]]
 )END";

    std::cout << message << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc > 3) {
        std::cerr << "Too many arguments" << std::endl;
        showUsage();
        exit(1);
    }

    std::filesystem::path file(argv[1]);
    if (argc == 3) {
        std::filesystem::path output(argv[2]);
        Emitter::emitter = std::make_unique<std::ofstream>(output);
    }

    Package::initAlmaPackage();
    intern_special_operators();
    intern_functions();
    intern_macros();
    intern_symbols();

    try {
        Environment lex_env;
        ast ast;
        ast.read(file);
        // ast.print();
        ast.eval(lex_env);
    } catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
