#ifndef SGL_TIMER_H
#define SGL_TIMER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SGL_TIMER_COUNT_MAX (-1)

typedef struct sgl_timer sgl_timer_t;


typedef void (*sgl_timer_callback_t)(const sgl_timer_t *timer);


struct sgl_timer {
    struct sgl_timer *prev;
    struct sgl_timer *next;
    sgl_timer_callback_t callback;
    void *user_data;
    uint32_t last_tick;    
    uint16_t interval;
    int16_t count;
};



bool sgl_timer_add(sgl_timer_t *timer, sgl_timer_callback_t callback, uint16_t interval, void *user_data);

bool sgl_timer_remove(sgl_timer_t *timer);

bool sgl_timer_set_count(sgl_timer_t *timer, int16_t count);

void sgl_timer_handler(void);

sgl_timer_t* sgl_timer_create(void);

bool sgl_timer_destroy(sgl_timer_t* timer);

void* sgl_timer_get_user_data(sgl_timer_t *timer);

bool sgl_timer_reset_last_tick(sgl_timer_t *timer);


#ifdef __cplusplus
}
#endif

#endif // SGL_TIMER_H
