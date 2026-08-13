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

std::unique_ptr<ASTNode> Parser::parseStatement() {

    if (peek().lexeme == "if") {
        return parseIfStmt();
    }
    else if (peek().lexeme == "for") {
        return parseForLoop();
    }
    else if (peek().lexeme == "while") {
        return parseWhileLoop();
    }
    else if (peek().lexeme == "return") {
        return parseReturn();
    }
    else if (peek().lexeme == "{") {
        return parseBlock();
    }
    else if (peek().lexeme == ";") {

    }
    else if (peek().type == tokenType::DATATYPE) {
        return determineDeclaration();
    }
    else {
        return parseExprStmt();
    }
}

std::unique_ptr<ASTNode> Parser::parseReturn() {
    std::unique_ptr<ASTNode> returnNode = std::make_unique<ASTNode>();
    std::unique_ptr<ASTNode> returnValue;

    advance(); // consume return key word

    returnValue = parseAssignment();

    returnNode->value = ReturnNode(std::move(returnValue));

    if (!match(tokenType::SEMICOLON)) throw std::runtime_error("Missing semicolon");

    return returnNode;
}

std::unique_ptr<ASTNode> Parser::parseIfStmt() {
    std::unique_ptr<ASTNode> ifNode = std::make_unique<ASTNode>();
    std::unique_ptr<ASTNode> conditionNode, blockNode;

    // consume 'if' keyword and '('
    if (peek().lexeme != "if") throw std::runtime_error("Incorrect if loop syntax.");
    advance();

    if (!match(tokenType::LPARANTHESIS)) throw std::runtime_error("Incorrect if loop syntax.");

    // parse condition, consume ']' & '{' then parse if statement's block
    conditionNode = parseAssignment();

    if (!match(tokenType::RPARANTHESIS)) throw std::runtime_error("Incorrect if loop syntax.");

    blockNode = parseBlock();

    ifNode->value = IfNode(std::move(conditionNode), std::move(blockNode));

    return ifNode;
}

std::unique_ptr<ASTNode> Parser::parseWhileLoop() {
    std::unique_ptr<ASTNode> whileNode = std::make_unique<ASTNode>();
    std::unique_ptr<ASTNode> conditionNode, blockNode;

    // consume 'while' keyword and '('
    if (peek().lexeme != "while") throw std::runtime_error("Incorrect while loop syntax.");
    advance();

    if (!match(tokenType::LPARANTHESIS)) throw std::runtime_error("Incorrect while loop syntax.");

    // parse condition, consume ')' & '{' then parse while loop's block
    conditionNode = parseAssignment();

    if (!match(tokenType::RPARANTHESIS)) throw std::runtime_error("Incorrect while loop syntax.");

    blockNode = parseBlock();

    whileNode->value = WhileNode(std::move(conditionNode), std::move(blockNode));

    return whileNode;
}

std::unique_ptr<ASTNode> Parser::parseForLoop() {
    std::unique_ptr<ASTNode> forNode = std::make_unique<ASTNode>();
    std::unique_ptr<ASTNode> initNode, conditionNode, incNode, blockNode;

    // consume 'for' keyword and '('
    if (peek().lexeme != "for") throw std::runtime_error("Incorrect for loop syntax.");
    advance();

    if (!match(tokenType::LPARANTHESIS)) throw std::runtime_error("Incorrect for loop syntax.");

    // parse initliization, condition, & increment. consume ';' when needed
    initNode = parseAssignment();
    if (!match(tokenType::SEMICOLON)) throw std::runtime_error("Incorrect for loop syntax.");
    conditionNode = parseAssignment();
    if (!match(tokenType::SEMICOLON)) throw std::runtime_error("Incorrect for loop syntax.");
    incNode = parseAssignment();

    if (!match(tokenType::RPARANTHESIS)) throw std::runtime_error("Incorrect while loop syntax.");

    blockNode = parseBlock();

    forNode->value = ForNode(std::move(initNode),std::move(conditionNode), std::move(incNode), std::move(blockNode));

    return forNode;
}
std::unique_ptr<ASTNode> Parser::determineDeclaration() {
    std::unique_ptr<ASTNode> dataTypeNode = parseDataType();

    // determines which node to build depending on whether the current token is a function or var identifier
    if (peek().type == tokenType::IDENTIFIER) {
        std::unique_ptr<ASTNode> varDecNode = parseVarDeclaration(std::move(dataTypeNode));

        return varDecNode;
    }
    else {
        std::unique_ptr<ASTNode> funcSignatureNode = parseFunctionDeclaration(std::move(dataTypeNode));

        return funcSignatureNode;
    }
}

