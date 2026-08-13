#include <iostream>
#include "lexer.h"
#include "parser.h"
#include <map>

void printTokens(std::vector<Token> tokens) {
    std::map<tokenType, std::string> typeToString = {
        {tokenType::LETTER, "LETTER"},
        {tokenType::STRING, "STRING"},
        {tokenType::SYMBOL, "SYMBOL"},
        {tokenType::NUMBER, "NUMBER"},
        {tokenType::KEYWORD, "KEYWORD"},
        {tokenType::IDENTIFIER, "IDENTIFIER"},
        {tokenType::DATATYPE, "DATATYPE"},
        {tokenType::SEMICOLON, "SEMICOLON"},
        {tokenType::LPARANTHESIS, "LPARANTHESIS"},
        {tokenType::RPARANTHESIS, "RPARANTHESIS"},
        {tokenType::COMMA, "COMMA"},
        {tokenType::INVALID, "INVALID"},
        {tokenType::END, "END"},
        {tokenType::EMPTY, "EMPTY"},
        {tokenType::FUNCTION, "FUNCTION"},
        {tokenType::LCURLYBRACKET, "LCURLYBRACKET"},
        {tokenType::RCURLYBRACKET, "RCURLYBRACKET"}
    };

    for (int i = 0; i < tokens.size(); i++) {
        std::cout << "Token #" << (i + 1) << ": " << tokens[i].lexeme << "\t\t\t" << "type: " << typeToString[tokens[i].type] << "\n";
    }
}

int main() {
    std::vector<Token> tokens;
    std::unique_ptr<ASTNode> parsedTokens;

    file f("test.txt");

    if (!f.is_open()) {
        std::cerr << "Failed to open file!\n";
        return 1;
    }

    tokens = Lexer::tokenize(f);


    printTokens(tokens);

    std::cout << "\n\nParsed tokens: \n\n";

    parsedTokens = Parser::parse(tokens);

    Parser::printAST(*parsedTokens, 0);

    return 0;
}

