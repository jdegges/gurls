#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "gurls.h"

int main( int argc, char** argv )
{
    int opt;
    uint32_t threads = 5;
    FILE* fin = NULL;

    while( (opt = getopt(argc, argv, "f:j:")) != -1 ) {
        switch( opt ) {
            case 'f':
                fin = fopen( optarg, "r" );
                if( !fin )
                    exit( EXIT_FAILURE );
                break;
            case 'j':
                threads = strtoul( optarg, NULL, 10 );
                break;
            default:
                fprintf( stderr, "Usage: gurls [-f file] [-j threads]\n" );
                exit( EXIT_FAILURE );
        }
    }

    if( !fin )
        fin = stdin;

    return gurls( fin, threads );
}
