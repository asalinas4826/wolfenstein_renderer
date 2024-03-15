#include "dyn_array.h"

void freeDynArray(DynArray* arr) {
  free(arr->head);
  arr->head = NULL;
}

void grow(DynArray* arr) {
  void* old_head = arr->head;
  arr->head = malloc(arr->capacity * 2 * arr->elem_size);
  memcpy(arr->head, old_head, arr->size * arr->elem_size);

  free(old_head);
  
  arr->capacity *= 2;
}

void* valAt(DynArray* arr, uint32_t idx) {
  if (idx >= 0 && idx < arr->size) {
    return arr->head + (arr->elem_size * idx);
  }
  return NULL;
}

void pushBack(DynArray* arr, void* value, size_t value_size) {
  if (value_size != arr->elem_size) {
    fprintf(stderr, "Pushing value of wrong size.\n");
    return;
  }
  memcpy(arr->head + arr->elem_size * arr->size, value, value_size);

  arr->size++;
  if (arr->size == arr->capacity) {
    grow(arr);
  }
}

int push(DynArray* arr, void* value, size_t value_size, size_t idx) {
  if (idx > arr->size || idx < 0) {
    fprintf(stderr, "Index out of range.\n");
    return -1;
  }
  if (arr->size == arr->capacity) {
    grow(arr);
  }
  for (size_t i = arr->size; i >= idx + 1; i--) {
    memcpy(arr->head + arr->elem_size * i, arr->head + arr->elem_size * (i - 1), arr->elem_size);
  }
  memcpy(arr->head + arr->elem_size * idx, value, value_size);
  arr->size++;
  return 0;
}

void popBack(DynArray* arr) {
  if (arr->size > 0) {
    arr->size--;
    return;
  }

  fprintf(stderr, "Popping from array of size 0.\n");
}

int pop(DynArray* arr, size_t idx) {
  if (idx >= arr->size || idx < 0) {
    fprintf(stderr, "Index out of range.\n");
    return -1;
  }

  for (size_t i = idx; i < arr->size - 1; i++) {
    memcpy(arr->head + arr->elem_size * i, arr->head + arr->elem_size * (i + 1), arr->elem_size);
  }
  arr->size--;
  return 0;
}

void initializeDynArray(DynArray* arr, void* value, size_t value_size, size_t count) {
  arr->size = count;
  arr->capacity = count * 2 > 0 ? count * 2 : 1;
  arr->elem_size = value_size;
  arr->head = malloc(arr->capacity * arr->elem_size);

  for (int i = 0; i < count; i++) {
    memcpy(arr->head + arr->elem_size * i, value, value_size);
  }
}
