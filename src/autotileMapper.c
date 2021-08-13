/*****
 * USE THIS TO CREATE TILEMAPS THAT
 * ARE AUTOTILED
 * 
 */


#include <SDL2/SDL.h>
#include "renderUtil.h"
#include "arrUtil.h"
#include "loadSaveUtil.h"

SDL_Renderer *gRenderer = NULL;
SDL_Window *gWindow = NULL;
SDL_bool gIsFullscreen = SDL_FALSE;

SDL_Color gridColor = {255,255,255,50};
SDL_Color black = {0,0,0,255};
SDL_Color yellow = {120,80,0};

int canvasWidth = 1920;
int canvasHeight = 1040;
int viewportWidth = 1920;
int viewportHeight = 1040;

Uint8 *gTilesetBitmasks;
int gTilesetRows = 8;
int gTilesetCols = 20;
//SDL_Texture *gTilesetTexture = NULL;
Tileset gTileset;
char *gTilesetFilename = "res/tileset.png";

const unsigned int gCellSize = 40;



typedef enum SurroundingTilesValues {
    NW_VALUE = 0x01,
    N_VALUE  = 0x02,
    NE_VALUE = 0x04,
    W_VALUE  = 0x08,
    E_VALUE  = 0x10,
    SW_VALUE = 0x20,
    S_VALUE  = 0x40,
    SE_VALUE = 0x80
} SurroundingTilesValues;


typedef struct idx2D {
    int row, col;
} idx2D;


Uint8 
bitmaskSumForSurroundingTiles(Uint32 *tilemap, int row, int col, int numRows, int numCols) {
    idx2D tNW, tN, tNE;
    idx2D tW,      tE;
    idx2D tSW, tS, tSE;

    Uint32 leftMostOne = 0x80000000;
    Uint32 colBitOne = 0x00;

    tNW.row = row - 1;
    tNW.col = col - 1;

    tN.row = row - 1;
    tN.col = col;

    tNE.row = row - 1;
    tNE.col = col + 1;

    tW.row = row;
    tW.col = col - 1;

    tE.row = row;
    tE.col = col + 1;
    
    tSW.row = row + 1;
    tSW.col = col - 1;

    tS.row = row + 1;
    tS.col = col;


    tSE.row = row + 1;
    tSE.col = col + 1;

    int sum = 0;

    colBitOne = leftMostOne >> tNW.col;

    if( tNW.row < 0 || tNW.col < 0) {
        sum += NW_VALUE;
    }
    else if( ( ( tilemap[tNW.row] & colBitOne ) == colBitOne )   ) {
        sum += NW_VALUE;
    }


    colBitOne = leftMostOne >> tN.col;
    if( tN.row < 0) {
        sum += N_VALUE;
    }
    else if(  ( tilemap[tN.row] & colBitOne ) == colBitOne  ) {
        sum += N_VALUE;
    }


    colBitOne = leftMostOne >> tNE.col;
    if( tNE.row < 0 || tNE.col >= numCols) {
        sum += NE_VALUE;
    }
    else if( ( tilemap[tNE.row] & colBitOne ) == colBitOne ) {
        sum += NE_VALUE;
    }


    colBitOne = leftMostOne >> tW.col;
    if(  tW.col < 0  ) {
        sum += W_VALUE;
    }
    else if( ( tilemap[tW.row] & colBitOne ) == colBitOne ) {
        sum += W_VALUE;
    }


    colBitOne = leftMostOne >> tE.col;
    if(  tE.col >= numCols ) {
        sum += E_VALUE;
    }
    else if( ( tilemap[tE.row] & colBitOne ) == colBitOne ) {
        sum += E_VALUE;
    }


    colBitOne = leftMostOne >> tSW.col;
    if(tSW.row >= numRows || tSW.col < 0) {
        sum += SW_VALUE;
    }
    else if( ( tilemap[tSW.row] & colBitOne ) == colBitOne ) {
        sum += SW_VALUE;
    }


    colBitOne = leftMostOne >> tS.col;
    if(  tS.row >= numRows ) {
        sum += S_VALUE;
    }
    else if( ( tilemap[tS.row] & colBitOne ) == colBitOne  ) {
        sum += S_VALUE;
    }


    colBitOne = leftMostOne >> tSE.col;
    if(  tSE.row >= numRows || tSE.col >= numCols  ){
        sum += SE_VALUE;
    }
    else if( ( tilemap[tSE.row] & colBitOne ) == colBitOne ) {
        sum += SE_VALUE;
    }


    return sum;


}


void
generateAutoTiledMap(Uint32 *tilemap, int numRows, int numCols) {

    Uint32 tileBitmask = 0x00;
    Uint8 tilesetBitmask = 0x00;
    SDL_Rect srcRect = {0,0,gCellSize,gCellSize};
    SDL_Rect dstRect = {0,0,gCellSize,gCellSize};
    SDL_bool breakOut = SDL_FALSE;
    // just render for now
    for( int r = 0; r < numRows; r++ ) {
        for( int c = 0; c < numCols; c++ ) {
            tileBitmask = bitmaskSumForSurroundingTiles(tilemap, r, c, numRows, numCols);
            // look for tile in tileset with this bitmask
            for( int tsR = 0; tsR < gTilesetRows; tsR++ ) {
                for( int tsC = 0; tsC < gTilesetCols; tsC++ ) {
                    tilesetBitmask = acc2DArray(gTilesetBitmasks, tsR, tsC, gTilesetCols);
                    if( tilesetBitmask == tileBitmask ) {
                        srcRect.x = tsC*gCellSize;
                        srcRect.y = tsR*gCellSize;
                        dstRect.x = (c)*gCellSize;
                        dstRect.y = r*gCellSize;
                        SDL_RenderCopy(gRenderer, gTileset.texture, &srcRect, &dstRect);
                        breakOut = SDL_TRUE;
                        break;
                    }
                }
                if( breakOut ) {
                    breakOut = SDL_FALSE;
                    break;
                }
            }
        }
    }
}


