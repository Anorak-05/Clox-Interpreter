#include <stdio.h>  // for printf
#include <stdlib.h> // for exit
#include <stdarg.h> // for va_list (variadic functions)
#include <string.h> // for memcpy

#include "vm.h"
#include "debug.h"    // for dissassembleInstruction
#include "compiler.h" // for compile
#include "object.h"
#include "memory.h"

VM vm; // bad, just done for simplicity's sake

// === LOCAL FUNCTIONS ===

static void resetStack(void)
{
    vm.stackTop = vm.stack;
}

static void runtimeError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

static Value peek(int distance)
{
    return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value)
{
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    ObjString* b = AS_STRING(pop());
    ObjString* a = AS_STRING(pop());

    int length = a->length + b->length;
    char* chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString* result = takeString(chars, length);
    push(OBJ_VAL(result));
}

static InterpretResult run(void)
{
#define READ_BYTE() ({vm.ip++; vm.ip[-1]; }) // ugly Macro for Readability
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
// constant-long has 3 Byte long lookup index -> build up from individual Bytes
#define READ_CONSTANT_LONG() \
    (vm.chunk->constants.values[READ_BYTE() + (READ_BYTE() << 8) + (READ_BYTE() << 16)])

#define BINARY_OP(valueType, op)                        \
    ({                                                  \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) \
        {                                               \
            runtimeError("Operands must be numbers.");  \
            return INTERPRET_RUNTIME_ERROR;             \
        }                                               \
        double b = AS_NUMBER(pop());                    \
        double a = AS_NUMBER(pop());                    \
        push(valueType(a op b));                        \
    })

    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        printf("       ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");

        dissassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
        case OP_ADD:
        {
            if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                concatenate();
            } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                double b = AS_NUMBER(pop());
                double a = AS_NUMBER(pop());
                push(NUMBER_VAL(a + b));
            } else {
                runtimeError("Operands must be either two numbers or two strings.");
            }
            break;
        }
        case OP_SUBTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;
        case OP_NEGATE:
            if (!IS_NUMBER(peek(0)))
            {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            printValue(constant);
            push(constant);
            break;
        }
        case OP_CONSTANT_LONG:
        {
            Value constant = READ_CONSTANT_LONG();
            printValue(constant);
            push(constant);
            break;
        }
        case OP_NIL:
            push(NIL_VAL);
            break;
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_EQUAL:
        {
            Value a = pop();
            Value b = pop();
            push(BOOL_VAL(valuesEqual(a, b)));
            break;
        }
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <); 
            break;
        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        default:
            break;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
#undef BINARY_OP
}

// === PUBLIC FUNCTIONS ===

void initVM(void)
{
    resetStack();
    vm.objects = NULL;
}

void freeVM(void)
{
    freeObjects();
}

void push(Value value)
{
    if (vm.stackTop == &vm.stack[STACK_MAX])
    {
        printf("\n\nSTACK OVERFLOW ERROR\n");
        exit(-1);
    }

    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop(void)
{
    vm.stackTop--;
    return *vm.stackTop;
}

InterpretResult interpret(const char *source)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}