#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "queue.h"

#define ABS_MAX_SIZE 20

gurl_queue_t* gurl_queue_open( size_t size )
{
    gurl_queue_t* q = malloc( sizeof(gurl_queue_t) );
    memset( q, 0, sizeof(gurl_queue_t) );
    q->size = size;
    q->count = 0;
    pthread_mutex_init( &q->mutex, NULL );
    pthread_cond_init( &q->cond_nonempty, NULL );
    pthread_cond_init( &q->cond_nonfull, NULL );
    return q;
}

void gurl_queue_close( gurl_queue_t* q )
{
    int rc = pthread_mutex_lock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_close()", "pthread_mutex_lock()" );
    while( q->count ) {
        rc = pthread_mutex_unlock( &q->mutex );
        gurl_pthread_error( rc, "gurl_queue_close()", "pthread_mutex_unlock()" );
        gurl_queue_pop( q );
    }
    pthread_mutex_unlock( &q->mutex );
    pthread_mutex_destroy( &q->mutex );
    pthread_cond_destroy( &q->cond_nonempty );
    pthread_cond_destroy( &q->cond_nonfull );
    free( q );
}

gurl_queue_obj_t* gurl_queue_create_obj( void* obj )
{
    gurl_queue_obj_t* gobj = malloc( sizeof(gurl_queue_obj_t) );
    if( !gobj )
    {
        fprintf( stderr, "Not enough memory to alloc gobj\n" );
        pthread_exit( NULL );
    }

    memset( gobj, 0, sizeof(gurl_queue_obj_t) );
    gobj->obj = obj;

    pthread_mutex_init( &gobj->mutex, NULL );
    pthread_cond_init( &gobj->cond_nonempty, NULL );
    pthread_cond_init( &gobj->cond_nonfull, NULL );

    return gobj;
}

void gurl_queue_destroy_obj( gurl_queue_obj_t* obj )
{
    pthread_mutex_destroy( &obj->mutex );
    pthread_cond_destroy( &obj->cond_nonempty );
    pthread_cond_destroy( &obj->cond_nonfull );
    free( obj );
}


/* adds image to queue */
int _gurl_queue_push( gurl_queue_t* q, void* obj, gurl_queue_pushtype_t pt )
{
    int rc;
    gurl_queue_obj_t* gobj;
    gurl_queue_obj_t* temp;

    if( (pt == QUEUE_TAP && gurl_queue_is_full(q))
        || (pt == QUEUE_TAP && q->size >= ABS_MAX_SIZE) )
        return 1;

    // get lock on queue
    rc = pthread_mutex_lock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_push()", "pthread_mutex_lock()" );
    while( q->count >= q->size ) {
        if( pt == QUEUE_SHOVE || pt == QUEUE_SSORT ) {
            q->size++;
            break;
        }
        rc = pthread_cond_wait( &q->cond_nonfull, &q->mutex );
        gurl_pthread_error( rc, "gurl_queue_push()", "pthread_cond_wait()" );
    }
    assert( q->count < q->size );

    gobj = gurl_queue_create_obj( obj );
    gobj->i_pos = q->time++;

    // add image to queue
    if( pt == QUEUE_SSORT || pt == QUEUE_PSORT ) {
        temp = q->tail; // iar
        while( temp != NULL ) {
            if( temp->i_pos == gobj->i_pos-1 )
                break;
            temp = temp->next;
        }
        // empty list + add first node
        if( temp == NULL && !q->count ) {
            q->tail =
            q->head = gobj;
        // nonempty list + insert at beginning
        } else if( temp == NULL && q->count ) {
            gobj->next = q->tail;
            q->tail->last = gobj;
            q->tail = gobj;
        // nonempty + insert at end
        } else if( temp == q->head && q->count ) {
            gobj->last = gobj;
            temp->next = gobj;
            q->head = gobj;
        // insert into middle
        } else {
            gobj->next = temp->next;
            gobj->last = temp;
            gobj->next->last = gobj;
            gobj->last->next = gobj;
        }
    } else {
        if( !q->count ) {
            q->tail =
            q->head = gobj;
        } else {
            gobj->next = NULL;
            gobj->last = q->head;
            q->head->next = gobj;
            q->head = gobj;
        }
    }
    q->count++;

    // if someone is waiting to pop, send signal
    rc = pthread_cond_signal( &q->cond_nonempty );
    gurl_pthread_error( rc, "gurl_seq_push()", "pthread_cond_signal()" );

    // unlock queue
    rc = pthread_mutex_unlock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_push()", "pthread_mutex_unlock()" );

    return 0;
}

int gurl_queue_tap( gurl_queue_t* q, void* obj )
{
    return _gurl_queue_push( q, obj, QUEUE_TAP );
}

void gurl_queue_push( gurl_queue_t* q, void* obj )
{
    _gurl_queue_push( q, obj, QUEUE_PUSH );
}

int gurl_queue_shove( gurl_queue_t* q, void* obj )
{
    return _gurl_queue_push( q, obj, QUEUE_SHOVE );
}

int gurl_queue_shove_sorted( gurl_queue_t* q, void* obj )
{
    //iaf->i_refcount = 0;
    return _gurl_queue_push( q, obj, QUEUE_SSORT );
}

int gurl_queue_push_sorted( gurl_queue_t* q, void* obj )
{
    //iaf->i_refcount = 0;
    return _gurl_queue_push( q, obj, QUEUE_PSORT );
}

/* returns unlocked image from queue */
void* gurl_queue_pop( gurl_queue_t* q )
{
    int rc;
    gurl_queue_obj_t* gobj;
    void* obj;

    // get lock on queue
    rc = pthread_mutex_lock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_mutex_lock()" );
    while( q->count == 0 ) {
        rc = pthread_cond_wait( &q->cond_nonempty, &q->mutex );
        gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_cond_wait()" );
    }
    assert( q->count > 0 );

    // pop image off queue
    gobj = q->tail;
    q->tail = q->tail->next;
    obj = gobj->obj;

    q->count--;

    // if someone is waiting to push, send signal
    rc = pthread_cond_signal( &q->cond_nonfull );
    gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_cond_signal()" );

    // unlock queue
    rc = pthread_mutex_unlock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_mutex_unlock()" );

    // clean up queue object
    gurl_queue_destroy_obj( gobj );

    return obj;
}

int gurl_queue_is_full( gurl_queue_t* q )
{
    int rc, status;

    rc = pthread_mutex_lock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_mutex_lock()" );

    status = q->count >= q->size;

    rc = pthread_mutex_unlock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_mutex_unlock()" );

    return status;
}

int gurl_queue_is_empty( gurl_queue_t* q )
{
    int rc, status;

    rc = pthread_mutex_lock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_mutex_lock()" );

    status = q->count == 0;

    rc = pthread_mutex_unlock( &q->mutex );
    gurl_pthread_error( rc, "gurl_queue_pop()", "pthread_mutex_unlock()" );

    return status;
}
