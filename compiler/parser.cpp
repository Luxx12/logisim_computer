#include "parser.h"

Token Parser::peek() {
    return tokens[current];
}

Token Parser::advance() {
    Token t = tokens[current];
    if (tokens[current].type != tokenType::END) current++;

    return t;
}

bool Parser::check(tokenType type) {
    return (tokens[current].type == type);
}

bool Parser::match(tokenType type) {
    if (check(type)) {
        Token t = advance();
        return true;
    }
    return false;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    std::unique_ptr<ASTNode> left = parseMultaplacative();

    while (peek().lexeme == "*" || peek().lexeme == "/") {
        Token op = peek();
        advance();
        std::unique_ptr<ASTNode> right = parseMultaplacative();

        std::unique_ptr<ASTNode> newTree = std::make_unique<ASTNode>();
        newTree->value = BinaryExprNode(op, std::move(left), std::move(right));
        left = std::move(newTree);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseMultaplacative() {
    std::unique_ptr<ASTNode> left = parsePrimary();

    while (peek().lexeme == "*" || peek().lexeme == "/") {
        Token op = peek();
        advance();
        std::unique_ptr<ASTNode> right = parsePrimary();

        std::unique_ptr<ASTNode> newTree = std::make_unique<ASTNode>();
        newTree->value = BinaryExprNode(op, std::move(left), std::move(right));
        left = std::move(newTree);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    // access current token and creating pointer to a AST node to possibly store that token's lexeme into
    Token t = peek();
    std::unique_ptr<ASTNode> primNode = std::make_unique<ASTNode>();

    // checks if token is of a primary type and assigns to the primary node as AST Node that varies on its AST type
    // depending on what the token is, if its not a primary then an exception is thrown
    if (match(tokenType::STRING)) {
        primNode->value = StringNode(t.lexeme);
    }
    else if (match(tokenType::NUMBER)) {
        primNode->value = NumLitNode(std::stoi(t.lexeme));
    }
    else if (match(tokenType::IDENTIFIER)) {
        primNode->value = VarRefNode(t.lexeme);
    }
    else {
        throw std::runtime_error("Expected an expression.");
    }

    return primNode;
}

std::unique_ptr<ASTNode> Parser::parse(std::vector<Token> inputtedTokens) {
    tokens = inputtedTokens;

}