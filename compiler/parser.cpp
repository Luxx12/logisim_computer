#include "parser.h"

#include <algorithm>

Token Parser::peek() {
    return tokens[current];
}

Token Parser::advance() {
    Token t = tokens[current];
    if (tokens[current].type != tokenType::END) current++;

    return t;
}

bool Parser::check(const tokenType& type) {
    return (tokens[current].type == type);
}

bool Parser::match(const tokenType& type) {
    if (check(type)) {
        Token t = advance();
        return true;
    }
    return false;
}

std::unique_ptr<ASTNode> Parser::parseAssignment() {
    std::unique_ptr<ASTNode> left = parseEquality();

    if (peek().lexeme == "=") {

        if (!std::holds_alternative<VarRefNode>(left->value)) {
            throw std::runtime_error("Incorrect assignment syntax.");
        }
        else {
            Token t = peek();
            advance();

            std::unique_ptr<ASTNode> right = parseAssignment();


            std::unique_ptr<ASTNode> newTree = std::make_unique<ASTNode>();

            newTree->value = AssignmentBranchNode(std::move(left), std::move(right));
            left = std::move(newTree);
        }
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    std::unique_ptr<ASTNode> left = parseShift();

    while (peek().lexeme == "==") {
        Token op = peek();
        advance();

        std::unique_ptr<ASTNode> right = parseShift();

        std::unique_ptr<ASTNode> subTree = std::make_unique<ASTNode>();
        subTree->value = BinaryExprNode(op,std::move(left),std::move(right));
        left = std::move(subTree);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseShift() {
    std::unique_ptr<ASTNode> left = parseAdditive();

    while (peek().lexeme == "<<" || peek().lexeme == ">>") {
        Token op = peek();
        advance();

        std::unique_ptr<ASTNode> right = parseAdditive();

        std::unique_ptr<ASTNode> subTree = std::make_unique<ASTNode>();
        subTree->value = BinaryExprNode(op, std::move(left), std::move(right));
        left = std::move(subTree);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    // begin creating left subtree here
    std::unique_ptr<ASTNode> left = parseMultaplacative();

    // checks if current token is additive, and continously creates a new sub tree in an order thats mathematically correct
    while (peek().lexeme == "+" || peek().lexeme == "-") {
        Token op = peek();
        advance();

        // create new right sub tree continously
        std::unique_ptr<ASTNode> right = parseMultaplacative();

        /*
         *  every thing found in the right tree gets put together into a new subtree that includes both right and left
         *  sub trees and the finally moves that pointer over to the left subtree and the loop continues again,
         *  this naturally orders every thing correctly so once evaluated leads to the correct answer
        */
        std::unique_ptr<ASTNode> newTree = std::make_unique<ASTNode>();
        newTree->value = BinaryExprNode(op, std::move(left), std::move(right));
        left = std::move(newTree);
    }

    return left;
}

// this function follows the same structure as parseAdditive
std::unique_ptr<ASTNode> Parser::parseMultaplacative() {
    std::unique_ptr<ASTNode> left = parseUnary();

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

// continously checks for stacking unary operators and creates a new sub tree that contains every '-' and a num literal
std::unique_ptr<ASTNode> Parser::parseUnary() {
    std::unique_ptr<ASTNode> newTree;

    if (peek().lexeme == "-") {
        Token t = peek();
        advance();

        newTree = std::make_unique<ASTNode>();
        newTree->value = UnaryExprNode(t,parseUnary());
    }

    if (newTree) {
        return newTree;
    }
    return parsePrimary();
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

void Parser::printAST(const ASTNode& node, int depth) {
    std::string indent(depth * 2, ' ');

    std::visit([&](const auto& n) {
        using T = std::decay_t<decltype(n)>;

        if constexpr (std::is_same_v<T, NumLitNode>) {
            std::cout << indent << "NumLit(" << n.num << ")\n";
        }
        else if constexpr (std::is_same_v<T, VarRefNode>) {
            std::cout << indent << "VarRef(" << n.varRef << ")\n";
        }
        else if constexpr (std::is_same_v<T, StringNode>) {
            std::cout << indent << "StringLit(" << n.value << ")\n";
        }
        else if constexpr (std::is_same_v<T, UnaryExprNode>) {
            std::cout << indent << "UnaryExpr(" << n.op.lexeme << ")\n";
            printAST(*n.value, depth + 1);
        }
        else if constexpr (std::is_same_v<T, BinaryExprNode>) {
            std::cout << indent << "BinaryExpr(" << n.op.lexeme << ")\n";
            std::cout << indent << "  left:\n";
            printAST(*n.left, depth + 2);
            std::cout << indent << "  right:\n";
            printAST(*n.right, depth + 2);
        }
        else if constexpr (std::is_same_v<T, AssignmentBranchNode>) {
            std::cout << indent << "Assignment\n";
            std::cout << indent << "  target:\n";
            printAST(*n.varRef, depth + 2);
            std::cout << indent << "  value:\n";
            printAST(*n.value, depth + 2);
        }
    }, node.value);
}

std::unique_ptr<ASTNode> Parser::parse(std::vector<Token>& inputtedTokens) {
    tokens = inputtedTokens;
    current = 0;
    return parseAssignment();
}
