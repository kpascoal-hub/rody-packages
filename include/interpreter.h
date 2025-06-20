/* interpreter.h */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "rody.h"

// Estrutura para representar um valor no interpretador
typedef enum {
    VALUE_INTEGER,
    VALUE_FLOAT,
    VALUE_STRING,
    VALUE_LIST,
    VALUE_DICT,
    VALUE_NULL,
} ValueType;

typedef struct {
    ValueType type;
    union {
        int int_val;
        float float_val;
        char* string_val;
        // Ponteiros para estruturas de lista e dicionário (ainda não implementadas)
        void* list_val;
        void* dict_val;
    } data;
} Value;

// Estrutura para a tabela de símbolos (simplificada por enquanto)
typedef struct SymbolTableEntry {
    char* name;
    Value value;
    struct SymbolTableEntry* next;
} SymbolTableEntry;

typedef struct {
    SymbolTableEntry* head;
} SymbolTable;

// Função para inicializar a tabela de símbolos
void init_symbol_table(SymbolTable* table);

// Função para adicionar um símbolo à tabela
void add_symbol(SymbolTable* table, const char* name, Value value);

// Função para buscar um símbolo na tabela
Value* get_symbol(SymbolTable* table, const char* name);

// Função para liberar a tabela de símbolos
void free_symbol_table(SymbolTable* table);

// Função para interpretar a AST
Value interpret(ASTNode* node, SymbolTable* global_table);

// Função para liberar um valor
void free_value(Value value);

#endif // INTERPRETER_H



