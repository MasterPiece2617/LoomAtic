#include <lexer.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

namespace
{
    void add_token(Designar::Lexer& lexer,
                   const std::string& name,
                   const std::string& pattern)
    {
        try
        {
            lexer.add_token(name, pattern);
        }
        catch (const std::domain_error& error)
        {
            throw std::domain_error("token '" + name + "': " + error.what());
        }
    }

    void add_language_rules(Designar::Lexer& lexer)
    {
        const std::string letters =
            "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|"
            "A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
        const std::string digits = "(0|1|2|3|4|5|6|7|8|9)";
        const std::string identifier =
            letters + "(" + letters + "|" + digits + ")*";

        const char* keywords[] = {"int",  "float", "string", "char", "bool",
                                  "func", "class", "for",    "while", "if",
                                  "else", "print", "return", "true",  "false"};
        for (const char* keyword : keywords)
        {
            add_token(lexer, keyword, keyword);
        }

        add_token(lexer, "IDENTIFIER", identifier);
        add_token(lexer, "REAL_LITERAL", digits + "+\\." + digits + "+");
        add_token(lexer, "INTEGER_LITERAL", digits + "+");
        add_token(lexer, "EQUAL", "==");
        add_token(lexer, "NOT_EQUAL", "!=");
        add_token(lexer, "GREATER_EQUAL", ">=");
        add_token(lexer, "LESS_EQUAL", "<=");
        add_token(lexer, "AND", "&&");
        add_token(lexer, "OR", "\\|\\|");
        add_token(lexer, "PLUS", "\\+");
        add_token(lexer, "MINUS", "\\-");
        add_token(lexer, "STAR", "\\*");
        add_token(lexer, "SLASH", "/");
        add_token(lexer, "GREATER", ">");
        add_token(lexer, "LESS", "<");
        add_token(lexer, "NOT", "!");
        add_token(lexer, "ASSIGN", "=");
        add_token(lexer, "LEFT_PAREN", "\\(");
        add_token(lexer, "RIGHT_PAREN", "\\)");
        add_token(lexer, "LEFT_BRACE", "\\{");
        add_token(lexer, "RIGHT_BRACE", "\\}");
        add_token(lexer, "LEFT_BRACKET", "\\[");
        add_token(lexer, "RIGHT_BRACKET", "\\]");
        add_token(lexer, "SEMICOLON", ";");
        add_token(lexer, "COMMA", ",");
        add_token(lexer, "WHITESPACE", " ");
        add_token(lexer, "WHITESPACE", "\t");
        add_token(lexer, "WHITESPACE", "\n");
        add_token(lexer, "WHITESPACE", "\r");
    }

    void tokenize_regular_text(Designar::Lexer& lexer,
                               const std::string& source,
                               std::size_t start,
                               std::size_t end,
                               Designar::DynArray<Designar::Token>& result)
    {
        if (start == end)
        {
            return;
        }

        const std::string fragment = source.substr(start, end - start);
        const Designar::DynArray<Designar::Token> tokens =
            lexer.tokenize(fragment);

        for (Designar::nat_t i = 0; i < tokens.size(); ++i)
        {
            const Designar::Token& token = tokens[i];
            result.append(Designar::Token{
                token.name, token.lexeme,
                static_cast<Designar::nat_t>(start + token.position)});
        }
    }

    Designar::DynArray<Designar::Token> tokenize_source(
        Designar::Lexer& lexer, const std::string& source)
    {
        Designar::DynArray<Designar::Token> result;
        std::size_t position = 0;
        std::size_t fragment_start = 0;

        while (position < source.size())
        {
            if (source[position] == '/' && position + 1 < source.size() &&
                source[position + 1] == '/')
            {
                tokenize_regular_text(lexer, source, fragment_start, position,
                                     result);
                position += 2;
                while (position < source.size() && source[position] != '\n' &&
                       source[position] != '\r')
                {
                    ++position;
                }
                fragment_start = position;
                continue;
            }

            const char quote = source[position];
            if (quote == '"' || quote == '\'')
            {
                tokenize_regular_text(lexer, source, fragment_start, position,
                                     result);
                const std::size_t token_start = position++;

                if (quote == '\'')
                {
                    if (position == source.size() || source[position] == quote)
                    {
                        throw std::runtime_error(
                            "literal de caracter vacio o sin cerrar en offset " +
                            std::to_string(token_start));
                    }

                    ++position;
                    if (position == source.size() || source[position] != quote)
                    {
                        throw std::runtime_error(
                            "el literal de caracter debe contener un solo "
                            "caracter en offset " +
                            std::to_string(token_start));
                    }
                }
                else
                {
                    while (position < source.size() && source[position] != quote)
                    {
                        ++position;
                    }
                    if (position == source.size())
                    {
                        throw std::runtime_error(
                            "literal de cadena sin cerrar en offset " +
                            std::to_string(token_start));
                    }
                }

                ++position;
                result.append(Designar::Token{
                    quote == '"' ? "STRING_LITERAL" : "CHAR_LITERAL",
                    source.substr(token_start, position - token_start),
                    static_cast<Designar::nat_t>(token_start)});
                fragment_start = position;
                continue;
            }

            ++position;
        }

        tokenize_regular_text(lexer, source, fragment_start, source.size(),
                             result);
        return result;
    }
}

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
        Designar::Lexer lexer;
        add_language_rules(lexer);

        const Designar::DynArray<Designar::Token> tokens =
            tokenize_source(lexer, source);

        for (Designar::nat_t i = 0; i < tokens.size(); ++i)
        {
            const Designar::Token& token = tokens[i];
            if (token.name != "WHITESPACE")
            {
                std::cout << token.name << " \"" << token.lexeme << "\" @"
                          << token.position << '\n';
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
