#pragma once

#include "lexer.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <iostream>

enum class ASTNodeType {
    PROGRAM,
    FUNCTIONDECLARATION,
    VARIABLEDECLARATION,
    ASSIGNMENT,
    IFSTATE,
    WHILESTATE,
    FORSTATE,
    RETURNSTATE,
    BINARYEXPR,
    UNARYEXPR,
    NUMLITERAL,
    STRINGLITERAL,
    IDENTIFER,
    CALL,
    BLOCK
};

struct ASTNode;

struct AssignmentBranchNode {
    std::unique_ptr<ASTNode> varRef;
    std::unique_ptr<ASTNode> value;
};

struct AssignmentNode {
    std::string value;
};

struct VarRefNode {
    std::string varRef;
};

struct NumLitNode {
    int num;
};

struct StringNode {
    std::string value;
};

struct BinaryExprNode {
    Token op;
    std::unique_ptr<ASTNode> left, right;
};
struct UnaryExprNode {
    Token op;
    std::unique_ptr<ASTNode> value;
};

using ASTNodeValue = std::variant<NumLitNode, BinaryExprNode, AssignmentBranchNode, AssignmentNode, VarRefNode, StringNode, UnaryExprNode>;

struct ASTNode {
    ASTNodeValue value;
};

class Parser {
public:
    static std::unique_ptr<ASTNode> parse(std::vector<Token>& inputtedTokens);
    static void printAST(const ASTNode& node, int depth); // prints newly parsed AST
private:
    static inline std::vector<Token> tokens;
    static inline int current;

    static Token peek(); // returns cur token
    static Token advance(); // increments to the next token and returns the one before it
    static bool check(const tokenType& type); // returns true based on if current token is of the inputted through token type
    static bool match(const tokenType& type); // if current token is of inputted token type then it calls the advance function and returns true

    // below contains the hiearchy of parsing functions, since this compiler will be using descentive recursion to parse through tokens
    // the parsing hiearchy is ordered from first function right below this to the final function at the end
    static std::unique_ptr<ASTNode> parseAssignment();
    static std::unique_ptr<ASTNode> parseEquality();
    static std::unique_ptr<ASTNode> parseShift();
    static std::unique_ptr<ASTNode> parseAdditive();
    static std::unique_ptr<ASTNode> parseMultaplacative();
    static std::unique_ptr<ASTNode> parseUnary();
    static std::unique_ptr<ASTNode> parsePrimary();
};
