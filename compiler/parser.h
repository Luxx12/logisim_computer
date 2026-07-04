#pragma once

#include "lexer.h"
#include <vector>
#include <memory>
#include <stdexcept>

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

struct AssignmentBranch {
    std::unique_ptr<ASTNode> varRef;
    std::unique_ptr<ASTNode> value;
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

using ASTNodeValue = std::variant<NumLitNode, BinaryExprNode, AssignmentBranch, VarRefNode, StringNode>;

struct ASTNode {
    ASTNodeValue value;
};

class Parser {
public:
    std::unique_ptr<ASTNode> parse(std::vector<Token> inputtedTokens);
private:
    std::vector<Token> tokens;
    int current = 0;

    Token peek(); // returns cur token
    Token advance(); // increments to the next token and returns the one before it
    bool check(tokenType type); // returns true based on if current token is of the inputted through token type
    bool match(tokenType type); // if current token is of inputted token type then it calls the advance function and returns true

    // below contains the hiearchy of parsing functions, since this compiler will be using descentive recursion to parse through tokens
    // the parsing hiearchy is ordering from first function right below this to the final parsing function
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseMultaplacative();
    std::unique_ptr<ASTNode> parsePrimary();
};