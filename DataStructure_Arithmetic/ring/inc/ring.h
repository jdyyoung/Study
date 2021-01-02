#ifndef _RING_H_
#define _RING_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint16_t head;
    uint16_t tail;
    uint16_t avail;
    uint16_t size;
    uint8_t *buf;
}ring_t;

extern ring_t* ring_create(uint16_t size);
extern void ring_destroy(ring_t *ring);
extern uint16_t ring_count(ring_t *ring);
extern bool ring_write(ring_t *ring, uint8_t *data, uint16_t len);
extern bool ring_read(ring_t *ring, uint8_t *buf, uint16_t len);
extern void ring_erase(ring_t *ring);
extern void ring_print(ring_t *ring);

#endif
