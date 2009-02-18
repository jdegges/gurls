#include <stdio.h>
#include <curl/curl.h>
#include <readline/readline.h>
#include <readline/history.h>

char* get_extension( char* url )
{
    int size;
    char* end = url;

    /* find end of string */
    while( *end++ != '\0' );
    size = end - url;

    /* find extension */
    while( url[--size] != '.' );
    size++;

    return url+size;
}

int main(void)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        int i;
        char bodyfn[1024];
        for(i = 1;; i++) {
            char* url;
            char* ext;
            FILE* bodyfp;

            url = readline(NULL);
            if( !url || *url == '\0' ) {
                curl_easy_cleanup(curl);
                return -1;
            }

            /* open output file */
            ext = get_extension(url);
            snprintf(bodyfn, 1024, "file-%04d.%s", i, ext);
            bodyfp = fopen(bodyfn, "w");
            if(!bodyfp) {
                curl_easy_cleanup(curl);
                return -1;
            }

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, bodyfp);
            res = curl_easy_perform(curl);
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return 0;
}

