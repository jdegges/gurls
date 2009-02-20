#ifndef H_GURL
#define H_GURL

#include "queue.h"

typedef struct gurl_t
{
    uint32_t        i_threads;
    gurl_queue_t*   queue;
} gurl_t;

static inline void gurl_pthread_error( int rc, char* a, char* b )
{
    if( rc ) {
        fprintf( stderr, "%s: the call to %s returned error code %d\n", a, b, rc );
        pthread_exit( NULL );
    }
}

static inline char* get_extension( char* url )
{
    int size;
    char* end = url;

    /* find end of string */
    while( *end++ != '\0' );
    size = end - url;

    /* find extension */
    while( size >= 0 && url[--size] != '.' );
    if( size < 0 )
        return NULL;
    size++;

    return url+size;
}

#endif
