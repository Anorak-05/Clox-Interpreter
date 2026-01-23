#include "value.h"

#include "memory.h"     // for allocating and freeing Memory
#include <stdio.h>      // for printf

void initValueArray(ValueArray *array) {
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void writeValueArray(ValueArray *array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldcapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldcapacity);
        array->values = GROW_ARRAY(Value, array->values, oldcapacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray *array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);   // reset all values to 0
}

void printValue(Value value) {
    switch (value.type)
    {
    case VAL_BOOL:
        printf(AS_BOOL(value) ? "true" : "false");
        break;
    case VAL_NIL:
        printf("nil");
        break;
    case VAL_NUMBER:
        printf("%g", AS_NUMBER(value));
        break;
    default:
        break;
    }
}