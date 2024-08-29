#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono> // this is for the time thing

#define FAIL      "\e[0;31m"
#define SUCCESS   "\e[0;32m"
#define DEFAULT   "\e[0;37m"
#define DB_PROMPT "blue_db~ :" // this is subject to change

/*

TEMPORARY WORKING SYNTAX DOCUMENTATION : 
CRUD

CREATE :
CREATE NEW DATABASE <DB_NAME>
CREATE NEW TABLE <TABLE_NAME> () // WORK HERE


****** (WORK ON THIS)CREATE NEW TABLE T1 (INTEGER : ID , STRING DATA , BOOL IS_STUDENT)
USE DATABASE

INSERTION : 
INSERT INTO <TABLE> VALUE (<ELEMENT1> , ... , <ELEMENTN>)

READING : 
SEARCH IN <TABLE> VALUE ()

DELETE : 
DELETE FROM <TABLE> VALUE ()

UPDATE : 
UPDATE <TABLE> WHERE (<CONDITION>) WITH 
== < > 

token exit 

*/

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
    TOKEN_INSERT , 
    TOKEN_INTO , 
    TOKEN_VALUE ,
    TOKEN_DELETE , 
    TOKEN_FROM, 
    TOKEN_SEARCH,
    TOKEN_IN,
    TOKEN_CREATE,
    TOKEN_NEW, 
    TOKEN_DATABASE,
    TOKEN_TABLE,
    TOKEN_USE,
    TOKEN_UPDATE , 
    TOKEN_WHERE,
    TOKEN_WITH,
    TOKEN_EQUALS , 
    TOKEN_LESS_THAN , 
    TOKEN_GREATER_THAN,
    TOKEN_STRING , 
    TOKEN_INTEGER, 
    TOKEN_LEFT_PAREN, 
    TOKEN_RIGHT_PAREN,
    TOKEN_COMMA,
    TOKEN_ID , 
    TOKEN_EXIT,
    TOKEN_END_OF_INPUT
} TOKEN_SET;


typedef enum 
{
    NODE_CREATE_DATABASE,
    NODE_CREATE_TABLE,
    NODE_USE,
    NODE_INSERT,
    NODE_SEARCH, 
    NODE_DELETE, 
    NODE_UPDATE,
    NODE_SUB_VALUES,
    NODE_STRING,
    NODE_INTEGER,
    NODE_CONDITION_EQUALS ,
    NODE_CONDITION_GREATER_THAN ,
    NODE_CONDITION_LESS_THAN ,
    NODE_EXIT
} NODE_SET;


struct AST_NODE
{
    NODE_SET NODE_TYPE;
    std::string * PAYLOAD;
    std::string * SUB_PAYLOAD;
    std::vector<AST_NODE*> CHILDREN;
    std::vector<AST_NODE*> UPDATE_CHILDREN;
    
};

std::string nodeTypeToString(NODE_SET REQUIRED_NODE)
{
    switch(REQUIRED_NODE)
    {
        case NODE_CREATE_DATABASE        : return "NODE_CREATE_DATABASE";
        case NODE_CREATE_TABLE           : return "NODE_CREATE_TABLE";
        case NODE_USE                    : return "NODE_USE";
        case NODE_INSERT                 : return "NODE_INSERT";
        case NODE_SEARCH                 : return "NODE_SEARCH";
        case NODE_DELETE                 : return "NODE_DELETE";
        case NODE_UPDATE                 : return "NODE_UPDATE";
        case NODE_SUB_VALUES             : return "NODE_SUB_VALUES";
        case NODE_STRING                 : return "NODE_STRING";
        case NODE_INTEGER                : return "NODE_INTEGER";
        case NODE_CONDITION_EQUALS       : return "NODE_CONDITION_EQUALS";
        case NODE_CONDITION_LESS_THAN    : return "NODE_CONDITION_LESS_THAN";
        case NODE_CONDITION_GREATER_THAN : return "NODE_CONDITION_GREATER_THAN";
        case NODE_EXIT                   : return "NODE_EXIT";
    }
    return "[!] UNINDENTIFIED NODE : " + REQUIRED_NODE;
}

