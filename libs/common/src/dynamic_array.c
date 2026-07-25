#include "dhemitus/dynamic_array.h"
#include "dhemitus/logger.h"
#include "dhemitus/memory.h"
#include "preference.h"

void *_dyanmic_array_create(u64 length, u64 stride){
    u64 header_size = DYNAMIC_ARRAY_FIELD_LENGTH * sizeof(u64);
    u64 array_size = length * stride;
    u64 *new_array = allocate_memory(header_size + array_size, MEMORY_LABEL_DYNAMIC_ARRAY);
    set_memory(new_array, 0, header_size + array_size);

    new_array[DYNAMIC_ARRAY_CAPACITY] = length;
    new_array[DYNAMIC_ARRAY_LENGTH] = 0;
    new_array[DYNAMIC_ARRAY_STRIDE] = stride;

    return (void *)(new_array + DYNAMIC_ARRAY_FIELD_LENGTH);
}

void _dynamic_array_destroy(void *array){
    u64 *header = (u64 *)array - DYNAMIC_ARRAY_FIELD_LENGTH;
    u64 header_size = DYNAMIC_ARRAY_FIELD_LENGTH * sizeof(u64);
    u64 total_size = header_size + header[DYNAMIC_ARRAY_CAPACITY] + header[DYNAMIC_ARRAY_STRIDE];
    free_memory(header, total_size, MEMORY_LABEL_DYNAMIC_ARRAY);
}

u64 _dynamic_field_get(void *array, u64 field){
    u64 *header = (u64 *)array - DYNAMIC_ARRAY_FIELD_LENGTH;
    return  header[field];
}

void _dynamic_field_set(void *array, u64 field, u64 value){
    u64 *header = (u64 *)array - DYNAMIC_ARRAY_FIELD_LENGTH;
    header[field] = value;
}

void *_dynamic_array_resize(void *array){
    u64 length = dynamic_array_length(array);
    u64 stride = dynamic_array_stride(array);
    void *temp = _dyanmic_array_create((DYNAMIC_ARRAY_RESIZE_FACTOR * dynamic_array_capacity(array)), stride);
    copy_memory(temp, array, length);
    _dynamic_field_set(temp, DYNAMIC_ARRAY_LENGTH, length);
    _dynamic_array_destroy(array);
    return temp;
}

void *_dynamic_array_push(void *array, const void *value){
    u64 length = dynamic_array_length(array);
    u64 stride = dynamic_array_stride(array);

    if(length >= dynamic_array_capacity(array)){
        array = _dynamic_array_resize(array);
    }

    u64 addr = (u64)array;
    addr += (length * stride);
    copy_memory((void *)addr, value, stride);
    _dynamic_field_set(array, DYNAMIC_ARRAY_LENGTH, length + 1);
    return array;
}

void _dynamic_array_pop(void *array, void *dest){
    u64 length = dynamic_array_length(array);
    u64 stride = dynamic_array_stride(array);
    u64 addr = (u64)array;
    addr += ((length - 1) * stride);

    copy_memory(dest, (void *)addr, stride);

    _dynamic_field_set(array, DYNAMIC_ARRAY_LENGTH, length - 1);
}

void *_dynamic_array_insert_at(void *array, u64 index, void *value){
    u64 length = dynamic_array_length(array);
    u64 stride = dynamic_array_stride(array);
    if(index >= length){
        LOG_ERROR("index: %i must be smaller than array's length: %i", index, length);
    }
    if(length >= dynamic_array_capacity(array)){
        array = _dynamic_array_resize(array);
    }
    u64 addr = (u64)array;
    if(index != length - 1){
        copy_memory(
            (void *)(addr + ((index + 1) * stride)),
            (void *)(addr + (index * stride)),
            stride * (length - index));
    }

    copy_memory((void *)(addr + (index * stride)), value, stride);

    _dynamic_field_set(array, DYNAMIC_ARRAY_LENGTH, length + 1);
    return array;
}

void *_dynamic_array_pop_at(void *array, u64 index, void *dest){
    u64 length = dynamic_array_length(array);
    u64 stride = dynamic_array_stride(array);
    if(index >= length){
        LOG_ERROR("index: %i must be smaller than array's length: %i", index, length);
    }
    u64 addr = (u64)array;
    copy_memory(dest, (void *)(addr + (index * stride)), stride);
   
    if(index != length - 1){
        copy_memory(
            (void *)(addr + (index * stride)),
            (void *)(addr + ((index + 1) * stride)),
            stride * (length - index));
    }

    _dynamic_field_set(array, DYNAMIC_ARRAY_LENGTH, length - 1);
    return array;
}
