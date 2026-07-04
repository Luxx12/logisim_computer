#include "lexer.h"

void letterStateCheck(Token& t){
    if (t.lexeme == "if" || t.lexeme  == "else if" || t.lexeme  == "else" || t.lexeme  == "for" || t.lexeme  == "while"
        || t.lexeme == "return") {
        t.type = tokenType::KEYWORD;
    }
    else if (t.lexeme == "int" || t.lexeme == "double" || t.lexeme == "char") {
        t.type = tokenType::DATATYPE;
    }
    else {
        t.type = tokenType::IDENTIFIER;
    }
}

 std::vector<Token> Lexer::tokenize(file& f) {
    std::string line, content;
    std::vector<Token> tokens;
    Token curToken = {tokenType::EMPTY, ""};


    // read each line
    while (std::getline(f,line)) {

        // read each char in line and process its token type from there
        for (int i = 0; i < line.size(); i++) {
            char ch = line[i];

            // the first if statement is for checking if token is a type contains comments that details the structure for
            // how the overall type checking procedure works, some procedures might be slightly variable
            if (std::isdigit(ch)) {

                // if type is empty, then this token will be of a number type
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::NUMBER;
                }
                // if its of its own type continue
                else if (curToken.type == tokenType::NUMBER){}
                // if current token is a letter then keep appending it to the token of type letter
                else if (curToken.type == tokenType::LETTER) {
                    // just dont do shit ig
                }
                // if its not either a letter or a token of type number then add cur token to tokens vector, and create a
                // new token of type number
                else{
                    // check to see if current token type equal to letter but actually end up being a keyword or identifer
                    if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                    tokens.push_back(curToken);
                    curToken = {tokenType::NUMBER, ""};
                }

                curToken.lexeme += ch;
            }
            // this checks for letters & keywords
            else if (std::isalpha(ch)) {

                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::LETTER;
                }
                // if its of its own type continue
                else if (curToken.type == tokenType::LETTER){}
                else{
                    // check to see if current token type equal to letter but actually end up being a keyword or identifer
                    if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                    tokens.push_back(curToken);

                    curToken = {tokenType::LETTER, ""};
                }

                curToken.lexeme+=ch;
            }
            // checks for functions

            // check for a possible conditional operator and append rest of operator if possible
            else if (ch == '!' || ch == '>' || ch == '<') {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::SYMBOL;
                }
                else {
                    // check to see if current token type equal to letter but actually end up being a keyword or identifer
                    if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                    tokens.push_back(curToken);

                    curToken = {tokenType::SYMBOL, ""};
                }

                curToken.lexeme+=ch;

                // check if next char after is a '=' to append to the current token, and skip over to the next char if so
                if (i + 1 < line.size() && line[i + 1] == '=') {
                    curToken.lexeme += line[i + 1];
                    i++;
                }

                // checking for bitwise operaters, and updating the current token and index if so
                if (i + 1 <  line.size() && ch == '<' && line[i + 1] == '<') {
                    curToken.lexeme += line[i + 1];
                    i++;
                }

                if (i + 1 <  line.size() && ch == '>' && line[i + 1] == '>') {
                    curToken.lexeme += line[i + 1];
                    i++;
                }

                // if current tokens' lexem is '!' then set type as invalid since thats not compilable
                if (curToken.lexeme == "!") {
                    // *** display error here ***
                    curToken.type = tokenType::INVALID;
                }
            }
            // check for a possible arithmetic operator and append rest of operator if possible
            else if (ch == '-' || ch == '+' || ch == '=' || ch == '/' || ch == '*' || ch == '^' ) {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::SYMBOL;
                }
                else {
                    // check to see if current token type equal to letter but actually end up being a keyword or identifer
                    if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                    tokens.push_back(curToken);

                    curToken = {tokenType::SYMBOL, ""};
                }

                curToken.lexeme += ch;

                // check if next char after is a '=' to append to the current token, and skip over to the next char if so
                if (i + 1 < line.size() && line[i + 1] == '=') {
                    curToken.lexeme += line[i + 1];
                    i++;
                }
            }
            // check for semicolon
            else if (ch == ';') {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::SEMICOLON;
                    curToken.lexeme += ch;
                }
                else {
                    // check to see if current token type equal to letter but actually end up being a keyword or identifer
                    if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                    tokens.push_back(curToken);

                    curToken = {tokenType::SEMICOLON, ";"};
                }
            }
            // check for paranthesis, first ( checks also determines if the previous token of type letter could
            // be a function
            else if (ch == '(') {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::LPARANTHESIS;
                    curToken.lexeme += ch;
                }
                else {
                    // if previous token is of type letter and its not a keyword, then that means it has to be a function
                    if (curToken.type == tokenType::LETTER) {

                        letterStateCheck(curToken);

                        if (curToken.type == tokenType::IDENTIFIER) curToken.type = tokenType::FUNCTION;

                    }
                    tokens.push_back(curToken);

                    curToken = {tokenType::LPARANTHESIS, "("};
                }
            }
            else if (ch == ')') {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::RPARANTHESIS;
                    curToken.lexeme += ch;
                }
                else {
                    // check to see if current token type equal to letter but actually end up being a keyword or identifer
                    if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                    tokens.push_back(curToken);

                    curToken = {tokenType::RPARANTHESIS, ")"};
                }
            }
            // check for curly brackets
            else if (ch == '{') {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::LCURLYBRACKET;
                    curToken.lexeme += ch;
                }
                else {
                    // if previous token is of type letter and its not a keyword, then that means it has to be a function
                    if (curToken.type == tokenType::LETTER) {

                        letterStateCheck(curToken);

                    }
                    tokens.push_back(curToken);

                    curToken = {tokenType::LCURLYBRACKET, "{"};
                }
            }
            else if (ch == '}') {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::RCURLYBRACKET;
                    curToken.lexeme += ch;
                }
                else {
                    // if previous token is of type letter and its not a keyword, then that means it has to be a function
                    if (curToken.type == tokenType::LETTER) {

                        letterStateCheck(curToken);

                    }
                    tokens.push_back(curToken);

                    curToken = {tokenType::RCURLYBRACKET, "}"};
                }
            }

            // check for string literals, if current token state is a string literal then it will continously append char
            // until the delimiter is found
            else if (ch == '\"') {
                if (curToken.type == tokenType::EMPTY) {
                    curToken.type = tokenType::STRING;
                }
                else {
                    // check to see if current token type equal to letter but actually end up being a keyword or identifer
                    if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                    tokens.push_back(curToken);

                    curToken = {tokenType::STRING, ""};
                }

                i++;
                while (i < line.size() && line[i] != '\"') {
                    curToken.lexeme += line[i];
                    i++;
                }

                // skip over char containing ending delimeter so it won't count as a token
                //if (i + 1 < line.size()) i++;
            }
            // if space is encountered, either it creates a new empty token or checks state to see if its a letter type
            // which is allowed to have spaces
            else if (std::isspace(ch)) {
                if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                if (curToken.type != tokenType::EMPTY) tokens.push_back(curToken);

                curToken = {tokenType::EMPTY, ""};
            }
            // final case if for an invalid type
            else {
                // check to see if current token type equal to letter but actually end up being a keyword or identifer
                if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);

                if (curToken.type != tokenType::EMPTY) tokens.push_back(curToken);

                curToken = {tokenType::INVALID, ""};
            }
        }
    }

    if (curToken.type != tokenType::EMPTY) {
        if (curToken.type == tokenType::LETTER) letterStateCheck(curToken);
        tokens.push_back(curToken);
    }

    tokens.push_back({tokenType::END,""});

    return tokens;
}