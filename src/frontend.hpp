#ifndef __FRONTEND_H
#define __FRONTEND_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono> // the timer thing is not working


// incldue comments using the ~ operator

// currently the data type support is only for int , float and string types
// add to the code such that the string support is extended for other 
// data types as well 

// add the ability to sort the results by ascending or descending order

#define FAIL      "\e[0;31m"
#define SUCCESS   "\e[0;32m"
#define DEFAULT   "\e[0;37m"
#define DB_PROMPT "blue_db : " // this is subject to change
std::string InputBuffer;  



typedef enum
{
    LEXER_SUCCESS ,
    LEXER_FAIL 
} LEXER_STATUS;

typedef enum
{
    PARSER_SUCCESS ,
    PARSER_FAIL 
} PARSER_STATUS;


typedef enum 
{
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_BOOL,
    TOKEN_DATE,
    TOKEN_TIME,
    TOKEN_NEW, 
    TOKEN_DOUBLE_COLON, 
    TOKEN_DOT,
    TOKEN_COMMA,
    TOKEN_ADD, 
    TOKEN_LEFT_SQR_BRACKET,
    TOKEN_RIGHT_SQR_BRACKET,
    TOKEN_LEFT_PAREN, 
    TOKEN_RIGHT_PAREN,
    TOKEN_STRING_DATA,
    TOKEN_INT_DATA, 
    TOKEN_FLOAT_DATA,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_PRINT, 
    TOKEN_REMOVE,
    TOKEN_UPDATE , 
    TOKEN_NOT, 
    TOKEN_OR , 
    TOKEN_AND , 
    TOKEN_ARROW , 
    TOKEN_EQUAL_TO, 
    TOKEN_EQUALS , 
    TOKEN_NOT_EQUALS , 
    TOKEN_LESS_THAN , 
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN_EQUALS, 
    TOKEN_GREATER_THAN_EQUALS, 
    TOKEN_ID , 
    TOKEN_EXIT,
    TOKEN_END_OF_INPUT
} TOKEN_SET;


typedef enum 
{
    NODE_NEW, 
    NODE_ADD, 
    NODE_PRINT, 
    NODE_UPDATE , 
    NODE_REMOVE ,
    NODE_EXIT, 
    NODE_NOT, 
    NODE_AND, 
    NODE_OR,
    NODE_CONDITION, 
    NODE_CONDITION_EQUALS ,
    NODE_CONDITION_NOT_EQUALS, 
    NODE_CONDITION_GREATER_THAN ,
    NODE_CONDITION_LESS_THAN ,
    NODE_CONDITION_LESS_THAN_EQUALS, 
    NODE_CONDITION_GREATER_THAN_EQUALS,  
    NODE_SUB_VALUES,
    NODE_STRING,
    NODE_INT,
    NODE_FLOAT, 
} NODE_SET;


struct AST_NODE
{
    NODE_SET NODE_TYPE;
    std::string * PAYLOAD;
    std::string * SUB_PAYLOAD;
    std::vector<std::string> DATA_LIST;
    AST_NODE * CHILD;
    std::vector<AST_NODE *> CHILDREN;
    std::vector<AST_NODE *> UPDATE_CHILDREN;
    std::vector<std::vector<AST_NODE *>> MULTI_DATA;
    TOKEN_SET HELPER_TOKEN;
    AST_NODE()
    {
        CHILD = nullptr; // to avoid garbage pointer issues
    }
};

std::unordered_map <TOKEN_SET , NODE_SET> REL_SET = {
    {TOKEN_EQUALS , NODE_CONDITION_EQUALS},
    {TOKEN_NOT_EQUALS , NODE_CONDITION_NOT_EQUALS},
    {TOKEN_LESS_THAN , NODE_CONDITION_LESS_THAN},
    {TOKEN_LESS_THAN_EQUALS , NODE_CONDITION_LESS_THAN_EQUALS},
    {TOKEN_GREATER_THAN , NODE_CONDITION_GREATER_THAN},
    {TOKEN_GREATER_THAN_EQUALS , NODE_CONDITION_GREATER_THAN_EQUALS},
};

std::unordered_map <TOKEN_SET , std::string> LOG_SET = {
    {TOKEN_AND , "&&"}, 
    {TOKEN_OR , "||"}, 
};


