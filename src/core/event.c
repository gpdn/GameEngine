#include "event.h"

typedef struct callback_entry_t {
    void* listener;
    event_fn callback;
} callback_entry_t;

typedef struct callbacks_array_t {
    callback_entry_t* data;
    size_t size;
    size_t capacity;
} callbacks_array_t;

static callbacks_array_t callbacks_array_init(size_t capacity);
static void callbacks_array_push(callbacks_array_t* array, callback_entry_t value);
static inline callback_entry_t* callbacks_array_get(callbacks_array_t* array, size_t index);
static void callbacks_array_set(callbacks_array_t* array, size_t index, callback_entry_t callback);
static void callbacks_array_reset(callbacks_array_t* array);
static int callbacks_array_find(callbacks_array_t* array, void* listener);
static int callbacks_array_remove_index(callbacks_array_t* array, size_t index);
static void callbacks_array_free(callbacks_array_t* array);

callbacks_array_t callbacks_array_init(size_t capacity) {
    callbacks_array_t arr;
    arr.data = (callback_entry_t*)malloc(sizeof(callback_entry_t) * capacity);
    arr.size = 0;
    arr.capacity = capacity;
    return arr;
}

void callbacks_array_push(callbacks_array_t* arr, callback_entry_t value) {
    if(arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = (callback_entry_t*)realloc(arr->data, sizeof(callback_entry_t) * arr->capacity);
    }
    arr->data[arr->size++] = value;
}

inline callback_entry_t* callbacks_array_get(callbacks_array_t* array, size_t index) {
    if(index > array->size) {
        LOG_ERROR("Array out of bound\n");
        return NULL;
    }
    return array->data + index;
}

void callbacks_array_set(callbacks_array_t* arr, size_t index, callback_entry_t value) {
    if(index > arr->size) {
        LOG_ERROR("Array out of bound\n");
        return;
    }

    arr->data[index] = value;
}

inline void callbacks_array_reset(callbacks_array_t* arr) {
    arr->size = 0;
}

inline int callbacks_array_find(callbacks_array_t* arr, void* listener) {
    for(size_t i = 0; i < arr->size; ++i) {
        if(arr->data[i].listener == listener) return 1;
    }

    return 0;
}

static int callbacks_array_remove_index(callbacks_array_t* arr, size_t index) {
    if(index > arr->size) {
        LOG_ERROR("Array out of bound\n");
        return 0;
    }

    for(size_t i = arr->size - 1; i > index; --i) {
        arr->data[i - 1] = arr->data[i];
    }

    --arr->size;

    return 1;
}

void callbacks_array_free(callbacks_array_t* arr) {
    if(arr->data) free(arr->data);
}

static int initialised = 0; 
static callbacks_array_t events[E_MAX_VALUE];

int event_system_init() {
    if(initialised) return 1;

    initialised = 1;

    for(int i = 0; i < E_MAX_VALUE; ++i) {
        events[i] = callbacks_array_init(5);
    }

    return 1;
}

int event_register(void* listener, event_type_t type, event_fn callback) {
    if(callbacks_array_find(events + type, listener)) return 0;
    callbacks_array_push(events + type, (callback_entry_t){listener, callback});
    return 1;
}

int event_fire(void* sender, event_type_t type, event_data_t data) {
    
    if(events[type].size == 0) {
        //LOG_DEBUG("No callbacks available for event %i\n", type);
        return 1;
    }
    
    for(size_t i = 0; i < events[type].size; ++i) {
        callback_entry_t* entry = events[type].data + i;
        if(entry->callback(type, sender, entry->listener, data)) return 1;
        return 1;
    }

    return 0;
}

int event_unregister(void* listener, event_type_t type, event_fn callback) {
    for(size_t i = 0; i < events[type].size; ++i) {
        callback_entry_t* entry = events[type].data + i;
        if(entry->listener == listener && entry->callback == callback) {
            return callbacks_array_remove_index(events + type, i);
        }
    }

    LOG_DEBUG("Callback not found when unregistering for event type %i\n", type);

    return 0;
}

int event_system_destroy() {
    for(int i = 0; i < E_MAX_VALUE; ++i) {
        callbacks_array_free(events + i);
    }

    return 1;
}
