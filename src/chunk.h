#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_RETURN,              // jump out of execution block??? Not implemented yet
    OP_CONSTANT,            // save / use constant value -> can address 2^8 constants
    OP_CONSTANT_LONG,       // save / use constant value -> can address 2^24 constants
    OP_NEGATE,              // '-' negate Value
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
} Opcode;

typedef struct {
    size_t count;           // for dynamic array: if count >= capacity on insert, allocate new array with double
    size_t capacity;        // the capacity, free old array and let code point to new array
    uint8_t *code;          // instruction array -> 1 per Byte
    int *lines;             // array with line number per instruction
    ValueArray constants;   // array for all constants defined in this chunk
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);  // append byte to chunk  
int writeConstant(Chunk* chunk, Value value, int line);// append Constant Instruction for given constant value
void freeChunk(Chunk *chunk);

int addConstant(Chunk *chunk, Value value);

#endif