std::string nodeTypeToString(NODE_SET REQUIRED_NODE)
{
    switch(REQUIRED_NODE)
    {
       case NODE_NEW                            : return "NODE_NEW";
       case NODE_ADD                            : return "NODE_ADD";
       case NODE_PRINT                          : return "NODE_PRINT";
       case NODE_UPDATE                         : return "NODE_UPDATE";
       case NODE_REMOVE                         : return "NODE_REMOVE";
       case NODE_EXIT                           : return "NODE_EXIT";
       case NODE_NOT                            : return "NODE_NOT";
       case NODE_AND                            : return "NODE_AND";
       case NODE_OR                             : return "NODE_OR";
       case NODE_CONDITION                      : return "NODE_CONDITION";
       case NODE_CONDITION_EQUALS               : return "NODE_CONDITION_EQUALS";
       case NODE_CONDITION_NOT_EQUALS           : return "NODE_CONDITION_NOT_EQUALS";
       case NODE_CONDITION_GREATER_THAN         : return "NODE_CONDITION_GREATER_THAN";
       case NODE_CONDITION_LESS_THAN            : return "NODE_CONDITION_LESS_THAN";
       case NODE_CONDITION_LESS_THAN_EQUALS     : return "NODE_CONDITION_LESS_THAN_EQUALS";
       case NODE_CONDITION_GREATER_THAN_EQUALS  : return "NODE_CONDITION_GREATER_THAN_EQUALS";
       case NODE_SUB_VALUES                     : return "NODE_SUB_VALUES";
       case NODE_STRING                         : return "NODE_STRING";
       case NODE_INT                            : return "NODE_INT";
       case NODE_FLOAT                          : return "NODE_FLOAT";
    }
    return "[!] UNINDENTIFIED NODE : " + REQUIRED_NODE;
}

struct TOKEN
{
    TOKEN_SET TOKEN_TYPE;
    std::string VALUE;
    int position;
};

std::string tokenTypeToString(TOKEN_SET REQUIRED_TOKEN)
{
    switch (REQUIRED_TOKEN)
    {
       case TOKEN_INT                  : return "TOKEN_INT";
       case TOKEN_FLOAT                : return "TOKEN_FLOAT";
       case TOKEN_STRING               : return "TOKEN_STRING";
       case TOKEN_CHAR                 : return "TOKEN_CHAR";
       case TOKEN_BOOL                 : return "TOKEN_BOOL";
       case TOKEN_DATE                 : return "TOKEN_DATE";
       case TOKEN_TIME                 : return "TOKEN_TIME";
       case TOKEN_NEW                  : return "TOKEN_NEW";
       case TOKEN_DOUBLE_COLON         : return "TOKEN_DOUBLE_COLON";
       case TOKEN_DOT                  : return "TOKEN_DOT";
       case TOKEN_COMMA                : return "TOKEN_COMMA";
       case TOKEN_ADD                  : return "TOKEN_ADD";
       case TOKEN_LEFT_SQR_BRACKET     : return "TOKEN_LEFT_SQR_BRACKET";
       case TOKEN_RIGHT_SQR_BRACKET    : return "TOKEN_RIGHT_SQR_BRACKET";
       case TOKEN_LEFT_PAREN           : return "TOKEN_LEFT_PAREN";
       case TOKEN_RIGHT_PAREN          : return "TOKEN_RIGHT_PAREN";
       case TOKEN_STRING_DATA          : return "TOKEN_STRING_DATA";
       case TOKEN_INT_DATA             : return "TOKEN_INT_DATA";
       case TOKEN_FLOAT_DATA           : return "TOKEN_FLOAT_DATA";
       case TOKEN_TRUE                 : return "TOKEN_TRUE";
       case TOKEN_FALSE                : return "TOKEN_FALSE";
       case TOKEN_PRINT                : return "TOKEN_PRINT";
       case TOKEN_REMOVE               : return "TOKEN_REMOVE";
       case TOKEN_UPDATE               : return "TOKEN_UPDATE";
       case TOKEN_NOT                  : return "TOKEN_NOT";
       case TOKEN_OR                   : return "TOKEN_OR";
       case TOKEN_AND                  : return "TOKEN_AND";
       case TOKEN_ARROW                : return "TOKEN_ARROW";
       case TOKEN_EQUAL_TO             : return "TOKEN_EQUAL_TO";
       case TOKEN_EQUALS               : return "TOKEN_EQUALS";
       case TOKEN_NOT_EQUALS           : return "TOKEN_NOT_EQUALS";
       case TOKEN_LESS_THAN            : return "TOKEN_LESS_THAN";
       case TOKEN_GREATER_THAN         : return "TOKEN_GREATER_THAN";
       case TOKEN_LESS_THAN_EQUALS     : return "TOKEN_LESS_THAN_EQUALS";
       case TOKEN_GREATER_THAN_EQUALS  : return "TOKEN_GREATER_THAN_EQUALS";
       case TOKEN_ID                   : return "TOKEN_ID";
       case TOKEN_EXIT                 : return "TOKEN_EXIT";
       case TOKEN_END_OF_INPUT         : return "TOKEN_END_OF_INPUT";    
    }
    return "[!] ERROR : UNIDENTIFIED TOKEN : " + REQUIRED_TOKEN;
}