struct TOKEN
{
    TOKEN_SET TOKEN_TYPE;
    std::string VALUE;
};


std::string tokenTypeToString(TOKEN_SET REQUIRED_TOKEN)
{
    switch (REQUIRED_TOKEN)
    {
        case TOKEN_INSERT      : return "TOKEN_INSERT"; 
        case TOKEN_INTO        : return "TOKEN_INTO";
        case TOKEN_VALUE       : return "TOKEN_VALUE"; 
        case TOKEN_DELETE      : return "TOKEN_DELETE"; 
        case TOKEN_FROM        : return "TOKEN_FROM"; 
        case TOKEN_SEARCH      : return "TOKEN_SEARCH"; 
        case TOKEN_IN          : return "TOKEN_IN"; 
        case TOKEN_CREATE      : return "TOKEN_CREATE";
        case TOKEN_NEW         : return "TOKEN_NEW";
        case TOKEN_DATABASE    : return "TOKEN_DATABASE";
        case TOKEN_TABLE       : return "TOKEN_TABLE";
        case TOKEN_USE         : return "TOKEN_USE";
        case TOKEN_UPDATE      : return "TOKEN_UPDATE";
        case TOKEN_WHERE       : return "TOKEN_WHERE";
        case TOKEN_WITH        : return "TOKEN_WITH";
        case TOKEN_EQUALS      : return "TOKEN_EQUALS";
        case TOKEN_LESS_THAN   : return "TOKEN_LESS_THAN";
        case TOKEN_GREATER_THAN: return "TOKEN_GREATER_THAN";
        case TOKEN_STRING      : return "TOKEN_STRING"; 
        case TOKEN_INTEGER     : return "TOKEN_INTEGER"; 
        case TOKEN_LEFT_PAREN  : return "TOKEN_LEFT_PAREN"; 
        case TOKEN_RIGHT_PAREN : return "TOKEN_RIGHT_PAREN"; 
        case TOKEN_COMMA       : return "TOKEN_COMMA";
        case TOKEN_ID          : return "TOKEN_ID";
        case TOKEN_EXIT        : return "TOKEN_EXIT";
        case TOKEN_END_OF_INPUT       : return "TOKEN_END_OF_INPUT";
        
    }
    return "[!] ERROR : UNIDENTIFIED TOKEN : " + REQUIRED_TOKEN;
}

