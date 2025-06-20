/* main.c */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_rody>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    // Lê o conteúdo do arquivo para uma string
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* source = (char*)malloc(length + 1);
    if (source == NULL) {
        fprintf(stderr, "Erro: Falha na alocação de memória para o código fonte.\n");
        fclose(file);
        return 1;
    }
    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);

    Lexer lexer;
    lexer_init(&lexer, source);

    Parser parser;
    parser_init(&parser, &lexer);

    ASTNode* program_ast = parse(&parser);

    SymbolTable global_table;
    init_symbol_table(&global_table);

    interpret(program_ast, &global_table);

    // Libera a memória
    free_ast(program_ast);
    free_symbol_table(&global_table);
    free(source);

    printf("Interpretação concluída com sucesso.\n");

    return 0;
}
