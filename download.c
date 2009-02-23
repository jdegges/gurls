#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "common.h"
#include "download.h"
#include "queue.h"
#include "gurls.h"

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

        free( u );
    }
}
