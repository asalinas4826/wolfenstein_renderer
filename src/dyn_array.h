#ifndef DYN_ARRAY
#define DYN_ARRAY(arr, val_size) \
  DynArray arr; \
  void* val_ptr = NULL; \
  initializeDynArray(&arr, val_ptr, val_size, 0); \
  
#define DYN_ARRAY_INIT(arr, value, val_size, count) \
  DynArray arr; \
  initializeDynArray(&arr, (void*) value, val_size, count); \

#define PUSH(arr, val, val_size) \
  pushBack(&arr, (void*) &val, val_size) \

#define PUSH_INTO(arr, val, val_size, idx) \
  push(&arr, (void*) &val, val_size, idx) \

#define POP(arr) popBack(&arr)

#define POP_FROM(arr, idx) \
  pop(&arr, idx) \

#define GET(arr, type, idx) \
  *((type*) valAt(&arr, idx)) \

#define GET_PTR(arr, type, idx) \
	((type*) valAt(&arr, idx))

#define FREE(arr) freeDynArray(&arr)

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t size;
  size_t capacity;
  size_t elem_size;
  void* head;
} DynArray;

void freeDynArray(DynArray* arr);
void grow(DynArray* arr);
void* valAt(DynArray* arr, uint32_t idx);
void pushBack(DynArray* arr, void* value, size_t value_size);
int push(DynArray* arr, void* value, size_t value_size, size_t idx);
void popBack(DynArray* arr);
int pop(DynArray* arr, size_t idx);
void initializeDynArray(DynArray* arr, void* value, size_t value_size, size_t count);

#endif
