#include "tslib.h"
#include <string.h>
#include <stdlib.h>

/* Forward declarations for instance methods */
static void tsringbuf_instance_destroy(tsringbuf_t **ring);

/* Create a new ring buffer */
tsringbuf_t* tsringbuf_create(size_t capacity, size_t max_element_size)
{
    tsringbuf_t *ring;
    size_t i;
    
    if (capacity == 0 || max_element_size == 0) {
        return NULL;
    }
    
    /* Allocate ring buffer structure */
    ring = (tsringbuf_t *)malloc(sizeof(tsringbuf_t));
    if (!ring) {
        return NULL;
    }
    
    /* Initialize data fields */
    ring->capacity = capacity;
    ring->max_element_size = max_element_size;
    ring->write_idx = 0;
    ring->read_idx = 0;
    ring->count = 0;
    
    /* Initialize method pointers */
    ring->destroy = tsringbuf_instance_destroy;
    ring->init = tsringbuf_init_buffer;
    ring->is_full = tsringbuf_is_full;
    ring->is_empty = tsringbuf_is_empty;
    ring->count_fn = tsringbuf_count;  /* Note: named count_fn to avoid conflict with data member */
    ring->available = tsringbuf_available;
    ring->push = tsringbuf_push;
    ring->pop = tsringbuf_pop;
    ring->peek = tsringbuf_peek;
    ring->clear = tsringbuf_clear;
    
    /* Allocate element array */
    ring->elements = (tsringbuf_element_t *)calloc(capacity, sizeof(tsringbuf_element_t));
    if (!ring->elements) {
        free(ring);
        return NULL;
    }
    
    /* Initialize elements with maximum capacity but no initial allocation */
    for (i = 0; i < capacity; i++) {
        ring->elements[i].data = NULL;  /* Will allocate on first use */
        ring->elements[i].len = 0;
        ring->elements[i].capacity = 0;
        ring->elements[i].valid = false;
    }
    
    return ring;
}

/* Destroy a ring buffer and free all memory */
void tsringbuf_destroy(tsringbuf_t *ring)
{
    size_t i;
    
    if (!ring) {
        return;
    }
    
    /* Free all element data */
    if (ring->elements) {
        for (i = 0; i < ring->capacity; i++) {
            if (ring->elements[i].data) {
                free(ring->elements[i].data);
                ring->elements[i].data = NULL;
                ring->elements[i].capacity = 0;
            }
        }
        free(ring->elements);
    }
    
    /* Free the ring buffer structure */
    free(ring);
}

/* Instance destroy wrapper that takes pointer to pointer */
static void tsringbuf_instance_destroy(tsringbuf_t **ring)
{
    if (ring && *ring) {
        tsringbuf_destroy(*ring);
        *ring = NULL;
    }
}

/* Initialize/reset an existing ring buffer */
void tsringbuf_init_buffer(tsringbuf_t *ring)
{
    size_t i;
    
    if (!ring) {
        return;
    }
    
    ring->write_idx = 0;
    ring->read_idx = 0;
    ring->count = 0;
    
    /* Mark all elements as invalid but keep allocated memory */
    for (i = 0; i < ring->capacity; i++) {
        ring->elements[i].valid = false;
        ring->elements[i].len = 0;
        /* Keep ring->elements[i].data and ring->elements[i].capacity unchanged */
    }
}

/* Check if ring buffer is full */
bool tsringbuf_is_full(const tsringbuf_t *ring)
{
    return ring && (ring->count >= ring->capacity);
}

/* Check if ring buffer is empty */
bool tsringbuf_is_empty(const tsringbuf_t *ring)
{
    return !ring || (ring->count == 0);
}

/* Get number of elements in ring buffer */
size_t tsringbuf_count(const tsringbuf_t *ring)
{
    return ring ? ring->count : 0;
}

/* Get available space in ring buffer */
size_t tsringbuf_available(const tsringbuf_t *ring)
{
    return ring ? (ring->capacity - ring->count) : 0;
}

/* Push data to ring buffer */
bool tsringbuf_push(tsringbuf_t *ring, const uint8_t *data, size_t len)
{
    tsringbuf_element_t *element;
    
    if (!ring || !data || len == 0) {
        return false;
    }
    
    /* Check if buffer is full */
    if (tsringbuf_is_full(ring)) {
        return false;
    }
    
    /* Check if data fits within maximum element size */
    if (len > ring->max_element_size) {
        return false;
    }
    
    /* Get element at write position */
    element = &ring->elements[ring->write_idx];
    
    /* Allocate or reallocate data buffer if needed */
    if (element->data == NULL || element->capacity < len) {
        /* Reallocate to exactly the size needed (or reuse if large enough) */
        uint8_t *new_data = (uint8_t *)realloc(element->data, len);
        if (!new_data) {
            return false;  /* Allocation failed */
        }
        element->data = new_data;
        element->capacity = len;
    }
    
    /* Copy data */
    memcpy(element->data, data, len);
    element->len = len;
    element->valid = true;
    
    /* Update write index and count */
    ring->write_idx = (ring->write_idx + 1) % ring->capacity;
    ring->count++;
    
    return true;
}

/* Pop data from ring buffer */
bool tsringbuf_pop(tsringbuf_t *ring, uint8_t *data, size_t max_len, size_t *actual_len)
{
    tsringbuf_element_t *element;
    size_t copy_len;
    
    if (!ring || !actual_len) {
        return false;
    }
    
    /* Check if buffer is empty */
    if (tsringbuf_is_empty(ring)) {
        *actual_len = 0;
        return false;
    }
    
    /* Get element at read position */
    element = &ring->elements[ring->read_idx];
    
    /* Verify element is valid */
    if (!element->valid) {
        *actual_len = 0;
        return false;
    }
    
    /* Determine how much to copy */
    copy_len = element->len;
    if (data && max_len > 0) {
        if (copy_len > max_len) {
            copy_len = max_len;
        }
        memcpy(data, element->data, copy_len);
    }
    
    *actual_len = element->len;  /* Report actual size */
    
    /* Mark element as invalid */
    element->valid = false;
    element->len = 0;
    
    /* Update read index and count */
    ring->read_idx = (ring->read_idx + 1) % ring->capacity;
    ring->count--;
    
    return true;
}

/* Peek at next element without removing it */
bool tsringbuf_peek(const tsringbuf_t *ring, uint8_t *data, size_t max_len, size_t *actual_len)
{
    const tsringbuf_element_t *element;
    size_t copy_len;
    
    if (!ring || !actual_len) {
        return false;
    }
    
    /* Check if buffer is empty */
    if (tsringbuf_is_empty(ring)) {
        *actual_len = 0;
        return false;
    }
    
    /* Get element at read position */
    element = &ring->elements[ring->read_idx];
    
    /* Verify element is valid */
    if (!element->valid) {
        *actual_len = 0;
        return false;
    }
    
    /* Determine how much to copy */
    copy_len = element->len;
    if (data && max_len > 0) {
        if (copy_len > max_len) {
            copy_len = max_len;
        }
        memcpy(data, element->data, copy_len);
    }
    
    *actual_len = element->len;  /* Report actual size */
    
    return true;
}

/* Clear all elements from ring buffer */
void tsringbuf_clear(tsringbuf_t *ring)
{
    if (!ring) {
        return;
    }
    
    tsringbuf_init_buffer(ring);
}

