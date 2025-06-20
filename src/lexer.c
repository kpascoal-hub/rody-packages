
/* lexer.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// Função para inicializar o lexer
void lexer_init(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->current_pos = 0;
    lexer->line = 1;
    lexer->column = 1;
}

// Função auxiliar para avançar o caractere atual
static char advance(Lexer* lexer) {
    char c = lexer->source[lexer->current_pos];
    lexer->current_pos++;
    lexer->column++;
    return c;
}

// Função auxiliar para obter o caractere atual sem avançar
static char peek(Lexer* lexer) {
    return lexer->source[lexer->current_pos];
}

// Função auxiliar para obter o próximo caractere sem avançar
static char peek_next(Lexer* lexer) {
    if (lexer->source[lexer->current_pos + 1] == '\0') return '\0';
    return lexer->source[lexer->current_pos + 1];
}

// Função auxiliar para verificar se o fim do arquivo foi atingido
static int is_at_end(Lexer* lexer) {
    return lexer->source[lexer->current_pos] == '\0';
}

// Função auxiliar para criar um token
Token make_token(TokenType type, const char* start, int length, int line, int column) {
    Token token;
    token.type = type;
    token.lexeme = (char*)malloc(length + 1);
    if (token.lexeme == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para lexeme.\n");
        exit(1);
    }
    strncpy(token.lexeme, start, length);
    token.lexeme[length] = '\0';
    token.line = line;
    token.column = column;
    return token;
}

// Função auxiliar para criar um token de erro
static Token error_token(const char* message, int line, int column) {
    Token token;
    token.type = TOKEN_EOF; // Usar EOF para indicar erro, ou criar um TOKEN_ERROR
    token.lexeme = (char*)message; // Não alocar, é uma string literal
    token.line = line;
    token.column = column;
    return token;
}

// Função para liberar a memória de um token
void token_free(Token* token) {
    if (token->lexeme != NULL && token->type != TOKEN_EOF) {
        free(token->lexeme);
        token->lexeme = NULL;
    }
}

// Função para pular espaços em branco e comentários
static void skip_whitespace_and_comments(Lexer* lexer) {
    for (;;) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;
            case '\n':
                lexer->line++;
                lexer->column = 1;
                advance(lexer);
                break;
            case '#':
                if (peek_next(lexer) == '*') { // Comentário multilinhas #*
                    advance(lexer); // Consome #
                    advance(lexer); // Consome *
                    while (!is_at_end(lexer) && !(peek(lexer) == '*' && peek_next(lexer) == '#')) {
                        if (peek(lexer) == '\n') {
                            lexer->line++;
                            lexer->column = 1;
                        }
                        advance(lexer);
                    }
                    if (!is_at_end(lexer)) { // Consome *#
                        advance(lexer);
                        advance(lexer);
                    }
                } else { // Comentário de linha única #
                    while (!is_at_end(lexer) && peek(lexer) != '\n') {
                        advance(lexer);
                    }
                }
                break;
            default:
                return;
        }
    }
}

// Função para identificar identificadores e palavras-chave
static Token identifier(Lexer* lexer) {
    int start_pos = lexer->current_pos - 1;
    // int start_column = lexer->column - 1; // Variável não utilizada
    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }
    int length = lexer->current_pos - start_pos;
    const char* text = lexer->source + start_pos;

    // Verificar palavras-chave
    if (length == 3 && strncmp(text, "get", 3) == 0) return make_token(TOKEN_GET, text, length, lexer->line, lexer->column - length);
    if (length == 5 && strncmp(text, "print", 5) == 0) return make_token(TOKEN_PRINT, text, length, lexer->line, lexer->column - length);
    if (length == 2 && strncmp(text, "if", 2) == 0) return make_token(TOKEN_IF, text, length, lexer->line, lexer->column - length);
    if (length == 4 && strncmp(text, "else", 4) == 0) return make_token(TOKEN_ELSE, text, length, lexer->line, lexer->column - length);
    if (length == 7 && strncmp(text, "else if", 7) == 0) return make_token(TOKEN_ELSE_IF, text, length, lexer->line, lexer->column - length);
    if (length == 5 && strncmp(text, "while", 5) == 0) return make_token(TOKEN_WHILE, text, length, lexer->line, lexer->column - length);
    if (length == 3 && strncmp(text, "for", 3) == 0) return make_token(TOKEN_FOR, text, length, lexer->line, lexer->column - length);
    if (length == 4 && strncmp(text, "loop", 4) == 0) return make_token(TOKEN_LOOP, text, length, lexer->line, lexer->column - length);
    if (length == 4 && strncmp(text, "wait", 4) == 0) return make_token(TOKEN_WAIT, text, length, lexer->line, lexer->column - length);
    if (length == 3 && strncmp(text, "fun", 3) == 0) return make_token(TOKEN_FUN, text, length, lexer->line, lexer->column - length);
    if (length == 6 && strncmp(text, "return", 6) == 0) return make_token(TOKEN_RETURN, text, length, lexer->line, lexer->column - length);
    if (length == 6 && strncmp(text, "system", 6) == 0) return make_token(TOKEN_SYSTEM, text, length, lexer->line, lexer->column - length);
    if (length == 6 && strncmp(text, "import", 6) == 0) return make_token(TOKEN_IMPORT, text, length, lexer->line, lexer->column - length);

    // Tipos de variáveis
    if (length == 3 && strncmp(text, "int", 3) == 0) return make_token(TOKEN_TYPE_INT, text, length, lexer->line, lexer->column - length);
    if (length == 5 && strncmp(text, "float", 5) == 0) return make_token(TOKEN_TYPE_FLOAT, text, length, lexer->line, lexer->column - length);
    if (length == 6 && strncmp(text, "string", 6) == 0) return make_token(TOKEN_TYPE_STRING, text, length, lexer->line, lexer->column - length);
    if (length == 4 && strncmp(text, "dict", 4) == 0) return make_token(TOKEN_TYPE_DICT, text, length, lexer->line, lexer->column - length);
    if (length == 6 && strncmp(text, "vector", 6) == 0) return make_token(TOKEN_TYPE_VECTOR, text, length, lexer->line, lexer->column - length);

    // Palavras-chave de formatação de print
    if (length == 2 && strncmp(text, "br", 2) == 0) return make_token(TOKEN_BR, text, length, lexer->line, lexer->column - length);
    if (length == 3 && strncmp(text, "tab", 3) == 0) return make_token(TOKEN_TAB, text, length, lexer->line, lexer->column - length);
    if (length == 5 && strncmp(text, "color", 5) == 0) return make_token(TOKEN_COLOR, text, length, lexer->line, lexer->column - length);

    return make_token(TOKEN_IDENTIFIER, text, length, lexer->line, lexer->column - length);
}

// Função para identificar números (inteiros e floats)
static Token number(Lexer* lexer) {
    int start_pos = lexer->current_pos - 1;
    while (isdigit(peek(lexer))) {
        advance(lexer);
    }
    if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        advance(lexer);
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
        return make_token(TOKEN_FLOAT, lexer->source + start_pos, lexer->current_pos - start_pos, lexer->line, lexer->column - (lexer->current_pos - start_pos));
    }
    return make_token(TOKEN_INTEGER, lexer->source + start_pos, lexer->current_pos - start_pos, lexer->line, lexer->column - (lexer->current_pos - start_pos));
}

// Função para identificar strings
static Token string(Lexer* lexer) {
    int start_pos = lexer->current_pos;
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') {
            lexer->line++;
            lexer->column = 1;
        }
        advance(lexer);
    }
    if (is_at_end(lexer)) {
        return error_token("String não terminada.", lexer->line, lexer->column);
    }
    advance(lexer); // Consome a aspa final
    return make_token(TOKEN_STRING, lexer->source + start_pos, lexer->current_pos - start_pos - 1, lexer->line, lexer->column - (lexer->current_pos - start_pos));
}

// Função principal para obter o próximo token
Token lexer_next_token(Lexer* lexer) {
    skip_whitespace_and_comments(lexer);

    if (is_at_end(lexer)) {
        return make_token(TOKEN_EOF, "", 0, lexer->line, lexer->column);
    }

    char c = advance(lexer);
    int current_line = lexer->line;
    int current_column = lexer->column - 1; // A coluna já avançou, então subtrai 1

    if (isalpha(c) || c == '_') return identifier(lexer);
    if (isdigit(c)) return number(lexer);

    switch (c) {
        case '(':
            return make_token(TOKEN_LPAREN, "(", 1, current_line, current_column);
        case ')':
            return make_token(TOKEN_RPAREN, ")", 1, current_line, current_column);
        case '{':
            return make_token(TOKEN_LBRACE, "{", 1, current_line, current_column);
        case '}':
            return make_token(TOKEN_RBRACE, "}", 1, current_line, current_column);
        case '[':
            return make_token(TOKEN_LBRACKET, "[", 1, current_line, current_column);
        case ']':
            return make_token(TOKEN_RBRACKET, "]", 1, current_line, current_column);
        case ';':
            return make_token(TOKEN_SEMICOLON, ";", 1, current_line, current_column);
        case ',':
            return make_token(TOKEN_COMMA, ",", 1, current_line, current_column);
        case ':':
            return make_token(TOKEN_COLON, ":", 1, current_line, current_column);
        case '.':
            return make_token(TOKEN_DOT, ".", 1, current_line, current_column);
        case '+':
            return make_token(TOKEN_PLUS, "+", 1, current_line, current_column);
        case '-':
            if (peek(lexer) == '>') {
                advance(lexer);
                if (peek(lexer) == '+') {
                    advance(lexer);
                    return make_token(TOKEN_ARROW_RIGHT_APPEND, "->+", 3, current_line, current_column);
                }
                return make_token(TOKEN_ARROW_RIGHT, "->", 2, current_line, current_column);
            }
            return make_token(TOKEN_MINUS, "-", 1, current_line, current_column);
        case '*':
            return make_token(TOKEN_MULTIPLY, "*", 1, current_line, current_column);
        case '/':
            return make_token(TOKEN_DIVIDE, "/", 1, current_line, current_column);
        case '=':
            return make_token(TOKEN_ASSIGN, "=", 1, current_line, current_column);
        case '<':
            if (peek(lexer) == '-') {
                advance(lexer);
                return make_token(TOKEN_ARROW_LEFT, "<-", 2, current_line, current_column);
            }
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOKEN_LE, "<=", 2, current_line, current_column);
            }
            return make_token(TOKEN_LT, "<", 1, current_line, current_column);
        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOKEN_GE, ">=", 2, current_line, current_column);
            }
            return make_token(TOKEN_GT, ">", 1, current_line, current_column);
        case '!':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOKEN_NEQ, "!=", 2, current_line, current_column);
            }
            break; // Erro se for apenas '!'
        case '"':
            return string(lexer);
    }

    return error_token("Caractere inesperado.", current_line, current_column);
}


