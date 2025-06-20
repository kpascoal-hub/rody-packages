/* rody.h */

#ifndef RODY_H
#define RODY_H

// Enumeração para os tipos de tokens
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_ARROW_LEFT,
    TOKEN_ARROW_RIGHT,
    TOKEN_ARROW_RIGHT_APPEND,
    TOKEN_GET,
    TOKEN_PRINT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ELSE_IF,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_LOOP,
    TOKEN_WAIT,
    TOKEN_FUN,
    TOKEN_RETURN,
    TOKEN_SYSTEM,
    TOKEN_IMPORT,
    TOKEN_COMMENT,
    TOKEN_MULTI_COMMENT,
    TOKEN_EOF,
    // Tipos de variáveis
    TOKEN_TYPE_INT,
    TOKEN_TYPE_FLOAT,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_DICT,
    TOKEN_TYPE_VECTOR,
    // Palavras-chave de formatação de print
    TOKEN_BR,
    TOKEN_TAB,
    TOKEN_COLOR,
    // Operadores de comparação
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LE,
    TOKEN_GE,
} TokenType;

// Estrutura para representar um token
typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int column;
} Token;

// Enumeração para os tipos de nós da AST
typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGNMENT,
    NODE_PRINT_STMT,
    NODE_GET_STMT,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_FOR_STMT,
    NODE_LOOP_STMT,
    NODE_FUN_DECL,
    NODE_FUN_CALL,
    NODE_RETURN_STMT,
    NODE_SYSTEM_CALL,
    NODE_FILE_READ,
    NODE_FILE_WRITE,
    NODE_FILE_APPEND,
    NODE_IDENTIFIER,
    NODE_INTEGER,
    NODE_FLOAT,
    NODE_STRING,
    NODE_LIST,
    NODE_DICT,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_BLOCK,
    NODE_WAIT,
    NODE_IMPORT,
} NodeType;

// Estrutura para representar um nó da AST
typedef struct ASTNode {
    NodeType type;
    Token token; // Token associado ao nó (ex: identificador, operador)
    struct ASTNode** children; // Array de ponteiros para nós filhos
    int num_children;
    // Outros campos específicos para cada tipo de nó (ex: valor para literais, nome para funções)
} ASTNode;

#endif // RODY_H



