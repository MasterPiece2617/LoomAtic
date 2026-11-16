#pragma once

#include <lexer.hpp>

#include <cstddef>
#include <string>

namespace LoomAtic
{
    struct Token
    {
        std::string name;
        std::string lexeme;
        std::size_t offset;
        std::size_t line;
        std::size_t column;
    };

    class Lexer
    {
    public:
        Lexer();

        Designar::DynArray<Token> tokenize(const std::string& source) const;

    private:
        Designar::Lexer designar_lexer;
    };
}
