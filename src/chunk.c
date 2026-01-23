#include <stdlib.h>
#include <stdio.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldcapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldcapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldcapacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldcapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int writeConstant(Chunk* chunk, Value value, int line) {
    // check if we need to write OP_CONSTANT or OP_CONSTANT_LONG instruction
    int constantIndex = addConstant(chunk, value);
    if (constantIndex >= 0xFFFFFF) return -1;

    bool useOP_CONSTANT_LONG = constantIndex > 255;

    if (useOP_CONSTANT_LONG) {
        writeChunk(chunk, OP_CONSTANT_LONG, line);  
        writeChunk(chunk, (uint8_t) constantIndex,          line);   // 24 Bytes in Little-Endian
        writeChunk(chunk, (uint8_t)(constantIndex >> 8),    line);
        writeChunk(chunk, (uint8_t)(constantIndex >> 16),   line);
    } else {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, constantIndex, line);
    }
    return constantIndex;
}

void freeChunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(uint8_t, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);   // reset all values to 0
}

int addConstant(Chunk *chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}