std::unordered_map <std::string , TOKEN_SET> KEYWORD_MAP =  {
    {"int" , TOKEN_INT},
    {"float" , TOKEN_FLOAT},
    {"string" , TOKEN_STRING},
    {"char" , TOKEN_CHAR},
    {"bool" , TOKEN_BOOL},
    {"date" , TOKEN_DATE},
    {"time" , TOKEN_TIME},
    {"new"  , TOKEN_NEW},
    {"add" , TOKEN_ADD},
    {"print" , TOKEN_PRINT},
    {"remove" , TOKEN_REMOVE},
    {"update" , TOKEN_UPDATE},
    {"exit" , TOKEN_EXIT},
    {"true" , TOKEN_TRUE},
    {"false" , TOKEN_FALSE}
};

class Lexer
{
    private : 
   
    int cursor;
    int length;
    char current;
    std::string LocalInputBuffer;
    std::vector <TOKEN *> TOKEN_LIST;
    bool stringParsingError;
    
    char advance()
    {
        if (cursor == length - 1) //this means that we are at the end of the input buffer
        {
            current = '\0';
            return current;
        } 
        else 
        {
            current = LocalInputBuffer[++cursor];
            return current;
        }
    }

    void skipWhiteSpaces() // deal with trailing 
    {
        while (current == ' ' && current != '\0')
            advance();
    }

    TOKEN * tokenizeSTRING()
    {
        advance(); // advancing the opening quotes
        TOKEN * newToken = new TOKEN;
        newToken->position = cursor;
        std::string temporaryBuffer = "";
        while (current != '"')
        {
            if (current == '\0')
            {
                stringParsingError = true;
                break;
            }
            temporaryBuffer.push_back(current);
            advance();
        }
        advance(); // advancing the closing quotes

        newToken->TOKEN_TYPE = TOKEN_STRING_DATA;
        newToken->VALUE = temporaryBuffer;
        return newToken;
    }

    TOKEN * tokenizeID()
    {
        TOKEN * newToken = new TOKEN;
        newToken->position = cursor;
        std::string temporaryBuffer = "";
        
        temporaryBuffer.push_back(current);
        advance();

        while (isalnum(current) || current == '_')
        {
            temporaryBuffer.push_back(current);
            advance();
        }

        newToken->TOKEN_TYPE = TOKEN_ID;
        newToken->VALUE = temporaryBuffer;
        if (KEYWORD_MAP.find(newToken->VALUE) != KEYWORD_MAP.end())
            newToken->TOKEN_TYPE = KEYWORD_MAP[newToken->VALUE];

        return newToken;
    }

    TOKEN * tokenizeNUMBER()
    {
        TOKEN * newToken = new TOKEN;
        newToken->position = cursor;
        std::string temporaryBuffer = "";
        bool decimal = false;
        while (isdigit(current) || current == '.')
        {
            if (current == '.')
                decimal = true;
            temporaryBuffer.push_back(current);
            advance();
        }
   
        // 9. -> float WE NEED TO HANDLE THIS 

        newToken->TOKEN_TYPE = decimal ? TOKEN_FLOAT_DATA : TOKEN_INT_DATA;
        newToken->VALUE = temporaryBuffer;

        return newToken;
    }

    void displayAllTokens()
    {
        int counter = 0 ;
        for (TOKEN * CURRENT_TOKEN : TOKEN_LIST)
        {
            std::cout << ++counter << ") " <<  CURRENT_TOKEN->VALUE  << " ";
            std::cout << tokenTypeToString(CURRENT_TOKEN->TOKEN_TYPE) << std::endl; 
        }
    }

