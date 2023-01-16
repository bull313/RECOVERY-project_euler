#ifndef _STACK_H_
#define _STACK_H_

#define STACK_DEFINE(type) typedef struct { \
    type* data; \
    uint32 size; \
} type ## _stack; \
\
static type ## _stack type ## _stack_init() { \
    type ## _stack stack; \
    \
    stack.data = (type*) calloc(256, sizeof(type)); \
    stack.size = 0; \
    \
    return stack; \
} \
\
static void type ## _stack_push(type ## _stack* stack, type item) { \
    uint32 i; \
    type* new_data; \
    \
    if (stack->size < 256) { \
        *( stack->data + stack->size++ ) = item; \
    } else { \
        new_data = (type*) calloc((stack->size + 1), sizeof(type)); \
        \
        for (i = 0; i < stack->size; ++i) { \
            *( new_data + i ) = *( stack->data + i ); \
        } \
        \
        *( new_data + stack->size++ ) = item; \
        stack->data = new_data; \
    } \
} \
\
static type* type ## _stack_pop(type ## _stack* stack) { \
    type* item; \
    \
    item = NULL; \
    \
    if (stack->size > 0) { \
        item = stack->data + stack->size-- - 1; \
    } \
    \
    return item; \
} \
\
static type* type ## _stack_peek(type ## _stack stack, uint32 idx) { \
    uint32 i; \
    type* item; \
    \
    item = NULL; \
    \
    for (i = 0; i < stack.size; ++i) { \
        if (idx == i) { \
            item = stack.data + i; \
        } \
    } \
    \
    return item; \
} \
\
static void type ## _stack_close(type ## _stack* stack) { \
    free(stack->data); \
    stack->size = 0; \
}

#endif
