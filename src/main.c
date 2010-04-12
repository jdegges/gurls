#define _XOPEN_SOURCE /* for getopt */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "gurls.h"

int main( int argc, char** argv )
{
    int opt;
    uint32_t threads = 5;
    FILE* fin = NULL;

    while( (opt = getopt(argc, argv, "hf:j:")) != -1 ) {
        switch( opt ) {
            case 'f':
                fin = fopen( optarg, "r" );
                if( !fin )
                    exit( EXIT_FAILURE );
                break;
            case 'j':
                threads = strtoul( optarg, NULL, 10 );
                break;
            case 'h':
                printf( "Syntax: gurls [-f file] [-j threads]\n" );
                printf( "\n" );
                printf( "Options:\n" );
                printf( "   -h                  Display this help menu\n" );
                printf( "   -f <filename>       File to read URLs from [stdin]\n" );
                printf( "   -j <integer>        Number of download threads to spawn [5]\n" );
                printf( "\n" );
                printf( "Example:\n" );
                printf( "   gurls -f my_urls.txt -j 8\n" );
                printf( "\n" );
                return 0;
            default:
                fprintf( stderr, "Usage: gurls [-f file] [-j threads]\n" );
                exit( EXIT_FAILURE );
        }
    }

    if( !fin )
        fin = stdin;

    return gurls( fin, threads );
}
