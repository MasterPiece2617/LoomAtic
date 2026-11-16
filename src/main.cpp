#include <loomatic/loomatic_lexer.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: main <archivo.lm>\n";
        return 2;
    }

    const std::string path = argv[1];
    if (path.size() < 3 || path.substr(path.size() - 3) != ".lm")
    {
        std::cerr << "El archivo de entrada debe tener extension .lm\n";
        return 2;
    }

    std::ifstream input(path, std::ios::binary);
    if (!input)
    {
        std::cerr << "No se pudo abrir el archivo: " << path << '\n';
        return 1;
    }

    const std::string source{std::istreambuf_iterator<char>(input),
                             std::istreambuf_iterator<char>()};

    try
    {
        const LoomAtic::Lexer lexer;
        const Designar::DynArray<LoomAtic::Token> tokens = lexer.tokenize(source);

        for (Designar::nat_t i = 0; i < tokens.size(); ++i)
        {
            const LoomAtic::Token& token = tokens[i];
            if (token.name != "WHITESPACE")
            {
                std::cout << token.line << ':' << token.column << " (offset "
                          << token.offset << ") " << token.name << " \""
                          << token.lexeme << "\"\n";
            }
        }
    }
    catch (const std::domain_error& error)
    {
        std::cerr << "Error en una expresion regular del lexer: "
                  << error.what() << '\n';
        return 1;
    }
    catch (const std::runtime_error& error)
    {
        std::cerr << "Error lexico: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