std::unordered_map <std::string , TOKEN_SET> KEYWORD_MAP =  {
    {"insert"    , TOKEN_INSERT},
    {"into"      , TOKEN_INTO},
    {"value"     , TOKEN_VALUE},
    {"delete"    , TOKEN_DELETE},
    {"from"      , TOKEN_FROM},
    {"search"    , TOKEN_SEARCH},
    {"in"        , TOKEN_IN},
    {"create"    , TOKEN_CREATE},
    {"new"       , TOKEN_NEW},
    {"database"  , TOKEN_DATABASE},
    {"table"     , TOKEN_TABLE},
    {"use"       , TOKEN_USE},
    {"update"    , TOKEN_UPDATE},
    {"where"     , TOKEN_WHERE},
    {"with"      , TOKEN_WITH},
    {"exit"      , TOKEN_EXIT},
    {"INSERT"    , TOKEN_INSERT},
    {"INTO"      , TOKEN_INTO},
    {"VALUE"     , TOKEN_VALUE},
    {"DELETE"    , TOKEN_DELETE},
    {"FROM"      , TOKEN_FROM},
    {"SEARCH"    , TOKEN_SEARCH},
    {"IN"        , TOKEN_IN},
    {"CREATE"    , TOKEN_CREATE},
    {"NEW"       , TOKEN_NEW},
    {"DATABASE"  , TOKEN_DATABASE},
    {"TABLE"     , TOKEN_TABLE},
    {"USE"       , TOKEN_USE},
    {"UPDATE"    , TOKEN_UPDATE},
    {"WHERE"     , TOKEN_WHERE},
    {"WITH"      , TOKEN_WITH},
    {"EXIT"      , TOKEN_EXIT},
    
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

        newToken->TOKEN_TYPE = TOKEN_STRING;
        newToken->VALUE = temporaryBuffer;
        return newToken;
    }

    TOKEN * tokenizeID()
    {
        TOKEN * newToken = new TOKEN;
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

    TOKEN * tokenizeINTEGER()
    {
        TOKEN * newToken = new TOKEN;
        std::string temporaryBuffer = "";
        
        while (isdigit(current))
        {
            temporaryBuffer.push_back(current);
            advance();
        }

        newToken->TOKEN_TYPE = TOKEN_INTEGER;
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
        newToken->TOKEN_TYPE = NEW_TOKEN_TYPE;
        if (newToken->TOKEN_TYPE == TOKEN_EQUALS)
            newToken->VALUE = "==";
        else 
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

    LEXER_STATUS tokenize()
    {
        while (current)
        {   
            skipWhiteSpaces();
            if (isalpha(current) || current == '_')
                TOKEN_LIST.push_back(tokenizeID());
            else if (isdigit(current))
                TOKEN_LIST.push_back(tokenizeINTEGER());
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
                case '<':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LESS_THAN));
                    break;
                }
                case '>':
                {
                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_GREATER_THAN));
                    break;
                }
                case '"':
                {
                    TOKEN_LIST.push_back(tokenizeSTRING());
                    if (stringParsingError)
                        return throwStringParsingError();
                    break;
                }
                
                case '=':
                {
                    advance();
                    if (current != '=')
                        return throwLexerError();

                    TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_EQUALS));
                    break;
                }
                case '\0' : break;
                default : 
                {std::cout << "this is from here : " ;return throwLexerError();}
            }
            }
        }

        // displayAllTokens();
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
    AST_NODE * EVALUATED_NODE;

    PARSER_STATUS throwSyntaxError()
    {
        std::cout << FAIL << "[!] SYNTAX ERROR : UNEXPECTED TOKEN : " << tokenTypeToString(CURRENT_TOKEN->TOKEN_TYPE) << DEFAULT << std::endl;
        exit(0); // change the behaviour of this 
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
            throwSyntaxError();            
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
    
    AST_NODE * parseCHILDREN()
    {
        AST_NODE * NEW_CHILD_NODE = new AST_NODE;
        NEW_CHILD_NODE->NODE_TYPE = (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_INTEGER) ? NODE_INTEGER : NODE_STRING;
        NEW_CHILD_NODE->PAYLOAD = &CURRENT_TOKEN->VALUE;

        if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_INTEGER)
            proceed(TOKEN_INTEGER);
        else    
            proceed(TOKEN_STRING);

        return NEW_CHILD_NODE;
    }

    

    AST_NODE * parseCONDITION()
    {
        /*
        THIS IS THE SYNTAX OF PARSING CONDITION 
        ( ID REL_OP INT/STRING )
        WE WILL PUT THE ID IN THE PAYLOAD 
        WE WILL PUT THE INT/STRING IN THE SUBPAYLOAD
        (ENROLLMENT_NUMBER == 56)
        (MARKS < 60)
        (EMPLOYEE_NAME == "ARYAN")
        */
        proceed(TOKEN_LEFT_PAREN);
        AST_NODE * CONDITION_NODE = new AST_NODE;

        CONDITION_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;

        switch(CURRENT_TOKEN->TOKEN_TYPE)
        {
            case TOKEN_LESS_THAN: 
            {
                CONDITION_NODE->NODE_TYPE = NODE_CONDITION_LESS_THAN; 
                proceed(TOKEN_LESS_THAN);
                break;
            }
            case TOKEN_GREATER_THAN: 
            {
                CONDITION_NODE->NODE_TYPE = NODE_CONDITION_GREATER_THAN; 
                proceed(TOKEN_GREATER_THAN);
                break;
            }
            case TOKEN_EQUALS: 
            {
                CONDITION_NODE->NODE_TYPE = NODE_CONDITION_EQUALS; 
                proceed(TOKEN_EQUALS);
                break;
            }
            default : throwSyntaxError();
        }
        
        if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_INTEGER)
            CONDITION_NODE->SUB_PAYLOAD = &checkAndProceed(TOKEN_INTEGER)->VALUE;
        else if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_STRING)
            CONDITION_NODE->SUB_PAYLOAD = &checkAndProceed(TOKEN_STRING)->VALUE;
        else 
            throwSyntaxError();

        proceed(TOKEN_RIGHT_PAREN);
        return CONDITION_NODE;
    }
    
    PARSER_STATUS parseCREATE()
    {
        /* 
        SYNTAX FOR CREATE :
        CREATE NEW DATABASE <DATABASE_NAME>
        CREATE NEW TABLE <TABLE_NAME> 
        */
        
        EVALUATED_NODE = new AST_NODE;
        proceed(TOKEN_CREATE);
        proceed(TOKEN_NEW);

        if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_DATABASE)
        {
            EVALUATED_NODE->NODE_TYPE =  NODE_CREATE_DATABASE;
            proceed(TOKEN_DATABASE);
        }
        else if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_TABLE)
        {
            EVALUATED_NODE->NODE_TYPE = NODE_CREATE_TABLE;
            proceed(TOKEN_TABLE);
        }
        else 
            throwSyntaxError();
        // ID : THIS_IS_AN_ID
        // STRING : "THIS_IS_A_STRING"
        // CREATE NEW TABLE T1
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE; 
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }

    PARSER_STATUS parseUSE()
    {
        /*
        SYNTAX FOR USE : 
        USE <DB_NAME>
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_USE;
        proceed(TOKEN_USE);
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE; 
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }

    PARSER_STATUS parseINSERT()
    {
        /*
        SYNTAX FOR INSERT : 
        INSERT INTO <TABLE_NAME> VALUE (....)
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_INSERT;
        proceed(TOKEN_INSERT);
        proceed(TOKEN_INTO);
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
        proceed(TOKEN_VALUE);

        proceed(TOKEN_LEFT_PAREN);
        while (true)
        {
            // CHECK FOR EMPTY INSERTS MAYBE IN THE GENERATOR?
            // or maybe we could insert null 
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_END_OF_INPUT )
                throwSyntaxError();
            if (CURRENT_TOKEN->TOKEN_TYPE != TOKEN_INTEGER && CURRENT_TOKEN->TOKEN_TYPE != TOKEN_STRING)
                throwSyntaxError();
            EVALUATED_NODE->CHILDREN.push_back(parseCHILDREN());
            
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_RIGHT_PAREN)
            {
                proceed(TOKEN_RIGHT_PAREN);
                break;
            }
            proceed(TOKEN_COMMA);
        }

        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }
    
    PARSER_STATUS parseSEARCH()
    {
        /*
        SYNTAX FOR SEARCH : 
        SEARCH IN <T_NAME> WHERE (CONDITION)
        T_NAME WOULD BE IN THE PAYLOAD 
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_SEARCH;
        proceed(TOKEN_SEARCH);
        proceed(TOKEN_IN);
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
        proceed(TOKEN_WHERE);
        EVALUATED_NODE->CHILDREN.push_back(parseCONDITION());
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }
    
    PARSER_STATUS parseDELETE()
    {
        /*
        SYNTAX FOR DELETE : 
        DELETE FROM <T_NAME> WHERE (CONDITION)
        T_NAME WOULD BE IN THE PAYLOAD 
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_DELETE;
        proceed(TOKEN_DELETE);
        proceed(TOKEN_FROM);
        EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
        proceed(TOKEN_WHERE);
        EVALUATED_NODE->CHILDREN.push_back(parseCONDITION());
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }
   
    PARSER_STATUS parseUPDATE()
    {
        /*
        SYNTAX FOR UPDATE : 
        UPDATE <TNAME> WHERE (<CONDITION>) WITH (V1 , V2 ...)
        TO GET THE CONDITION NODE FROM THE NODE_UPDATE , ACCESS :
            CURRENT_NODE->CHILDREN[0];
        <CONDITION> : (ID REL-OP INT / STRING)
        THE VALUES (V1 , V2 .. VN ) WOULD BE PUSHED INTO THE UPDATE_CHILDREN VECTOR
        */

       EVALUATED_NODE = new AST_NODE;
       EVALUATED_NODE->NODE_TYPE = NODE_UPDATE;
       proceed(TOKEN_UPDATE);
       EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE;
       proceed(TOKEN_WHERE);
       EVALUATED_NODE->CHILDREN.push_back(parseCONDITION());
       proceed(TOKEN_WITH);
       proceed(TOKEN_LEFT_PAREN);
       while (true)
       {
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_END_OF_INPUT )
                throwSyntaxError();
            if (CURRENT_TOKEN->TOKEN_TYPE != TOKEN_INTEGER && CURRENT_TOKEN->TOKEN_TYPE != TOKEN_STRING)
                throwSyntaxError();
            EVALUATED_NODE->UPDATE_CHILDREN.push_back(parseCHILDREN());
            if (CURRENT_TOKEN->TOKEN_TYPE == TOKEN_RIGHT_PAREN)
            {
                proceed(TOKEN_RIGHT_PAREN);
                break;
            }
            proceed(TOKEN_COMMA);
       }
       check(TOKEN_END_OF_INPUT);
       return PARSER_SUCCESS;
    }

    PARSER_STATUS parseEXIT()
    {
        /*
        SYNTAX FOR EXIT : 
        EXIT
        */
        EVALUATED_NODE = new AST_NODE;
        EVALUATED_NODE->NODE_TYPE = NODE_EXIT;
        proceed(TOKEN_EXIT);
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }
    public  :
    Parser() {} 

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
                // FOR EVERY FUNCTION , IF THERE IS A SYNTAX ERROR
                // INSIDE THE FUNCTION , WE FIRST NEED TO CALL THE 
                // THROW SYNTAX ERROR FROM INSIDE THE FUNCTION 
                // THEN WE NEED TO RETURN THE PARSE FAIL ENUM 
                case TOKEN_CREATE : return parseCREATE();
                case TOKEN_USE    : return parseUSE();
                case TOKEN_INSERT : return parseINSERT();
                case TOKEN_SEARCH : return parseSEARCH();
                case TOKEN_DELETE : return parseDELETE();
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

    void handle (std::string InputBuffer)
    {
        auto startTimer = std::chrono::high_resolution_clock::now();
        // USING THE LEXER TO TOKENIZE THE INPUT BUFFER
        MAIN_LEXER->initialize(InputBuffer);
        LEXER_STATUS CURRENT_LEXER_STATUS =  MAIN_LEXER->tokenize();
        // USING THE PARSER TO PARSE THE TOKEN_STREAM
        PARSER_STATUS CURRENT_PARSER_STATUS;
        if (CURRENT_LEXER_STATUS == LEXER_SUCCESS)
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
    }
};

int main()
{
    system("cls"); 
    std::string InputBuffer;  
    EvaluationWrapper * main_io = new EvaluationWrapper();
    while (true)
    {
        std::cout << DEFAULT << DB_PROMPT;
        std::getline(std::cin , InputBuffer);
        main_io->handle(InputBuffer);
    }
    return 0;
}