int 
main() {
    const unsigned int numTileMapRows = 32;
    const unsigned int numTileMapCols = 32;


    Uint32 tilemapCols[numTileMapRows]; // tile mask. Is there a tile here

    const unsigned int numRowsInTileset = 4;
    const unsigned int numColsInTileset = 40;
    Uint8 autoTileBitmasks[numRowsInTileset][numColsInTileset]; // what is the autoTiling bitmask of each tile in the set

    SDL_bool saveButtonReleased = SDL_FALSE;
    SDL_bool loadButtonReleased = SDL_FALSE;




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

    if( SDL_Init( SDL_INIT_VIDEO ) < 0) {
        fprintf( stderr, "Error %s\n", SDL_GetError() );
        exit( EXIT_FAILURE );
    }
    if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Image failed to init. %s\n", SDL_GetError());
        exit(1);
    }

    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if( gIsFullscreen ) {
        windowFlags |= SDL_WINDOW_FULLSCREEN ;
    }
    gWindow = SDL_CreateWindow( "Auto tile Mapper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, canvasWidth, canvasHeight, windowFlags);
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

    gTileset = loadTilesetImageFromFile(gRenderer, gTilesetFilename, 40);

    gTilesetBitmasks = (Uint8 *) malloc(sizeof(Uint8) * gTilesetRows * gTilesetCols);


    /***
     * Zero it out
     * 
     */
    for( int row = 0; row < gTilesetRows; row++ ) {
        for( int col = 0; col < gTilesetCols; col++ ) {
            set2DArrayElm(gTilesetBitmasks, row, col, gTilesetCols, 0x00);
        }
    }
    loadResourceFromFile(gTilesetBitmasks, "res/autoTileBitmasks", sizeof(Uint8), gTilesetRows*gTilesetCols);
    
    SDL_bool quitGame = SDL_FALSE;
    SDL_Event event;
    SDL_bool lmbPressed = SDL_FALSE;
    SDL_bool rmbPressed = SDL_FALSE;

    while(!quitGame){
        saveButtonReleased = SDL_FALSE;
        loadButtonReleased = SDL_FALSE;
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
                else if( event.key.keysym.sym == SDLK_F11 ) {
                    Uint32 windowFlags = SDL_GetWindowFlags( gWindow );
                    SDL_SetWindowFullscreen( gWindow, windowFlags ^= SDL_WINDOW_FULLSCREEN );
                }
                else if( event.key.keysym.sym == SDLK_s ) {
                    saveButtonReleased = SDL_TRUE;
                    
                }
                else if( event.key.keysym.sym == SDLK_l) {
                    loadButtonReleased = SDL_TRUE;
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
        int adjCellSize = gCellSize *(ratio);

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
        // save stuff
        if(saveButtonReleased) {
            SDL_bool success = saveResourceToFile(tilemapCols, "res/tilemapCols", sizeof(Uint32), numTileMapRows);
            if(!success) {
                fprintf(stderr, "%s\n", SDL_GetError());
                exit(1);
            }
        }

        // load stuff
        if(loadButtonReleased) {
            SDL_bool success = loadResourceFromFile(tilemapCols, "res/tilemapCols", sizeof(Uint32), numTileMapRows);
            if(!success) {
                fprintf(stderr, "%s\n", SDL_GetError());
                exit(1);
            }
        }
        /**
         * Render
         * **/
        SDL_SetRenderDrawColor(gRenderer, black.r,black.g,black.b,black.a);
        SDL_RenderClear(gRenderer);

        generateAutoTiledMap(tilemapCols,numTileMapRows,numTileMapCols );

        // draw rectangles
        SDL_SetRenderDrawColor(gRenderer, yellow.r,yellow.g,yellow.b,120);
        SDL_Rect filledCell;
        
        filledCell.w = gCellSize;
        filledCell.h = gCellSize;
        for( int y = 0; y < numTileMapRows; y++ ) {
            Uint32 value = 0x80000000;
            for( int x = 0; x < numTileMapCols; x++ ){
                if( ( tilemapCols[ y ] & value ) == value ) {
                    filledCell.x = tilemapPositionX+x*gCellSize;
                    filledCell.y = tilemapPositionY+y*gCellSize;
                    SDL_RenderFillRect(gRenderer, &filledCell);
                }
                value >>= 1;
            }
        }

        renderGrid(gRenderer, tilemapPositionX,tilemapPositionY,numTileMapCols,numTileMapRows,gCellSize,gridColor);


        SDL_RenderPresent(gRenderer);
    }
} 