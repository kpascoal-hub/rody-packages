
/* parser.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Função auxiliar para criar um novo nó AST
static ASTNode* new_ast_node(NodeType type, Token token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para ASTNode.\n");
        exit(1);
    }
    node->type = type;
    node->token = token;
    node->children = NULL;
    node->num_children = 0;
    return node;
}

// Função para adicionar um filho a um nó AST
static void add_child(ASTNode* parent, ASTNode* child) {
    parent->num_children++;
    parent->children = (ASTNode**)realloc(parent->children, parent->num_children * sizeof(ASTNode*));
    if (parent->children == NULL) {
        fprintf(stderr, "Erro: Falha na realocação de memória para filhos da AST.\n");
        exit(1);
    }
    parent->children[parent->num_children - 1] = child;
}

// Função para inicializar o parser
void parser_init(Parser* parser, Lexer* lexer) {
    parser->lexer = lexer;
    // Pega o primeiro token
    parser->current_token = lexer_next_token(lexer);
}

// Função auxiliar para avançar para o próximo token
static void advance_parser(Parser* parser) {
    parser->previous_token = parser->current_token;
    parser->current_token = lexer_next_token(parser->lexer);
}

// Função auxiliar para verificar se o token atual é do tipo esperado
static int check(Parser* parser, TokenType type) {
    return parser->current_token.type == type;
}

// Função auxiliar para consumir um token do tipo esperado, ou reportar erro
static Token consume(Parser* parser, TokenType type, const char* message) {
    if (check(parser, type)) {
        Token token = parser->current_token;
        advance_parser(parser);
        return token;
    } else {
        fprintf(stderr, "Erro de sintaxe na linha %d, coluna %d: %s. Token inesperado: ",
                parser->current_token.line, parser->current_token.column, message);
        if (parser->current_token.lexeme != NULL) {
            fprintf(stderr, "'%s'\n", parser->current_token.lexeme);
        } else {
            fprintf(stderr, "(null)\n");
        }
        exit(1);
    }
}

// Protótipos de funções de parsing
static ASTNode* expression(Parser* parser);
static ASTNode* term(Parser* parser);
static ASTNode* factor(Parser* parser);
static ASTNode* statement(Parser* parser);

// <factor> ::= INTEGER | FLOAT | STRING | IDENTIFIER | "(" <expression> ")"
static ASTNode* factor(Parser* parser) {
    if (check(parser, TOKEN_INTEGER)) {
        return new_ast_node(NODE_INTEGER, consume(parser, TOKEN_INTEGER, "Esperado um inteiro."));
    } else if (check(parser, TOKEN_FLOAT)) {
        return new_ast_node(NODE_FLOAT, consume(parser, TOKEN_FLOAT, "Esperado um float."));
    } else if (check(parser, TOKEN_STRING)) {
        return new_ast_node(NODE_STRING, consume(parser, TOKEN_STRING, "Esperado uma string."));
    } else if (check(parser, TOKEN_IDENTIFIER)) {
        return new_ast_node(NODE_IDENTIFIER, consume(parser, TOKEN_IDENTIFIER, "Esperado um identificador."));
    } else if (check(parser, TOKEN_LPAREN)) {
        consume(parser, TOKEN_LPAREN, "Esperado '('.");
        ASTNode* expr = expression(parser);
        consume(parser, TOKEN_RPAREN, "Esperado ')'.");
        return expr;
    }
    fprintf(stderr, "Erro de sintaxe na linha %d, coluna %d: Esperado um fator.\n",
            parser->current_token.line, parser->current_token.column);
    exit(1);
}

// <term> ::= <factor> (("+" | "-") <factor>)*
static ASTNode* term(Parser* parser) {
    ASTNode* node = factor(parser);

    while (check(parser, TOKEN_PLUS) || check(parser, TOKEN_MINUS)) {
        Token operator_token = parser->current_token;
        advance_parser(parser);
        ASTNode* right = factor(parser);
        ASTNode* binary_op_node = new_ast_node(NODE_BINARY_OP, operator_token);
        add_child(binary_op_node, node);
        add_child(binary_op_node, right);
        node = binary_op_node;
    }
    return node;
}

// <expression> ::= <term> (("+" | "-") <term>)*
static ASTNode* expression(Parser* parser) {
    ASTNode* node = term(parser);

    while (check(parser, TOKEN_MULTIPLY) || check(parser, TOKEN_DIVIDE)) {
        Token operator_token = parser->current_token;
        advance_parser(parser);
        ASTNode* right = term(parser);
        ASTNode* binary_op_node = new_ast_node(NODE_BINARY_OP, operator_token);
        add_child(binary_op_node, node);
        add_child(binary_op_node, right);
        node = binary_op_node;
    }
    return node;
}

// <statement> ::= <expression> ";"
static ASTNode* statement(Parser* parser) {
    ASTNode* expr_node = expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Esperado ';'.");
    return expr_node;
}

// Função para liberar a memória da AST
void free_ast(ASTNode* node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < node->num_children; i++) {
        free_ast(node->children[i]);
    }
    if (node->children != NULL) {
        free(node->children);
    }
    // A memória do lexeme do token é gerenciada pelo lexer, exceto para TOKEN_EOF que é uma string literal
    if (node->token.type != TOKEN_EOF) {
        token_free(&node->token);
    }
    free(node);
}

// <program> ::= <statement>* EOF
ASTNode* parse(Parser* parser) {
    ASTNode* program_node = new_ast_node(NODE_PROGRAM, make_token(TOKEN_EOF, "", 0, 0, 0)); // Token dummy

    while (!check(parser, TOKEN_EOF)) {
        add_child(program_node, statement(parser));
    }

    consume(parser, TOKEN_EOF, "Esperado fim de arquivo.");
    return program_node;
}


