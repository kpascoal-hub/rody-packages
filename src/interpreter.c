/* interpreter.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"

// Implementação simples de strdup para compatibilidade C99
char* strdup_c99(const char* s) {
    size_t len = strlen(s) + 1;
    char* new_s = (char*)malloc(len);
    if (new_s == NULL) {
        return NULL;
    }
    return (char*)memcpy(new_s, s, len);
}

// Função para inicializar a tabela de símbolos
void init_symbol_table(SymbolTable* table) {
    table->head = NULL;
}

// Função para adicionar um símbolo à tabela
void add_symbol(SymbolTable* table, const char* name, Value value) {
    // Primeiro, verifica se o símbolo já existe para atualizar
    SymbolTableEntry* current = table->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Libera o valor antigo se for string para evitar vazamento de memória
            if (current->value.type == VALUE_STRING) {
                free(current->value.data.string_val);
            }
            current->value = value;
            return;
        }
        current = current->next;
    }

    // Se não existe, adiciona um novo
    SymbolTableEntry* new_entry = (SymbolTableEntry*)malloc(sizeof(SymbolTableEntry));
    if (new_entry == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para SymbolTableEntry.\n");
        exit(1);
    }
    new_entry->name = strdup_c99(name);
    new_entry->value = value;
    new_entry->next = table->head;
    table->head = new_entry;
}

// Função para buscar um símbolo na tabela
Value* get_symbol(SymbolTable* table, const char* name) {
    SymbolTableEntry* current = table->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return &current->value;
        }
        current = current->next;
    }
    return NULL; // Não encontrado
}

// Função para liberar a tabela de símbolos
void free_symbol_table(SymbolTable* table) {
    SymbolTableEntry* current = table->head;
    while (current != NULL) {
        SymbolTableEntry* next = current->next;
        if (current->value.type == VALUE_STRING) {
            free(current->value.data.string_val);
        }
        free(current->name);
        free(current);
        current = next;
    }
    table->head = NULL;
}

// Função para liberar um valor
void free_value(Value value) {
    if (value.type == VALUE_STRING) {
        free(value.data.string_val);
    }
    // Adicionar lógica para liberar listas e dicionários quando implementados
}

// Função principal para interpretar a AST
Value interpret(ASTNode* node, SymbolTable* global_table) {
    Value result;
    result.type = VALUE_NULL; // Valor padrão

    if (node == NULL) {
        return result;
    }

    switch (node->type) {
        case NODE_PROGRAM:
            for (int i = 0; i < node->num_children; i++) {
                interpret(node->children[i], global_table);
            }
            break;
        case NODE_INTEGER:
            result.type = VALUE_INTEGER;
            result.data.int_val = atoi(node->token.lexeme);
            break;
        case NODE_FLOAT:
            result.type = VALUE_FLOAT;
            result.data.float_val = atof(node->token.lexeme);
            break;
        case NODE_STRING:
            result.type = VALUE_STRING;
            result.data.string_val = strdup_c99(node->token.lexeme);
            break;
        case NODE_BINARY_OP: {
            Value left = interpret(node->children[0], global_table);
            Value right = interpret(node->children[1], global_table);

            // Implementação de otimização de expressões (constant folding)
            if (left.type == VALUE_INTEGER && right.type == VALUE_INTEGER) {
                result.type = VALUE_INTEGER;
                switch (node->token.type) {
                    case TOKEN_PLUS: result.data.int_val = left.data.int_val + right.data.int_val; break;
                    case TOKEN_MINUS: result.data.int_val = left.data.int_val - right.data.int_val; break;
                    case TOKEN_MULTIPLY: result.data.int_val = left.data.int_val * right.data.int_val; break;
                    case TOKEN_DIVIDE: 
                        if (right.data.int_val == 0) {
                            fprintf(stderr, "Erro de execução: Divisão por zero.\n");
                            exit(1);
                        }
                        result.data.int_val = left.data.int_val / right.data.int_val; 
                        break;
                    default:
                        fprintf(stderr, "Erro de interpretação: Operador binário inteiro desconhecido: %d\n", node->token.type);
                        exit(1);
                }
            } else if (left.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                result.type = VALUE_FLOAT;
                switch (node->token.type) {
                    case TOKEN_PLUS: result.data.float_val = left.data.float_val + right.data.float_val; break;
                    case TOKEN_MINUS: result.data.float_val = left.data.float_val - right.data.float_val; break;
                    case TOKEN_MULTIPLY: result.data.float_val = left.data.float_val * right.data.float_val; break;
                    case TOKEN_DIVIDE: 
                        if (right.data.float_val == 0.0) {
                            fprintf(stderr, "Erro de execução: Divisão por zero.\n");
                            exit(1);
                        }
                        result.data.float_val = left.data.float_val / right.data.float_val; 
                        break;
                    default:
                        fprintf(stderr, "Erro de interpretação: Operador binário float desconhecido: %d\n", node->token.type);
                        exit(1);
                }
            } else if (left.type == VALUE_INTEGER && right.type == VALUE_FLOAT) {
                result.type = VALUE_FLOAT;
                switch (node->token.type) {
                    case TOKEN_PLUS: result.data.float_val = (float)left.data.int_val + right.data.float_val; break;
                    case TOKEN_MINUS: result.data.float_val = (float)left.data.int_val - right.data.float_val; break;
                    case TOKEN_MULTIPLY: result.data.float_val = (float)left.data.int_val * right.data.float_val; break;
                    case TOKEN_DIVIDE: 
                        if (right.data.float_val == 0.0) {
                            fprintf(stderr, "Erro de execução: Divisão por zero.\n");
                            exit(1);
                        }
                        result.data.float_val = (float)left.data.int_val / right.data.float_val; 
                        break;
                    default:
                        fprintf(stderr, "Erro de interpretação: Operador binário misto desconhecido: %d\n", node->token.type);
                        exit(1);
                }
            } else if (left.type == VALUE_FLOAT && right.type == VALUE_INTEGER) {
                result.type = VALUE_FLOAT;
                switch (node->token.type) {
                    case TOKEN_PLUS: result.data.float_val = left.data.float_val + (float)right.data.int_val; break;
                    case TOKEN_MINUS: result.data.float_val = left.data.float_val - (float)right.data.int_val; break;
                    case TOKEN_MULTIPLY: result.data.float_val = left.data.float_val * (float)right.data.int_val; break;
                    case TOKEN_DIVIDE: 
                        if (right.data.int_val == 0) {
                            fprintf(stderr, "Erro de execução: Divisão por zero.\n");
                            exit(1);
                        }
                        result.data.float_val = left.data.float_val / (float)right.data.int_val; 
                        break;
                    default:
                        fprintf(stderr, "Erro de interpretação: Operador binário misto desconhecido: %d\n", node->token.type);
                        exit(1);
                }
            } else {
                fprintf(stderr, "Erro de interpretação: Operação binária inválida entre tipos.\n");
                exit(1);
            }
            break;
        }
        default:
            fprintf(stderr, "Erro de interpretação: Tipo de nó AST desconhecido: %d\n", node->type);
            exit(1);
    }
    return result;
}



