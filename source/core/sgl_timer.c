#include "sgl_timer.h"
#include <string.h>

// #ifdef _WIN32
// #include <windows.h>
// #else
// // include sys time
// #endif

sgl_timer_t *g_timer_list = NULL;

uint32_t sgl_timer_get_tick_port(void) 
{
// todo
    return 0;

}

void* sgl_timer_malloc_port(size_t size)
{
    // todo
    //return malloc(size);
}

void sgl_timer_free_port(void* ptr)
{
    // todo
    //free(ptr);
}

/**
 * @brief Create a new timer
 * @return Pointer to the timer structure, or NULL if allocation failed
 */
sgl_timer_t* sgl_timer_create(void)
{
    sgl_timer_t* timer = sgl_timer_malloc_port(sizeof(sgl_timer_t));
    if (timer == NULL) {
        return NULL;
    }
    memset(timer, 0, sizeof(sgl_timer_t));
    return timer;
}

/**
 * @brief Destroy a timer
 * @param timer Pointer to the timer structure
 * @return true if successful, false if failed
 * @note do not call this function twice!!! after destroy , user must set pointer to NULL
 */
bool sgl_timer_destroy(sgl_timer_t* timer)
{
    if (timer == NULL) {
        return false;
    }

    if (timer->callback != NULL) {
        sgl_timer_remove(timer);
    }

    sgl_timer_free_port(timer);
    return true;
}




/**
 * @brief Add a timer to the timer list
 * @param timer Pointer to the timer structure
 * @param callback Callback function to be called when timer expires
 * @param interval Timer interval in ticks
 * @param user_data User data passed to callback function
 * @return true if successful, false if failed
 * @note Timer will be inserted in ascending order by interval
 */
bool sgl_timer_add(sgl_timer_t *timer, sgl_timer_callback_t callback, uint16_t interval, void *user_data) {
    if (timer == NULL || callback == NULL || interval == 0) {
        return false;
    }
    
    if (timer->callback != NULL) {
        return false;
    }

    timer->prev = NULL;
    timer->next = NULL;
    timer->callback = callback;
    timer->user_data = user_data;
    timer->interval = interval;
    timer->count = -1;

    if (g_timer_list == NULL) {
        g_timer_list = timer;
    } else {
        sgl_timer_t *current = g_timer_list;
        while (current != NULL) {
            if (current->interval >= interval) {
                if (current->prev == NULL) {
                    timer->next = current;
                    current->prev = timer;
                    g_timer_list = timer;
                } else {
                    timer->prev = current->prev;
                    timer->next = current;
                    current->prev->next = timer;
                    current->prev = timer;
                }
                break;
            }
            if (current->next == NULL) {
                current->next = timer;
                timer->prev = current;
                break;
            }
            current = current->next;
        }
    }
    
    timer->last_tick = sgl_timer_get_tick_port();
    
    return true;
}

/**
 * @brief Get user data associated with a timer
 * @param timer Pointer to the timer structure
 * @return Pointer to user data, or NULL if none
 */
void* sgl_timer_get_user_data(sgl_timer_t *timer)
{
    if (timer == NULL) {
        return NULL;
    }
    return timer->user_data;
}

/**
 * @brief Reset the last tick time of a timer to the current time
 * @param timer Pointer to the timer structure
 * @return true if successful, false if failed
 */
bool sgl_timer_reset_last_tick(sgl_timer_t *timer)
{
    if (timer == NULL) {
        return false;
    }
    timer->last_tick = sgl_timer_get_tick_port();
    return true;
}

/**
 * @brief Remove a timer from the timer list
 * @param timer Pointer to the timer structure to be removed
 * @return true if successful, false if failed
 * @note Timer structure will be zeroed after removal
 */
bool sgl_timer_remove(sgl_timer_t *timer) {
    if (timer == NULL) {
        return false;
    }
    
    if (timer->callback == NULL) {
        return false;
    }
    
    if (timer == g_timer_list) {
        g_timer_list = timer->next;
        if (g_timer_list != NULL) {
            g_timer_list->prev = NULL;
        }
    } else {
        if (timer->prev != NULL) {
            timer->prev->next = timer->next;
        }
        if (timer->next != NULL) {
            timer->next->prev = timer->prev;
        }
    }
    
    memset(timer, 0, sizeof(sgl_timer_t));
    
    return true;
}


/**
 * @brief Set the repeat count for a timer
 * @param timer Pointer to the timer structure
 * @param count Number of times to repeat (negative for infinite)
 * @return true if successful, false if failed
 */
bool sgl_timer_set_count(sgl_timer_t *timer, int16_t count) {
    if (timer == NULL || count == 0) {
        return false;
    }
    
    timer->count = count;
    
    return true;
}


/**
 * @brief Timer handler function, should be called periodically
 * @note This function checks all timers and executes callbacks if expired
 * @warning Must be called frequently enough to not miss timer events
 */
void sgl_timer_handler(void) {
    uint32_t current_tick = sgl_timer_get_tick_port();
    sgl_timer_t *current = g_timer_list;
    
    while (current != NULL) {
        sgl_timer_t *next = current->next;
        
        if (current_tick - current->last_tick >= current->interval) {
            if(current->callback) {
                current->callback(current);
            }
            current->last_tick = current_tick;
            
            if (current->count > 0) {
                current->count--;
                if (current->count == 0) {
                    sgl_timer_remove(current);
                }
            }
        }
        
        current = next;
    }
}
