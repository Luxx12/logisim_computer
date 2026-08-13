#pragma once

#include "lexer.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <iostream>

struct ASTNode;

struct BlockNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
};

struct ReturnNode {
    std::unique_ptr<ASTNode> returnValue;
};

struct IfNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> block;
};

struct WhileNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> block;
};

struct ForNode {
    std::unique_ptr<ASTNode> initialization;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> block;
};

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

struct FuncIdentifier {
    std::string funcIdentifier;
};

struct DataTypeNode {
    std::string dataType;
};

struct DeclarationNode {
    std::unique_ptr<ASTNode> dataType;
    std::unique_ptr<ASTNode> varAssignment;
};

struct ParameterNode {
    std::unique_ptr<ASTNode> dataType;
    std::unique_ptr<ASTNode> varAssignment;
};

struct FunctionDecNode {
    std::unique_ptr<ASTNode> returnType;
    std::unique_ptr<ASTNode> funcIdentifer;
    std::vector<std::unique_ptr<ASTNode>> parameters;
    std::unique_ptr<ASTNode> block;
};

struct FunctionCallNode {
    std::unique_ptr<ASTNode> funcIdentifier;
    std::vector<std::unique_ptr<ASTNode>> parameters;
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

using ASTNodeValue = std::variant<NumLitNode, BinaryExprNode, AssignmentBranchNode, AssignmentNode, VarRefNode, FuncIdentifier,
    StringNode, UnaryExprNode, BlockNode, ReturnNode, IfNode, WhileNode, ForNode, DeclarationNode, DataTypeNode, FunctionDecNode,
    ParameterNode, FunctionCallNode>;

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
    static std::unique_ptr<ASTNode> parseFunctionDeclaration(std::unique_ptr<ASTNode> dataTypeNode);
    static std::unique_ptr<ASTNode> determineDeclaration();
    static std::unique_ptr<ASTNode> parseFunctionCall(std::unique_ptr<ASTNode> funcIdentifier);
    static std::unique_ptr<ASTNode> parseReturn();
    static std::unique_ptr<ASTNode> parseVarDeclaration(std::unique_ptr<ASTNode> dataTypeNode);
    static std::unique_ptr<ASTNode> parseStatement();
    static std::unique_ptr<ASTNode> parseIfStmt();
    static std::unique_ptr<ASTNode> parseWhileLoop();
    static std::unique_ptr<ASTNode> parseForLoop();
    static std::unique_ptr<ASTNode> parseExprStmt();
    static std::unique_ptr<ASTNode> parseBlock();
    static std::unique_ptr<ASTNode> parseDataType();
    static std::unique_ptr<ASTNode> parseAssignment();
    static std::unique_ptr<ASTNode> parseEquality();
    static std::unique_ptr<ASTNode> parseRelational();
    static std::unique_ptr<ASTNode> parseShift();
    static std::unique_ptr<ASTNode> parseAdditive();
    static std::unique_ptr<ASTNode> parseMultaplacative();
    static std::unique_ptr<ASTNode> parseUnary();
    static std::unique_ptr<ASTNode> determineFunctionCall();
    static std::unique_ptr<ASTNode> parsePrimary();
};
