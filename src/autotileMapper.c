/*****
 * USE THIS TO CREATE TILEMAPS THAT
 * ARE AUTOTILED
 * 
 */


#include <SDL2/SDL.h>
#include "renderUtil.h"

SDL_Renderer *gRenderer = NULL;
SDL_Window *gWindow = NULL;
SDL_bool gIsFullscreen = SDL_TRUE;

SDL_Color gridColor = {255,255,255,50};
SDL_Color black = {0,0,0,255};
SDL_Color yellow = {120,80,0};

int canvasWidth = 1920;
int canvasHeight = 1040;
int viewportWidth = 1920;
int viewportHeight = 1040;




int 
main() {
    const unsigned int numTileMapRows = 32;
    const unsigned int numTileMapCols = 32;
    const unsigned int cellSize = 32;


    Uint32 tilemapCols[numTileMapRows]; // tile mask. Is there a tile here

    const unsigned int numRowsInTileset = 4;
    const unsigned int numColsInTileset = 40;
    Uint8 autoTileBitmasks[numRowsInTileset][numColsInTileset]; // what is the autoTiling bitmask of each tile in the set





    for( int i = 0; i < numTileMapRows; i++ ) {
        tilemapCols[i] = 0b0;
    }
    // 0000000000000000000000...
    // 0000000000000000000000...
    // 0000000000000000000000...
    // .
    // .
    // .
    int tilemapPositionX = 0;
    int tilemapPositionY = 0;

    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if( gIsFullscreen ) {
        windowFlags |= SDL_WINDOW_FULLSCREEN ;
    }
    gWindow = SDL_CreateWindow( "Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, windowFlags);
    if ( gWindow == NULL ) {
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }

    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if (gRenderer == NULL ) {
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }

    if( SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND) < 0 ){
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }
    
    SDL_bool quitGame = SDL_FALSE;
    SDL_Event event;
    SDL_bool lmbPressed = SDL_FALSE;
    SDL_bool rmbPressed = SDL_FALSE;

    while(!quitGame){
        /**
         * EVENTS
         * **/
        while(SDL_PollEvent(&event)){
            if( event.type == SDL_MOUSEBUTTONDOWN ) {
                if( event.button.button == SDL_BUTTON_LEFT ) {
                    lmbPressed = SDL_TRUE;
                }
                else if ( event.button.button == SDL_BUTTON_RIGHT) {
                    rmbPressed = SDL_TRUE;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP ) {
                if( event.button.button == SDL_BUTTON_LEFT ) {
                    lmbPressed = SDL_FALSE;
                }
                else if ( event.button.button == SDL_BUTTON_RIGHT) {
                    rmbPressed = SDL_FALSE;
                }
            }
            if( event.type == SDL_KEYUP ) {
                if( event.key.keysym.sym == SDLK_ESCAPE ) {
                    quitGame = SDL_TRUE;
                }
                if( event.key.keysym.sym == SDLK_F11 ) {
                    Uint32 windowFlags = SDL_GetWindowFlags( gWindow );
                    SDL_SetWindowFullscreen( gWindow, windowFlags ^= SDL_WINDOW_FULLSCREEN );
                }
            }
            if( event.type == SDL_WINDOWEVENT ) {
                if( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
                    SDL_Log("Window resized to %d X %d", event.window.data1, event.window.data2 );
                    canvasWidth = event.window.data1;
                    canvasHeight = event.window.data2;
                    float ratio = (float)((float)canvasWidth/(float)viewportWidth);
                    SDL_RenderSetScale( gRenderer, ratio, ratio );
                    //SDL_RenderSetLogicalSize( gRenderer, canvasWidth, canvasHeight);
                }
            }
        }

        float ratio = (float)((float)canvasWidth/(float)viewportWidth);
        int adjCellSize = cellSize *(ratio);

        /**
         * Sim
         */
        if(lmbPressed) {
            // find cell
            int mx;
            int my;
            int row = 0;
            int col = 0;
            SDL_GetMouseState(&mx, &my);

            // click is inside of the tilemap area
            if( mx < tilemapPositionX+numTileMapCols*adjCellSize && my < tilemapPositionY+numTileMapRows*adjCellSize ) {
                col = (mx+tilemapPositionX)/adjCellSize;
                row = (my+tilemapPositionY)/adjCellSize;

                Uint32 value = 0x80000000;
                value >>= col;
                tilemapCols[ row ] |= value;
            }
        }
        else if(rmbPressed) {
            // find cell
            int mx;
            int my;
            int row = 0;
            int col = 0;
            SDL_GetMouseState(&mx, &my);

            // click is inside of the tilemap area
            if( mx < tilemapPositionX+numTileMapCols*adjCellSize && my < tilemapPositionY+numTileMapRows*adjCellSize ) {
                col = (mx+tilemapPositionX)/adjCellSize;
                row = (my+tilemapPositionY)/adjCellSize;

                Uint32 value = 0x80000000;
                value >>= col;
                if( ( tilemapCols[ row ] & value )== value ) 
                    tilemapCols[ row ] ^= value;    
            }
        }
        /**
         * Render
         * **/
        SDL_SetRenderDrawColor(gRenderer, black.r,black.g,black.b,black.a);
        SDL_RenderClear(gRenderer);

        // draw rectangles
        SDL_SetRenderDrawColor(gRenderer, yellow.r,yellow.g,yellow.b,255);
        SDL_Rect filledCell;
        
        filledCell.w = cellSize;
        filledCell.h = cellSize;
        for( int y = 0; y < numTileMapRows; y++ ) {
            Uint32 value = 0x80000000;
            for( int x = 0; x < numTileMapCols; x++ ){
                if( ( tilemapCols[ y ] & value ) == value ) {
                    filledCell.x = tilemapPositionX+x*cellSize;
                    filledCell.y = tilemapPositionY+y*cellSize;
                    SDL_RenderFillRect(gRenderer, &filledCell);
                }
                value >>= 1;
            }
        }

        renderGrid(gRenderer, tilemapPositionX,tilemapPositionY,numTileMapCols,numTileMapRows,cellSize,gridColor);

        SDL_RenderPresent(gRenderer);
    }
} 