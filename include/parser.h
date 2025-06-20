/* parser.h */

#ifndef PARSER_H
#define PARSER_H

#include "rody.h"
#include "lexer.h"

// Estrutura para o parser
typedef struct {
    Lexer* lexer;
    Token current_token;
    Token previous_token;
} Parser;

// Função para inicializar o parser
void parser_init(Parser* parser, Lexer* lexer);

// Função para analisar o programa e construir a AST
ASTNode* parse(Parser* parser);

// Função para liberar a memória da AST
void free_ast(ASTNode* node);

#endif // PARSER_H



