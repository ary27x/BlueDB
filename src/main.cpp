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
int
float 
string 
char 
bool
date 
time
new 
add
print
remove
update
*/


/*
keywords : 
int : 5 : int_data
float  float_data
string  string_data
char  
bool token_true token_false
date 
time



new 
:: 
, 
add 
[ 
]
" "
print
remove
update
||
&&
->

datatypes : 
int , float , string , char , bool , date , time

creating a table : 
-> new students :: int id , string name

adding data to the table : 
-> add students :: [1 , "Aryan"] , [2 , "Joey"]

printing data from the table : 
-> print students 
-> print students :: name = "Aryan" || id = 2
-> print students.name :: name = "Aryan" && id = 5

remove : 
-> remove students 
-> remove students :: name = "Aryan" && id = 1

update : 
update students :: name = "Aryan" || id = 7 -> name = "Aryan Kumar " , id = 5

exit


*/
 


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
    TOKEN_EQUALS , 
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
    NODE_CONDITION_EQUALS ,
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
    std::vector<AST_NODE*> CHILDREN;
    std::vector<AST_NODE*> UPDATE_CHILDREN;
    
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
       case NODE_CONDITION_EQUALS               : return "NODE_CONDITION_EQUALS";
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
       case TOKEN_EQUALS               : return "TOKEN_EQUALS";
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
        // convert the search term to lower case before calling the if 
        // check here to make it case insensitive
        // or maybe dont make it case insensitive
        if (KEYWORD_MAP.find(newToken->VALUE) != KEYWORD_MAP.end())
            newToken->TOKEN_TYPE = KEYWORD_MAP[newToken->VALUE];

        return newToken;
    }

    TOKEN * tokenizeNUMBER()
    {
        TOKEN * newToken = new TOKEN;
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
                case '<':
                {
                    advance();
                    if (current == '=') // <=
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LESS_THAN_EQUALS));
                    else    
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_LESS_THAN));
                    break;
                }
                case '>':
                {
                    advance();
                    if (current == '=') // >=
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_GREATER_THAN_EQUALS));
                    else 
                        TOKEN_LIST.push_back(tokenizeSPECIAL(TOKEN_GREATER_THAN));
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
    
    PARSER_STATUS parseNEW()
    {
        /* 
        new <table> :: <type> <name> , <type> <name>
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
                default : throwSyntaxError();
            }
            EVALUATED_NODE->CHILDREN.push_back(buffer_pointer);

        if (CURRENT_TOKEN->TOKEN_TYPE != TOKEN_COMMA)
            break;  
        }
        check(TOKEN_END_OF_INPUT);
        return PARSER_SUCCESS;
    }


// DEAL WITH THE USE THING
    // PARSER_STATUS parseUSE()
    // {
    //     /*
    //     SYNTAX FOR USE : 
    //     USE <DB_NAME>
    //     */
    //     EVALUATED_NODE = new AST_NODE;
    //     EVALUATED_NODE->NODE_TYPE = NODE_USE;
    //     proceed(TOKEN_USE);
    //     EVALUATED_NODE->PAYLOAD = &checkAndProceed(TOKEN_ID)->VALUE; 
    //     check(TOKEN_END_OF_INPUT);
    //     return PARSER_SUCCESS;
    // }

    PARSER_STATUS parseADD()
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
    
    PARSER_STATUS parsePRINT()
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
    
    PARSER_STATUS parseREMOVE()
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

                // case TOKEN_USE    : return parseUSE();

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

    void handle (std::string InputBuffer)
    {
        auto startTimer = std::chrono::high_resolution_clock::now();
        // USING THE LEXER TO TOKENIZE THE INPUT BUFFER
        MAIN_LEXER->initialize(InputBuffer);
        LEXER_STATUS CURRENT_LEXER_STATUS =  MAIN_LEXER->tokenize();
        std::vector<TOKEN *>  * temp = MAIN_LEXER->getTokenStream();
        int count = 0;
        for ( TOKEN * buffer:  *temp)
        {
            std::cout << ++count  << " " << buffer->VALUE << " " << tokenTypeToString(buffer->TOKEN_TYPE) << endl; 
        }
        return;
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