    TOKEN * tokenizeSPECIAL (TOKEN_SET NEW_TOKEN_TYPE)
    {
        TOKEN * newToken = new TOKEN;
        newToken->position = cursor;
        newToken->TOKEN_TYPE = NEW_TOKEN_TYPE; 
        newToken->VALUE = current;
        advance();
        return newToken;
    }

    LEXER_STATUS throwLexerError()
    {
        std::cout << FAIL << "[!] LEXER ERROR : UNIDENTIFIED CHARACTER AT INDEX " << cursor << " : " << current << std::endl;
        return LEXER_FAIL;
    }

    LEXER_STATUS throwStringParsingError()
    {
        std::cout << FAIL << "[!] LEXER ERROR : CLOSING QUOTES NOT FOUND IN THE STRING PRESENT IN THE GIVEN COMMAND " << cursor << " : " << current << std::endl;
        return LEXER_FAIL;
    }

    public : 
    Lexer()
    {
    }

    std::vector<TOKEN *> * getTokenStream()
    {
        return &TOKEN_LIST;
    }

    void initialize(std::string InputBuffer)
    {
        cursor = 0;
        length = InputBuffer.size();
        LocalInputBuffer = InputBuffer;
        current = LocalInputBuffer[cursor];
        TOKEN_LIST.clear();
        stringParsingError = false;
    }

    char seek (int offset)
    {
        if (cursor + offset >= length)
            return '\0';
        else 
            return LocalInputBuffer[cursor + offset];
    }
    LEXER_STATUS tokenize()
    {
        while (current)
        {   
            skipWhiteSpaces();
            if (isalpha(current) || current == '_')
                TOKEN_LIST.push_back(tokenizeID());
            else if (isdigit(current))
                TOKEN_LIST.push_back(tokenizeNUMBER());
            else
            {
            switch (current)
            {
                case '(':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LEFT_PAREN));
                    break;
                }
                case ')':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_RIGHT_PAREN));
                    break;
                }
                case ',':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_COMMA));
                    break;
                }
                case '.':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_DOT));
                    break;
                }

                // replace all the advance functions with the seek function 

                case '<':
                {
                    if (seek(1) == '=') // <=
                    {
                        advance();
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LESS_THAN_EQUALS));
                    }
                    else    
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LESS_THAN));
                    break;
                }
                case '>':
                {
                    if (seek(1) == '=') // >=
                    {
                        advance();
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_GREATER_THAN_EQUALS));
                    }
                    else 
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_GREATER_THAN));
                    break;
                }

                case '=':
                {
                    if (seek(1) == '=')
                    {
                        advance();
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_EQUALS));
                    }
                    else
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_EQUAL_TO));
                    break;
                }
                case '"':
                {
                    TOKEN_LIST.push_back(tokenizeSTRING());
                    if (stringParsingError)
                        return throwStringParsingError();
                    break;
                }
                case '[':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LEFT_SQR_BRACKET));
                    break;
                }
                case ']':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_RIGHT_SQR_BRACKET));
                    break;
                }
                case '!':
                {
                    if (seek(1) == '=')
                    {
                        advance();
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_NOT_EQUALS));
                    }
                    else
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_NOT));
                    break;
                }
                case ':':
                {
                    advance();
                    if (current != ':')
                        return throwLexerError();
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_DOUBLE_COLON));
                    break;
                }
                case '|':
                {
                    advance();
                    if (current != '|')
                        return throwLexerError();
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_OR));
                    break;
                }
                case '&':
                {
                    advance();
                    if (current != '&')
                        return throwLexerError();
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_AND));
                    break;
                }
                case '-':
                {
                    advance();
                    if (current != '>')
                        return throwLexerError();
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_ARROW));
                    break;
                }
                case '\0' : break;
                default : 
                {std::cout << "this is from here : " ;return throwLexerError();}
            }
            }
        }
        TOKEN * END_TOKEN = new TOKEN;
        END_TOKEN->TOKEN_TYPE = TOKEN_END_OF_INPUT;
        TOKEN_LIST.push_back(END_TOKEN);
        return LEXER_SUCCESS;
    }

};

class Parser
{
    private :
    TOKEN * CURRENT_TOKEN;
    std::vector <TOKEN *> LOCAL_COPY_TOKEN_STREAM;
    int token_number;
    bool syntaxError;

