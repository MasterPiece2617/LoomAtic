#include <loomatic/loomatic_lexer.hpp>

#include <cstddef>
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
            letters + "(" + letters + "|" + digits + "|_)*";

        const struct
        {
            const char* lexeme;
            const char* token_name;
        } keywords[] = {{"int", "KW_INT"},
                        {"float", "KW_FLOAT"},
                        {"string", "KW_STRING"},
                        {"char", "KW_CHAR"},
                        {"bool", "KW_BOOL"},
                        {"func", "KW_FUNC"},
                        {"class", "KW_CLASS"},
                        {"for", "KW_FOR"},
                        {"while", "KW_WHILE"},
                        {"if", "KW_IF"},
                        {"else", "KW_ELSE"},
                        {"print", "KW_PRINT"},
                        {"return", "KW_RETURN"},
                        {"true", "BOOL_LITERAL"},
                        {"false", "BOOL_LITERAL"}};
        for (const auto& keyword : keywords)
        {
            add_token(lexer, keyword.token_name, keyword.lexeme);
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

    void tokenize_regular_text(const Designar::Lexer& lexer,
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
        const Designar::Lexer& lexer, const std::string& source)
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

    void advance_location(const std::string& source,
                          std::size_t target,
                          std::size_t& offset,
                          std::size_t& line,
                          std::size_t& column)
    {
        while (offset < target)
        {
            const char character = source[offset++];
            if (character == '\r')
            {
                ++line;
                column = 1;
                if (offset < source.size() && source[offset] == '\n')
                {
                    ++offset;
                }
            }
            else if (character == '\n')
            {
                ++line;
                column = 1;
            }
            else
            {
                ++column;
            }
        }
    }
}

namespace LoomAtic
{
    Lexer::Lexer()
    {
        add_language_rules(designar_lexer);
    }

    Designar::DynArray<Token> Lexer::tokenize(const std::string& source) const
    {
        const Designar::DynArray<Designar::Token> designar_tokens =
            tokenize_source(designar_lexer, source);
        Designar::DynArray<Token> tokens;
        std::size_t offset = 0;
        std::size_t line = 1;
        std::size_t column = 1;

        for (Designar::nat_t i = 0; i < designar_tokens.size(); ++i)
        {
            const Designar::Token& token = designar_tokens[i];
            const std::size_t token_offset =
                static_cast<std::size_t>(token.position);
            advance_location(source, token_offset, offset, line, column);
            tokens.append(
                Token{token.name, token.lexeme, token_offset, line, column});
        }

        return tokens;
    }
}
