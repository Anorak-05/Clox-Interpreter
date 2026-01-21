#include "vm.h"
#include "debug.h"  // for dissassembleInstruction
#include "compiler.h"   // for compile
#include <stdio.h>  // for printf
#include <stdlib.h> // for exit

VM vm; // bad, just done for simplicity's sake

// === LOCAL FUNCTIONS ===

static void resetStack(void)
{
    vm.stackTop = vm.stack;
}

static InterpretResult run(void)
{
#define READ_BYTE() ({vm.ip++; vm.ip[-1]; }) // ugly Macro for Readability
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
// constant-long has 3 Byte long lookup index -> build up from individual Bytes
#define READ_CONSTANT_LONG() \
    (vm.chunk->constants.values[READ_BYTE() + (READ_BYTE() << 8) + (READ_BYTE() << 16)])

#define BINARY_OP(op) \
    ({double b = pop(); double a = pop(); push(a op b); })

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
            BINARY_OP(+);
            break;
        case OP_SUBTRACT:
            BINARY_OP(-);
            break;
        case OP_MULTIPLY:
            BINARY_OP(*);
            break;
        case OP_DIVIDE:
            BINARY_OP(/);
            break;
        case OP_NEGATE:
            *(vm.stackTop - 1) = -(*(vm.stackTop - 1));
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
}

void freeVM(void)
{
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
    compile(source);
    return INTERPRET_OK;
}