    PARSER_STATUS throwVerboseSyntaxError(TOKEN_SET REQUIRED_TOKEN)
    {
        std::cout << FAIL << "[!] SYNTAX ERROR : Unexpected Token : " << tokenTypeToString(CURRENT_TOKEN->TOKEN_TYPE) << " , Expected Token : " << tokenTypeToString(REQUIRED_TOKEN) << DEFAULT << std::endl;
        int positionCounter = 0;
        for (char currentCharacter : InputBuffer)
        {
            if (positionCounter >= CURRENT_TOKEN->position && positionCounter <= CURRENT_TOKEN->position + CURRENT_TOKEN->VALUE.length())
                std::cout << FAIL << currentCharacter << DEFAULT ;
            else
                std::cout << currentCharacter;
            positionCounter++;
        }
        exit(0); // change the behaviour of this 
        return PARSER_FAIL;
    }

    PARSER_STATUS throwSyntaxError()
    {
        std::cout << FAIL << "[!] SYNTAX ERROR : UNEXPECTED TOKEN : " << tokenTypeToString(CURRENT_TOKEN->TOKEN_TYPE) << DEFAULT << std::endl;
        int positionCounter = 0;
        for (char currentCharacter : InputBuffer)
        {
            if (positionCounter >= CURRENT_TOKEN->position && positionCounter <= CURRENT_TOKEN->position + CURRENT_TOKEN->VALUE.length())
                std::cout << FAIL << currentCharacter << DEFAULT ;
            else
                std::cout << currentCharacter;
            positionCounter++;
        }
        exit(0); // also change this behaviour
        return PARSER_FAIL;
    }

    void check(TOKEN_SET REQUIRED_CHECK_TOKEN)
    {
        if (CURRENT_TOKEN->TOKEN_TYPE != REQUIRED_CHECK_TOKEN)
            throwSyntaxError();
    }

    TOKEN * proceed (TOKEN_SET REQUIRED_TOKEN)
    {
        if (CURRENT_TOKEN->TOKEN_TYPE != REQUIRED_TOKEN)
        {
            throwVerboseSyntaxError(REQUIRED_TOKEN);            
            syntaxError = true; // this 
            return CURRENT_TOKEN;
        }
        token_number++;
        CURRENT_TOKEN = LOCAL_COPY_TOKEN_STREAM[token_number];
        return CURRENT_TOKEN;
    }

    TOKEN * checkAndProceed(TOKEN_SET REQUIRED_TOKEN)
    {
        TOKEN * bufferPointer = CURRENT_TOKEN;
        proceed(REQUIRED_TOKEN);
        return bufferPointer;
    }
    
