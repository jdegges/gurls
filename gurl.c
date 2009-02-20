#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>

#include "gurl.h"
#include "download.h"

int main(void)
{
    gurl_t* gurl;
    pthread_t tid;
    int rc;

    curl_global_init( CURL_GLOBAL_ALL );

    gurl = malloc( sizeof(gurl_t) );
    memset( gurl, 0, sizeof(gurl_t) );

    gurl->i_threads = 5;
    gurl->queue = gurl_queue_open( 20 );

    rc = pthread_create( &tid, NULL, gurl_download_manager, (void*) gurl );
    gurl_pthread_error( rc, "gurl main!", "pthread_create()" );

    rc = pthread_join( tid, NULL );
    gurl_pthread_error( rc, "gurl main!", "pthread_join()" );

    gurl_queue_close( gurl->queue );
    free( gurl );

    return 0;
}
