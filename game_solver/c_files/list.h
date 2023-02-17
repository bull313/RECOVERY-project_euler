#ifndef _LIST_H_
#define _LIST_H_

#define LIST_DEFINE(type) typedef struct { \
    type* data; \
    uint32 length; \
} list_ ## type; \
\
static list_ ## type list_ ## type ## _init() { \
    list_ ## type list; \
    \
    list.data = (type*) calloc(256, sizeof(type)); \
    list.length = 0; \
    \
    return list; \
} \
\
static void list_ ## type ## _add(list_ ## type* list, type item) { \
    uint32 i; \
    type* new_data; \
    \
    if (list->length < 256) { \
        *( list->data + list->length++ ) = item; \
    } else { \
        new_data = (type*) calloc((list->length + 1), sizeof(type)); \
        \
        for (i = 0; i < list->length; ++i) { \
            *( new_data + i ) = *( list->data + i ); \
        } \
        \
        *( new_data + list->length++ ) = item; \
        list->data = new_data; \
    } \
} \
\
static type* list_ ## type ## _get(list_ ## type list, uint32 idx) { \
    uint32 i; \
    type* item; \
    \
    item = NULL; \
    \
    for (i = 0; i < list.length; ++i) { \
        if (idx == i) { \
            item = list.data + i; \
        } \
    } \
    \
    return item; \
} \
static void list_ ## type ## _set(list_ ## type* list, uint32 i, type item) { \
    if (i >= 0 && i < list->length) { \
        *(list->data + i) = item; \
    } \
} \
\
static void list_ ## type ## _close(list_ ## type* list) { \
    free(list->data); \
    list->length = 0; \
}

#endif
