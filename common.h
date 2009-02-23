#ifndef H_COMMON
#define H_COMMON

#include <pthread.h>

static inline void gurl_pthread_error( int rc, char* a, char* b )
{
    if( rc ) {
        fprintf( stderr, "%s: the call to %s returned error code %d\n", a, b, rc );
        pthread_exit( NULL );
    }
}

#endif
