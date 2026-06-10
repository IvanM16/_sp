#include "tiny_compiler.h"

// Define the global variable that holds the source code pointer
const char *source_code = NULL;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        printf("Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    // Read the whole file into memory
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *buffer = malloc(length + 1);
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);

    // Set the global pointer for the Lexer
    source_code = buffer;
    
    printf("Intermediate Representation (IR):\n");
    printf("--------------------------------------------\n");

    // Start the pipeline
    advance_token();      // Lexer gets the first token
    compile_program();    // Parser & Codegen build the IR
    execute_vm();         // VM runs the generated IR

    free(buffer);
    return 0;
}