    AST_NODE * parseCONDITION()
    {
        AST_NODE * CONDITION_NODE = new AST_NODE;
        CONDITION_NODE->NODE_TYPE = NODE_CONDITION;

        AST_NODE * buffer_pointer;
        while (true)
        {
            std::string * construct = new std::string();
            buffer_pointer = new AST_NODE;
            *construct += checkAndProceed(TOKEN_ID)->VALUE;
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_DOT)
            {
                proceed(TOKEN_DOT);
                *construct += "." + checkAndProceed(TOKEN_ID)->VALUE;
            }
            buffer_pointer->PAYLOAD = construct;

            if (REL_SET.find(CURRENT_TOKEN->TOKEN_TYPE) == REL_SET.end())
                throwSyntaxError();
            buffer_pointer->NODE_TYPE = REL_SET[CURRENT_TOKEN->TOKEN_TYPE];
            proceed(CURRENT_TOKEN->TOKEN_TYPE);
            switch(CURRENT_TOKEN->TOKEN_TYPE)
            {
                case TOKEN_ID :
                {
                    std::string * construct = new std::string();
                    buffer_pointer->HELPER_TOKEN = TOKEN_ID;
                    *construct += checkAndProceed(TOKEN_ID)->VALUE;
                    if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_DOT)
                    {
                        proceed(TOKEN_DOT);
                        *construct += "." + checkAndProceed(TOKEN_ID)->VALUE;
                    }
                    buffer_pointer->SUB_PAYLOAD = construct;
                    break;
                } 
                case TOKEN_INT_DATA : 
                {
                    buffer_pointer->HELPER_TOKEN = TOKEN_INT_DATA;
                    buffer_pointer->SUB_PAYLOAD = &checkAndProceed(TOKEN_INT_DATA)->VALUE;
                    break;
                }
                case TOKEN_FLOAT_DATA : 
                {
                    buffer_pointer->HELPER_TOKEN = TOKEN_FLOAT_DATA;
                    buffer_pointer->SUB_PAYLOAD = &checkAndProceed(TOKEN_FLOAT_DATA)->VALUE;
                    break;
                }
                case TOKEN_STRING_DATA :
                {
                    buffer_pointer->HELPER_TOKEN = TOKEN_STRING_DATA;
                    buffer_pointer->SUB_PAYLOAD = &checkAndProceed(TOKEN_STRING_DATA)->VALUE;
                    break;
                } 
                default : throwSyntaxError();
            }
            CONDITION_NODE->CHILDREN.push_back(buffer_pointer);
            if (LOG_SET.find(CURRENT_TOKEN->TOKEN_TYPE) == LOG_SET.end())
                break;
            else
            {
                CONDITION_NODE->DATA_LIST.push_back(LOG_SET[CURRENT_TOKEN->TOKEN_TYPE]);
                proceed(CURRENT_TOKEN->TOKEN_TYPE);
            }
        }

        return CONDITION_NODE;
    }
    
    PARSER_STATUS parseNEW()
    {
        /* 
        new <table> :: <type> <name> , <type> <name>
        <table> would go in the ->payload
        <columns> would go in the ->children
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_NEW;

        proceed(TOKEN_NEW);
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
        proceed(TOKEN_DOUBLE_COLON);

        AST_NODE * buffer_pointer;
        while (true)
        {
            buffer_pointer = new AST_NODE;
            switch(CURRENT_TOKEN->TOKEN_TYPE)
            {
                case TOKEN_INT : 
                {
                    proceed(TOKEN_INT);
                    buffer_pointer->NODE_TYPE = NODE_INT;
                    buffer_pointer->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
                    break;
                }
                case TOKEN_STRING: 
                {
                    proceed(TOKEN_STRING);
                    buffer_pointer->NODE_TYPE = NODE_STRING;
                    buffer_pointer->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
                    break;
                }
                case TOKEN_FLOAT : 
                {
                    proceed(TOKEN_FLOAT);
                    buffer_pointer->NODE_TYPE = NODE_FLOAT;
                    buffer_pointer->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
                    break;
                }
                default : return throwSyntaxError();
            }
            EVALUATED_NODE->CHILDREN.push_back(buffer_pointer);

        if (CURRENT_TOKEN->TOKEN_TYPE != TOKEN_COMMA)
            break; 
        else
            proceed(TOKEN_COMMA); 
        }
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }

    PARSER_STATUS parseADD()
    {
        /*
        add <table_name> :: <row1> , <row2> , .... , <rowN>
        <rowN> : [<value1> , <value2>]
        */

        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_ADD;
        proceed(TOKEN_ADD);
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
        proceed(TOKEN_DOUBLE_COLON);
        while (true)
        {
            std::vector<AST_NODE *> buffer_vector;
            proceed(TOKEN_LEFT_SQR_BRACKET);
            while (true)
            {
                AST_NODE * buffer_ast_node;
                buffer_ast_node = new AST_NODE;
                switch (CURRENT_TOKEN->TOKEN_TYPE)
                {
                    case TOKEN_INT_DATA:
                    {
                        buffer_ast_node->NODE_TYPE = NODE_INT;
                        buffer_ast_node->PAYLOAD = &checkAndProceed(TOKEN_INT_DATA)->VALUE;
                        break;
                    }
                    case TOKEN_FLOAT_DATA:
                    {
                        buffer_ast_node->NODE_TYPE = NODE_FLOAT;
                        buffer_ast_node->PAYLOAD = &checkAndProceed(TOKEN_FLOAT_DATA)->VALUE;
                        break;
                    }
                    case TOKEN_STRING_DATA:
                    {
                        buffer_ast_node->NODE_TYPE = NODE_STRING;
                        buffer_ast_node->PAYLOAD = &checkAndProceed(TOKEN_STRING_DATA)->VALUE;
                        break;
                    }
                    default : return throwSyntaxError();
                }
                buffer_vector.push_back(buffer_ast_node);
                if (CURRENT_TOKEN->TOKEN_TYPE != TOKEN_COMMA)
                    break;
                else 
                    proceed(TOKEN_COMMA);
            }
            proceed(TOKEN_RIGHT_SQR_BRACKET);
            EVALUATED_NODE->MULTI_DATA.push_back(buffer_vector);
            if (CURRENT_TOKEN->TOKEN_TYPE != TOKEN_COMMA)
                break;
            else    
                proceed(TOKEN_COMMA);
        }
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }

    PARSER_STATUS parsePRINT()
    {
       // print students :: name = "aryan" || id = 9
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_PRINT;
        proceed(TOKEN_PRINT);

        while (true)
        {
            std::string construct = checkAndProceed(TOKEN_ID)->VALUE;
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_DOT)
            {
                proceed(TOKEN_DOT);
                construct += "." + checkAndProceed(TOKEN_ID)->VALUE;
            }
            EVALUATED_NODE->DATA_LIST.push_back(construct);

            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_COMMA)
            {
                proceed(TOKEN_COMMA);
                continue;
            }
            else 
                break;
        }
        switch(CURRENT_TOKEN->TOKEN_TYPE)
        {
            case TOKEN_END_OF_INPUT: return PARSER_SUCCESS;
            case TOKEN_DOUBLE_COLON: 
            {
                proceed(TOKEN_DOUBLE_COLON);
                EVALUATED_NODE->CHILD = parseCONDITION();
                check(TOKEN_END_OF_INPUT);
                return PARSER_SUCCESS;
            }
            default : return throwSyntaxError();
        }
    }
    
    PARSER_STATUS parseREMOVE()
    {
        /*
        remove students 
        remove students :: name = "aryan"
        remove studnets :: students.name = "aryan"
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_REMOVE;
        proceed(TOKEN_REMOVE);

        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;

        switch(CURRENT_TOKEN->TOKEN_TYPE)
        {
            case TOKEN_END_OF_INPUT : return PARSER_SUCCESS;
            case TOKEN_DOUBLE_COLON : 
            {
                proceed(TOKEN_DOUBLE_COLON);
                EVALUATED_NODE->CHILD = parseCONDITION();
                check(TOKEN_END_OF_INPUT);
                return PARSER_SUCCESS;
            }
            default : return throwSyntaxError();
        }
    }
   
    void parseUPDATE_VALUES(AST_NODE *& ROOT_NODE)
    {
        // id = <value>
        // <value> : id | string data | int data | float data
        AST_NODE * buffer_pointer;
        while (true)
        {
            buffer_pointer = new AST_NODE;
            std::string construct = "";
            construct += checkAndProceed(TOKEN_ID)->VALUE;
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_DOT)
            {
                proceed(TOKEN_DOT);
                construct += "." + checkAndProceed(TOKEN_ID)->VALUE;
            }
            buffer_pointer->PAYLOAD = &construct;
            proceed(TOKEN_EQUAL_TO);
            switch(CURRENT_TOKEN->TOKEN_TYPE)
            {
                case TOKEN_ID :
                {
                    buffer_pointer->HELPER_TOKEN = TOKEN_ID;
                    std::string construct;
                    construct += checkAndProceed(TOKEN_ID)->VALUE;
                    if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_DOT)
                    {
                        proceed(TOKEN_DOT);
                        construct += "." + checkAndProceed(TOKEN_ID)->VALUE;
                    }
                    buffer_pointer->SUB_PAYLOAD = &construct;
                    break;
                } 
                case TOKEN_INT_DATA : 
                {
                    buffer_pointer->HELPER_TOKEN = TOKEN_INT_DATA;
                    buffer_pointer->SUB_PAYLOAD = &checkAndProceed(TOKEN_INT_DATA)->VALUE;
                    break;
                }
                case TOKEN_FLOAT_DATA : 
                {
                    buffer_pointer->HELPER_TOKEN = TOKEN_FLOAT_DATA;
                    buffer_pointer->SUB_PAYLOAD = &checkAndProceed(TOKEN_FLOAT_DATA)->VALUE;
                    break;
                }
                case TOKEN_STRING_DATA :
                {
                    buffer_pointer->HELPER_TOKEN = TOKEN_STRING_DATA;
                    buffer_pointer->SUB_PAYLOAD = &checkAndProceed(TOKEN_STRING_DATA)->VALUE;
                    break;
                } 
                default : throwSyntaxError();
            }
            ROOT_NODE->CHILDREN.push_back(buffer_pointer);
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_COMMA)
                proceed(TOKEN_COMMA);
            else
                break;
        }
    }

    PARSER_STATUS parseUPDATE()
    {
        /*
        consider the two statements 
        
        1) update students :: name == "aryan" && id == 7 -> name = "aryan kumar"
        this would update all the records-> name to aryan kumar , where the name 
        is aryan and id = 7

        2) updating all the element in the table : 
        update students -> name = "aryan kumar"
        this would unconditionally update all the elements in the table
        
        update students :: name == "aryan" || id < 5 -> name = "aryan kumar" , id = 8
        update students -> name = "aryan kumar"

        the name of the table would in the ->payload
        the condition would be int the child node
        the update values would in the children vector
        */

        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_UPDATE;
        proceed(TOKEN_UPDATE);
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;

        switch (CURRENT_TOKEN->TOKEN_TYPE)
        {
            case TOKEN_DOUBLE_COLON : 
            {
                proceed(TOKEN_DOUBLE_COLON);
                EVALUATED_NODE->CHILD = parseCONDITION();
                proceed(TOKEN_ARROW);
                parseUPDATE_VALUES(EVALUATED_NODE);
                break;
            }
            case TOKEN_ARROW : 
            {
                proceed(TOKEN_ARROW);
                parseUPDATE_VALUES(EVALUATED_NODE);
                break;
            }
            default : return throwSyntaxError();
        }
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }

    PARSER_STATUS parseEXIT()
    {
        /*
        exit
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_EXIT;
        proceed(TOKEN_EXIT);
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }
    public  :
    Parser() {} 
    AST_NODE * EVALUATED_NODE;

    void initialize(std::vector<TOKEN *> * TOKEN_LIST_ADDRESS)
    {
        LOCAL_COPY_TOKEN_STREAM.clear();
        LOCAL_COPY_TOKEN_STREAM = *(TOKEN_LIST_ADDRESS);
        token_number = 0;
        CURRENT_TOKEN = LOCAL_COPY_TOKEN_STREAM[token_number]; 
        syntaxError = false;
    }

    PARSER_STATUS parse()
    {
            switch (CURRENT_TOKEN->TOKEN_TYPE)
            {
                case TOKEN_NEW    : return parseNEW();
                case TOKEN_ADD    : return parseADD();
                case TOKEN_PRINT  : return parsePRINT();
                case TOKEN_REMOVE : return parseREMOVE();
                case TOKEN_UPDATE : return parseUPDATE();
                case TOKEN_EXIT   : return parseEXIT();                
                default           : return throwSyntaxError();
            }
    }
};

class EvaluationWrapper
{
    private : 
    Lexer * MAIN_LEXER;
    Parser * MAIN_PARSER;
    int commandCount;

    public : 
    EvaluationWrapper()
    {
        MAIN_LEXER = new Lexer();
        MAIN_PARSER = new Parser();
        commandCount = 0;
    }

    AST_NODE * handle (std::string InputBuffer)
    {
        auto startTimer = std::chrono::high_resolution_clock::now();
        // USING THE LEXER TO TOKENIZE THE INPUT BUFFER
        MAIN_LEXER->initialize(InputBuffer);
        LEXER_STATUS CURRENT_LEXER_STATUS =  MAIN_LEXER->tokenize();
        std::vector<TOKEN *>  * temp = MAIN_LEXER->getTokenStream();
        
        // USING THE PARSER TO PARSE THE TOKEN_STREAM
        PARSER_STATUS CURRENT_PARSER_STATUS;
        if (CURRENT_LEXER_STATUS == LEXER_SUCCESS) // could make use of assert here to make things cleaner
            // and write cleaner and more understandable code 
        {
            MAIN_PARSER->initialize(MAIN_LEXER->getTokenStream());
            CURRENT_PARSER_STATUS = MAIN_PARSER->parse();
        }
        auto stopTimer = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds> (stopTimer - startTimer);
        commandCount++;
        if (CURRENT_LEXER_STATUS == LEXER_FAIL || CURRENT_PARSER_STATUS == PARSER_FAIL)
            std::cout << FAIL << "$ Command ID -> " << commandCount << " failed in " << time.count() << "ms\n\n" << DEFAULT;
        else 
            std::cout << SUCCESS << "$ Command ID -> " << commandCount << " executed in " << time.count() << "ms\n\n" << DEFAULT;
        return MAIN_PARSER->EVALUATED_NODE;
    }
};


#endif

