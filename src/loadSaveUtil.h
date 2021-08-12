#ifndef LS_UTIL
#define LS_UTIL

#include <SDL2/SDL.h>
#include <stddef.h>

/**
 * Return 
 * SDL_TRUE: success
 * SDL_FALSE: error
 */
SDL_bool saveResourceToFile( void* resource_ptr, char *filename, size_t resource_size, int num_resources ) {
    const char *write_binary_mode = "wb";
    SDL_RWops *write_context = SDL_RWFromFile( filename , write_binary_mode );
    if( write_context == NULL ) {
        // fprintf(stderr, "%s\n", SDL_GetError() );
        // exit( EXIT_FAILURE );
        return SDL_FALSE;
    }
    SDL_RWwrite( write_context, resource_ptr, resource_size, num_resources );
    SDL_RWclose( write_context );

    return SDL_TRUE;
}

/**
 * Return 
 * SDL_TRUE: success
 * SDL_FALSE: error
 */
SDL_bool loadResourceFromFile( void *resource_ptr, char *filename, size_t resource_size, int num_resources ) {
    const char *read_binary_mode = "rb";

    SDL_RWops *read_context = SDL_RWFromFile( filename, read_binary_mode );
    // File does not exist
    if( read_context == NULL ) {
        // fprintf(stderr, "%s\n", SDL_GetError() );
        // exit( EXIT_FAILURE );
        return SDL_FALSE;
    }

    SDL_RWread( read_context, resource_ptr,  resource_size , num_resources );
    SDL_RWclose( read_context );

    return SDL_TRUE;
}

#endif