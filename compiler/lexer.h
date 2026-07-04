#pragma once
#include <variant>
#include <fstream>
#include <iostream>
#include <vector>
#include <cctype>

using file = std::ifstream; // shorten the type name, so I dont have to write allat

// dictates token type when lexed, the sterotypical typing will be used EX: 1 = number, if = keyword, "A" = String,
// x = identifer . Identifiers, keywords, and letters will all be stored as strings.
// type LETTER serves as a temp state to determine whether the token is an identifier, keyword, or string literal when
// done appending to the current token
enum class tokenType {
    LETTER,
    STRING,
    SYMBOL,
    NUMBER,
    KEYWORD,
    IDENTIFIER,
    DATATYPE,
    FUNCTION,
    SEMICOLON,
    LPARANTHESIS,
    RPARANTHESIS,
    LCURLYBRACKET,
    RCURLYBRACKET,
    INVALID,
    END,
    EMPTY
};

struct Token {
    tokenType type;
    std::string lexeme; // the actual content of the token, type can be variable
};

class Lexer {
public:
    static std::vector<Token> tokenize(file& f);
};

