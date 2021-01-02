#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ring.h"

/**
 * @brief          create a ring buffer
 * @param[in] size uint16_t indicating size of ring buffer in bytes
 * @return         pointer to ring object
 */
ring_t* ring_create(uint16_t size){
    ring_t *this = (ring_t*)malloc(sizeof(ring_t));
    if(this){
        this->head = 0;
        this->tail = 0;
        this->avail = size;
        this->buf = (uint8_t*)calloc(size, sizeof(uint8_t));
        if(!this->buf)
            goto error;
        this->size = size;
    }
    return this;

error:
    if(this){
        if(this->buf)
            free(this->buf);
        free(this);
    }
    return NULL;
}

/**
 * @brief          destroy ring object
 * @param[in] ring pointer to ring context
 */
void ring_destroy(ring_t *ring){
    if(ring){
        if(ring->buf)
            free(ring->buf);
        free(ring);
    }
}

/**
 * @brief          get the number of bytes stored in the ring buffer
 * @param[in] ring pointer to ring object
 * @return         number of bytes stored in ring buffer as uint16_t
 */
uint16_t ring_count(ring_t *ring){
    return (ring) ? ring->size - ring->avail : 0;
}

/**
 * @brief          increments head index
 * @param[in] ring pointer to ring object
 * @param[in] n    integer number of bytes to increment
 */
static void _ring_increment_head(ring_t *ring, int n){
    ring->head = (ring->head + n) % (ring->size);
    ring->avail += n;
}

/**
 * @brief          increments tail index
 * @param[in] ring pointer to ring object
 * @param[in] n    integer number of bytes to increment
 */
static void _ring_increment_tail(ring_t *ring, int n){
    ring->tail = (ring->tail + n) % (ring->size);
    ring->avail -= n;
}

/**
 * @brief          write data to ring buffer
 * @param[in] ring pointer to ring object
 * @param[in] data pointer to input data
 * @param[in] len  uint16_t number of bytes to write
 * @return
 */
bool ring_write(ring_t *ring, uint8_t *data, uint16_t len){
    if(!ring || !data)
        goto error;

    if(len > ring->avail)
        goto error;

    uint16_t rem = len;
    uint16_t n = 0;
    if((ring->tail + len) > (ring->size - 1)){
        n = ring->size - ring->tail;
        memcpy(&ring->buf[ring->tail], data, n);
        rem -= n;
        _ring_increment_tail(ring, n);
    }
    memcpy(&ring->buf[ring->tail], &data[n], rem);
    _ring_increment_tail(ring, rem);

error:
    return false;
}

/**
 * @brief          read data from ring buffer
 * @param[in] ring pointer to ring object
 * @param[in] data pointer to output buffer
 * @param[in] len  uint16_t number of bytes to read
 * @return
 */
bool ring_read(ring_t *ring, uint8_t *buf, uint16_t len){
    if(!ring || !buf)
        goto error;

    if(len > (ring->size - ring->avail))
        goto error;

    uint16_t rem = len;
    uint16_t n = 0;
    if((ring->head + len) > (ring->size - 1)){
        n = ring->size - ring->head;
        memcpy(buf, &ring->buf[ring->head], n);
        rem -= n;
        _ring_increment_head(ring, n);
    }
    memcpy(&buf[n], &ring->buf[ring->head], rem);
    _ring_increment_head(ring, rem);

error:
    return false;
}

/**
 * @brief          completely erase the ring buffer
 * @param[in] ring pointer to ring object
 */
void ring_erase(ring_t *ring){
    if(!ring)
        goto error;

    memset(ring->buf, 0, ring->size);
    ring->head = 0;
    ring->tail = 0;
    ring->avail = ring->size;

error:
;
}

/**
 * @brief          print out contents of the ring buffer
 * @param[in] ring pointer to ring object
 */
void ring_print(ring_t *ring){
    if(!ring)
        goto error;

    int i;
    for(i = 0; i < ring->size; ++i){
        if(i == ring->head && ring->head == ring->tail){
            if(ring->avail == 0)
                printf("TH ");
            else
                printf("HT ");
        }
        else if(i == ring->head)
            printf("H  ");
        else if(i == ring->tail)
            printf("T  ");
        else
            printf("   ");
    }
    printf("\n");
    for(i = 0; i < ring->size; ++i){
        printf("%02X ", ring->buf[i]);
    }

    printf("\nhead: %d tail: %d avail: %d\n", ring->head, ring->tail, ring->avail);
    printf("\n\n");

error:
;
}
