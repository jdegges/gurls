#ifndef _H_QUEUE
#define _H_QUEUE

#include <stdint.h>
#include <pthread.h>

typedef enum gurl_queue_pushtype_t
{
    QUEUE_TAP,      // non blocking push
    QUEUE_PUSH,     // blocking push
    QUEUE_SHOVE,    // non blocking push + dynamic resize
    QUEUE_PSORT,    // push items in sorted order
    QUEUE_SSORT     // shove items in sorted order
} gurl_queue_pushtype_t;

typedef struct gurl_queue_obj_t
{
    uint64_t                    i_pos;
    void*                       obj;
    struct gurl_queue_obj_t*    next;
    struct gurl_queue_obj_t*    last;
    pthread_mutex_t             mutex;
    pthread_cond_t              cond_nonempty;
    pthread_cond_t              cond_nonfull;
} gurl_queue_obj_t;

typedef struct gurl_queue_t
{
    gurl_queue_obj_t*   head;
    gurl_queue_obj_t*   tail;
    uint64_t            count;
    uint64_t            size;
    uint64_t            time;
    pthread_mutex_t     mutex;
    pthread_cond_t      cond_nonempty;
    pthread_cond_t      cond_nonfull;
} gurl_queue_t;

gurl_queue_t* gurl_queue_open( size_t size );
void gurl_queue_close( gurl_queue_t* q );
int gurl_queue_tap( gurl_queue_t* q, void* obj );
void gurl_queue_push( gurl_queue_t* q, void* obj );
int gurl_queue_shove( gurl_queue_t* q, void* obj );
int gurl_queue_push_sorted( gurl_queue_t* q, void* obj );
int gurl_queue_shove_sorted( gurl_queue_t* q, void* obj );
void* gurl_queue_pop( gurl_queue_t* q );
int gurl_queue_is_full( gurl_queue_t* q );
int gurl_queue_is_empty( gurl_queue_t* q );
gurl_queue_obj_t* gurl_create_obj( void* obj );
void gurl_destroy_obj( gurl_queue_obj_t* obj );

#endif
