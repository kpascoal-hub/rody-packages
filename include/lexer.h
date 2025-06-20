/* lexer.h */

#ifndef LEXER_H
#define LEXER_H

#include "rody.h"

// Estrutura para o lexer
typedef struct {
    const char* source;
    int current_pos;
    int line;
    int column;
} Lexer;

// Função para inicializar o lexer
void lexer_init(Lexer* lexer, const char* source);

// Função para obter o próximo token
Token lexer_next_token(Lexer* lexer);

// Função para liberar a memória de um token
void token_free(Token* token);

// Função auxiliar para criar um token (declarada aqui para uso em parser.c)
Token make_token(TokenType type, const char* start, int length, int line, int column);

#endif // LEXER_H



