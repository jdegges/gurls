#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include <curl/curl.h>
#include <loomlib/thread_pool.h>

#include <gurls.h>

typedef struct gurl_url_t
{
    char        url[1024];
    char        filename[1024];
} gurl_url_t;

void gurl_url_download( func_data data, exec_func *next_func, func_data *next_data )
{
    CURL* curl = curl_easy_init();

    gurl_url_t* u = (gurl_url_t *) data;
    FILE* fp;

    if( u == NULL ) {
        curl_easy_cleanup( curl );
        return;
    }

    fp = fopen( u->filename, "w" );
    if( !fp ) {
        curl_easy_cleanup( curl );
        return;
    }

    /* set up curl and download file */
    curl_easy_setopt( curl, CURLOPT_URL, u->url );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
    curl_easy_perform( curl );
    curl_easy_cleanup( curl );

    fclose( fp );
    free( u );
}

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
    struct thread_pool *pool;
    uint32_t i;
    int32_t rc;
    char url[1024];
    char saftey_ext[5] = "txt\0";

    /* validate threads */
    threads = threads > 30 ? 30 :
              threads == 0 ? 5  :
              threads;

    pool = thread_pool_new( threads );
    if( !pool ) {
        return -1;
    }

    /* initialize curl */
    curl_global_init( CURL_GLOBAL_ALL );

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

        thread_pool_push( pool, gurl_url_download, u );
    }

    /* send term signal to children */
    thread_pool_terminate( pool );

    /* cleanup */
    thread_pool_free( pool );
    curl_global_cleanup();
    return 0;
}
