#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "download.h"
#include "queue.h"
#include "gurl.h"

gurl_url_t* gurl_create_url( char* url )
{
    gurl_url_t* u = malloc( sizeof(gurl_url_t) );

    if( u ) {
        memset( u, 0, sizeof(gurl_url_t) );
        strncpy( u->url, url, 1024 );
    }

    return u;
}

void gurl_destroy_url( gurl_url_t* u )
{
    free( u );
}

void* gurl_url_download( void* vptr )
{
    CURL* curl;
    gurl_queue_t* q = (gurl_queue_t*) vptr;

    // initialize curl
    curl = curl_easy_init();

    // pop from queue
    // download with curl
    for(;;)
    {
        gurl_url_t* u = gurl_queue_pop( q );
        FILE* fp;

        if( u == NULL ) {
            curl_easy_cleanup( curl );
            pthread_exit( NULL );
        }

        fp = fopen( u->filename, "w" );
        if( !fp ) {
            curl_easy_cleanup( curl );
            pthread_exit( NULL );
        }

        /* set up curl and download file */
        curl_easy_setopt( curl, CURLOPT_URL, u->url );
        curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
        curl_easy_perform( curl );

        gurl_destroy_url( u );
    }
}

void* gurl_download_manager( void* vptr )
{
    gurl_t* g = (gurl_t*) vptr;
    char saftey_ext[5] = "txt\0";
    int j = 5;
    int i, rc;
    pthread_t threads[5];

    // create some download threads
    for( i = 0; i < j; i++ )
    {
        rc = pthread_create( &threads[i], NULL, gurl_url_download, (void*) g->queue );
        gurl_pthread_error( rc, "gurl_download_manager()", "pthread_create()" );
    }

    // read input
    // wrap in gurl_url_t's
    // push to queue
    for( i = 0;; i++)
    {
        gurl_url_t* u;
        char* url;
        char* ext;

        // get next url
        url = readline(NULL);
        if( !url || *url == '\0' ) {
            for( i = 0; i < j; i++ ) {
                gurl_queue_push( g->queue, NULL );
            }
            for( i = 0; i < j; i++ ) {
                rc = pthread_join( threads[i], NULL );
                gurl_pthread_error( rc, "gurl_download_manager()", "pthread_join()" );
            }
            pthread_exit( NULL );
        }

        // create new gurl_url_t object
        u = gurl_create_url( url );
        if( !u ) {
            fprintf( stderr, "Out of memory\n" );
            pthread_exit( NULL );
        }

        // set output filename
        ext = get_extension( url );
        if( !ext )
            ext = saftey_ext;
        snprintf( u->filename, 1024, "file-%010d.%s", i, ext );

        gurl_queue_push( g->queue, (void*) u );
        free( url );
    }
}
