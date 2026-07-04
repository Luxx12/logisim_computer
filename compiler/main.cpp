#include <iostream>
#include "lexer.h"
#include <map>

void printTokens(std::vector<Token> tokens) {
    std::map<Type, std::string> typeToString = {
        {Type::LETTER, "LETTER"},
        {Type::STRING, "STRING"},
        {Type::SYMBOL, "SYMBOL"},
        {Type::NUMBER, "NUMBER"},
        {Type::KEYWORD, "KEYWORD"},
        {Type::IDENTIFIER, "IDENTIFIER"},
        {Type::SEMICOLON, "SEMICOLON"},
        {Type::LPARANTHESIS, "LPARANTHESIS"},
        {Type::RPARANTHESIS, "RPARANTHESIS"},
        {Type::INVALID, "INVALID"},
        {Type::END, "END"},
        {Type::EMPTY, "EMPTY"},
        {Type::FUNCTION, "FUNCTION"},
        {Type::LCURLYBRACKET, "LCURLYBRACKET"},
        {Type::RCURLYBRACKET, "RCURLYBRACKET"}
    };

    for (int i = 0; i < tokens.size(); i++) {
        std::cout << "Token #" << (i + 1) << ": " << tokens[i].lexeme << "\t\t\t" << "type: " << typeToString[tokens[i].type] << "\n";

    }
}

int main() {
    std::vector<Token> tokens;

    file f("test.txt");

    if (!f.is_open()) {
        std::cerr << "Failed to open file!\n";
        return 1;
    }

    tokens = Lexer::tokenize(f);


    printTokens(tokens);

    return 0;
}

