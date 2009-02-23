#ifndef H_GURL_DOWNLOAD
#define H_GURL_DOWNLOAD

typedef struct gurl_url_t
{
    char        url[1024];
    char        filename[1024];
} gurl_url_t;

void* gurl_url_download( void* vptr );

#endif