std::unique_ptr<ASTNode> Parser::parseVarDeclaration(std::unique_ptr<ASTNode> dataTypeNode) {
    std::unique_ptr<ASTNode> decNode = std::make_unique<ASTNode>();

    std::unique_ptr<ASTNode> assignmentNode = parseExprStmt();

    decNode->value = DeclarationNode(std::move(dataTypeNode),std::move(assignmentNode));

    return decNode;
}

std::unique_ptr<ASTNode> Parser::parseFunctionDeclaration(std::unique_ptr<ASTNode> dataTypeNode) {
    std::vector<std::unique_ptr<ASTNode>> parameters = {};
    std::unique_ptr<ASTNode> funcNode = std::make_unique<ASTNode>();
    std::unique_ptr<ASTNode> funcIdentifer = parsePrimary();
    std::unique_ptr<ASTNode> paramDataType, paramVarRef;

    funcNode->value = FunctionDecNode(std::move(dataTypeNode), std::move(funcIdentifer), {}, nullptr);
    FunctionDecNode& function = std::get<FunctionDecNode>(funcNode->value);

    // consume '('
    if (!match(tokenType::LPARANTHESIS)) throw std::runtime_error("incorrect function signature syntax");

    // parse parameters
    while (!match(tokenType::RPARANTHESIS)) {
        paramDataType = parseDataType();
        paramVarRef = parsePrimary();

        std::unique_ptr<ASTNode> parameterNode = std::make_unique<ASTNode>();
        parameterNode->value = ParameterNode(std::move(paramDataType), std::move(paramVarRef));

        function.parameters.push_back(std::move(parameterNode));
        match(tokenType::COMMA);
    }

    function.block = parseBlock();

    return funcNode;
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall(std::unique_ptr<ASTNode> funcIdentifier) {
    std::unique_ptr<ASTNode> funcNode = std::make_unique<ASTNode>();
    std::unique_ptr<ASTNode> funcIdentifer = std::move(funcIdentifer);
    std::unique_ptr<ASTNode> paramVarRef;

    funcNode->value = FunctionCallNode(std::move(funcIdentifer), {});
    FunctionCallNode& function = std::get<FunctionCallNode>(funcNode->value);

    // consume '('
    if (!match(tokenType::LPARANTHESIS)) throw std::runtime_error("incorrect function signature syntax");

    // parse parameters
    while (!match(tokenType::RPARANTHESIS)) {
        paramVarRef = parsePrimary();

        function.parameters.push_back(std::move(paramVarRef));
        match(tokenType::COMMA);
    }

    return funcNode;
}

std::unique_ptr<ASTNode> Parser::parseExprStmt() {
    std::unique_ptr<ASTNode> exprStmt = parseAssignment();

    // consume ';' if it exists, throws an error if not
    if (!match(tokenType::SEMICOLON)) {
        throw std::runtime_error("No semicolon.");
    }

    return exprStmt;
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    std::unique_ptr<ASTNode> block = std::make_unique<ASTNode>();
    block->value = BlockNode({});
    BlockNode& curBlock = std::get<BlockNode>(block->value);


    // consume '{'
    if (!match(tokenType::LCURLYBRACKET)) throw std::runtime_error("Incorrect function block syntax");

    // continously parse each statement within the block until the block/file ends
    while (peek().lexeme != "}" && peek().type != tokenType::END) {
        curBlock.statements.push_back(parseStatement());
    }

    // consume '}'
    if (!match(tokenType::RCURLYBRACKET)) throw std::runtime_error("Incorrect function block syntax");

    return block;
}

std::unique_ptr<ASTNode> Parser::parseDataType() {
    std::unique_ptr<ASTNode> dataType = std::make_unique<ASTNode>();
    dataType->value = DataTypeNode(peek().lexeme);
    advance();

    return dataType;
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
    std::unique_ptr<ASTNode> left = parseRelational();

    while (peek().lexeme == "==" || peek().lexeme == "!=") {
        Token op = peek();
        advance();

        std::unique_ptr<ASTNode> right = parseRelational();

        std::unique_ptr<ASTNode> subTree = std::make_unique<ASTNode>();
        subTree->value = BinaryExprNode(op,std::move(left),std::move(right));
        left = std::move(subTree);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseRelational() {
    std::unique_ptr<ASTNode> left = parseShift();

    while (peek().lexeme == ">=" || peek().lexeme == "<=" || peek().lexeme == ">" || peek().lexeme == "<") {
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
    return determineFunctionCall();
}

std::unique_ptr<ASTNode> Parser::determineFunctionCall() {
    std::unique_ptr<ASTNode> functionCall = std::make_unique<ASTNode>();
    std::unique_ptr<ASTNode> funcIdentifer = parseAssignment();

    if (peek().type == tokenType::LPARANTHESIS) {
        return parseFunctionCall(std::move(funcIdentifer));
    }

    return funcIdentifer;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    // access current token and creating pointer to a AST node to possibly store that token's lexeme into
    Token t = peek();
    std::unique_ptr<ASTNode> primNode = std::make_unique<ASTNode>();

    // checks if token is of a primary type and assigns to the primary node as AST Node that varies on its AST type
    // depending on what the token is, if it's not a primary then an exception is thrown
    if (match(tokenType::STRING)) {
        primNode->value = StringNode(t.lexeme);
    }
    else if (match(tokenType::NUMBER)) {
        primNode->value = NumLitNode(std::stoi(t.lexeme));
    }
    else if (match(tokenType::IDENTIFIER)) {
        primNode->value = VarRefNode(t.lexeme);
    }
    else if (match(tokenType::FUNCTION)) {
        primNode->value = FuncIdentifier(t.lexeme);
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
        else if constexpr(std::is_same_v<T, DataTypeNode>) {
            std::cout << indent << "Data Type(" << n.dataType << ")\n";
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
        else if constexpr (std::is_same_v<T, BlockNode>) {
            std::cout << indent << " Block:\n";
            for (size_t i = 0; i < n.statements.size(); i++) {
                printAST(*n.statements[i], depth + 2);
            }
        }
        else if constexpr(std::is_same_v<T, WhileNode>) {
            std::cout << indent << "While Loop\n";
            std::cout << indent << "  condition:\n";
            printAST(*n.condition, depth + 2);
            std::cout << indent << "  block:\n";
            printAST(*n.block, depth + 2);
        }
        else if constexpr(std::is_same_v<T, IfNode>) {
            std::cout << indent << "If Statement\n";
            std::cout << indent << "  condition:\n";
            printAST(*n.condition, depth + 2);
            std::cout << indent << "  block:\n";
            printAST(*n.block, depth + 2);
        }
        else if constexpr(std::is_same_v<T, ForNode>) {
            std::cout << indent << "For Loop\n";
            std::cout << indent << "  initialization:\n";
            printAST(*n.initialization, depth + 2);
            std::cout << indent << "  condition:\n";
            printAST(*n.condition, depth + 2);
            std::cout << indent << "  increment:\n";
            printAST(*n.increment, depth + 2);
            std::cout << indent << "  block:\n";
            printAST(*n.block, depth + 2);
        }
        else if constexpr(std::is_same_v<T, DeclarationNode>) {
            std::cout << indent << "Variable Declaration\n";
            std::cout << indent << "  DataType:\n";
            printAST(*n.dataType, depth + 2);
            std::cout << indent << "  Variable Assignment:\n";
            printAST(*n.varAssignment, depth + 2);
        }
        else if constexpr(std::is_same_v<T,ParameterNode>) {
            std::cout << indent << "Parameter\n";
            std::cout << indent << "  DataType:\n";
            printAST(*n.dataType, depth + 2);
            std::cout << indent << "  Variable Assignment:\n";
            printAST(*n.varAssignment, depth + 2);
        }
        else if constexpr(std::is_same_v<T, FunctionDecNode>) {
            std::cout << indent << "Function Declaration\n";
            std::cout << indent << "  ReturnType:\n";
            printAST(*n.returnType, depth + 2);
            std::cout << indent << "  Identifier:\n";
            printAST(*n.funcIdentifer, depth + 2);
            std::cout << indent << "  Parameters:\n";
            for (size_t i = 0; i < n.parameters.size(); i++) {
                printAST(*n.parameters[i], depth + 2);
            }
            std::cout << indent << "  Block:\n";
            printAST(*n.block, depth + 2);
        }

    }, node.value);
}

std::unique_ptr<ASTNode> Parser::parse(std::vector<Token>& inputtedTokens) {
    tokens = inputtedTokens;
    current = 0;
    return parseStatement();
}
