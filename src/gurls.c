#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <pthread.h>
#include <string.h>
#include <regex.h>

#include "gurls.h"
#include "common.h"
#include "download.h"
#include "queue.h"

static inline char* get_extension( char* url )
{
    int size;
    char* end = url;

    /* find end of string */
    while( *end != '\n' && *end++ != '\0');
    *end = '\0';
    size = end - url;

    /* find extension */
    while( size >= 0 && url[--size] != '.' );
    if( size < 0 )
        return NULL;
    size++;

    return url+size;
}

static inline int is_url( char* url )
{
    char pattern[92] = "((https?|ftp|gopher|telnet|file|notes|ms-help):((//)|(\\\\\\\\))+[\\w\\d:#@%/;$()~_?\\+-=\\\\\\.&]*)";
    int    status;
    regex_t    re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) {
        return 0;      /* Report error. */
    }
    status = regexec(&re, url, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) {
        return 0;      /* Report error. */
    }
    return 1;
}

int gurls( FILE* fin, uint32_t threads )
{
    gurl_queue_t* queue;
    pthread_t* tid;
    uint32_t i;
    int32_t rc;
    char url[1024];
    char saftey_ext[5] = "txt\0";

    /* validate threads */
    threads = threads > 30 ? 30 :
              threads == 0 ? 5  :
              threads;

    /* create tid's */
    tid = malloc( sizeof(pthread_t)*threads );
    if( !tid ) {
        return -1;
    }

    /* create url queue */
    queue = gurl_queue_open( 20 );
    if( !queue ) {
        return -1;
    }

    /* initialize curl */
    curl_global_init( CURL_GLOBAL_ALL );

    /* spawn threads */
    for( i = 0; i < threads; i++ ) {
        rc = pthread_create( tid+i, NULL, gurl_url_download, (void*) queue );
        gurl_pthread_error( rc, "gurls()", "pthread_create()" );
    }

    /* push input onto queue */
    for( i = 0; fgets(url, 1024, fin) ; i++ )
    {
        gurl_url_t* u;
        char* aux;

        if( *url == '\n' || *url == '\0' ) {
            break;
        }

        if( !is_url(url) ) {
            i--;
            continue;
        }

        // create url object
        u = malloc( sizeof(gurl_url_t) );
        if( !u ) {
            break;
        }

        // remove trailing newline
        for( aux = url; aux-url < 1024 && *aux != '\n' && *aux != '\0'; aux++ );
        *aux = '\0';

        // fill up url object
        strncpy( u->url, url, 1024 );
        aux = get_extension( url );
        if( !aux )
            aux = saftey_ext;
        snprintf( u->filename, 1024, "file-%010d.%s", i, aux );

        // add url to queue
        gurl_queue_push( queue, (void*) u );
    }

    /* send term signal to children */
    for( i = 0; i < threads; i++ ) {
        gurl_queue_push( queue, NULL );
    }

    /* join children */
    for( i = 0; i < threads; i++ ) {
        rc = pthread_join( tid[i], NULL );
        gurl_pthread_error( rc, "gurls()", "pthread_join()" );
    }

    /* cleanup */
    gurl_queue_close( queue );
    curl_global_cleanup();
    return 0;
}
