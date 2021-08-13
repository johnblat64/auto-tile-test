/**
 * This program sets the bitmask for tiles in a tileset
 */

#include <SDL2/SDL.h>
#include "renderUtil.h"
#include "loadSaveUtil.h"
#include "arrUtil.h"



SDL_Renderer *gRenderer = NULL;
SDL_Color yellow = {150,80,0,200};
SDL_Color red = {255,20,20,120};
SDL_Color gridColor = {255,255,255,50};
SDL_Color black = {0,0,0,255};

SDL_Window *gWindow = NULL;
SDL_bool gIsFullscreen = SDL_FALSE;

SDL_Texture *gTilesetTexture = NULL;
char *gTilesetFilename = "res/tileset.png";
const unsigned int gTilesetRows = 8;
unsigned int gTilesetCols = 20;

const unsigned int tileCellSize = 40;

Uint8 *gAutoTileBitmasks;

const int numberOfBits = 3;
const unsigned int gVisualBitmaskCellSize = tileCellSize/3;

Uint8 bitmaskCellValues[3][3] = {
    {0x01, 0x02, 0x04 },
    {0x08, 0x00, 0x10 },
    {0x20, 0x40, 0x80 }
} ;






int
main() {
    Tileset tileset;
    SDL_bool lmbPressed = SDL_FALSE;
    SDL_bool rmbPressed = SDL_FALSE;
    SDL_bool saveButtonReleased = SDL_FALSE;
    SDL_bool loadButtonReleased = SDL_FALSE;

    int mouseX = 0;
    int mouseY = 0;
    int logicalMouseX = 0;
    int logicalMouseY = 0;

    unsigned int origViewportWidth = gTilesetCols*tileCellSize;
    unsigned int origViewportHeight = gTilesetRows*tileCellSize;
    /**
     * Set up SDL
     */
    // Initializing stuff
    if( SDL_Init( SDL_INIT_VIDEO ) < 0) {
        fprintf( stderr, "Error %s\n", SDL_GetError() );
        exit( EXIT_FAILURE );
    }
    if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Image failed to init. %s\n", SDL_GetError());
        exit(1);
    }

    #pragma region initWindow
    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if( gIsFullscreen ) {
        windowFlags |= SDL_WINDOW_FULLSCREEN ;
    }
    gWindow = SDL_CreateWindow( "Tileset Bitmask Setter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, origViewportWidth, origViewportHeight, windowFlags);
    if ( gWindow == NULL ) {
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }
    #pragma endregion 

    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if (gRenderer == NULL ) {
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }


    if( SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND) < 0 ){
        fprintf( stderr, "Error %s\n ", SDL_GetError() );
        exit( EXIT_FAILURE );
    }

    tileset = loadTilesetImageFromFile(gRenderer, "res/tileset.png", 40);

    gAutoTileBitmasks = (Uint8 *) malloc(sizeof(Uint8) * gTilesetRows * gTilesetCols);


    /***
     * Zero it out
     * 
     */
    for( int row = 0; row < gTilesetRows; row++ ) {
        for( int col = 0; col < gTilesetCols; col++ ) {
            set2DArrayElm(gAutoTileBitmasks, row, col, gTilesetCols, 0x00);
        }
    }
    
    SDL_RenderSetLogicalSize(gRenderer, gTilesetCols*tileCellSize, gTilesetRows*tileCellSize);
    
    

    SDL_bool quitGame = SDL_FALSE;
    SDL_Event event;

    /**
     * Run the loop
     */
    while(!quitGame){

        saveButtonReleased = SDL_FALSE;
        loadButtonReleased = SDL_FALSE;

        SDL_GetMouseState(&mouseX, &mouseY);
        int windowWidth, windowHeight;
        SDL_GetWindowSize(gWindow, &windowWidth, &windowHeight);
        
        /**
         * EVENTS
         * **/
        while(SDL_PollEvent(&event)){
            if( event.type == SDL_QUIT) {
                quitGame = SDL_TRUE;
                break;
            }
            else if( event.type == SDL_KEYUP ) {
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
            else if( event.type == SDL_WINDOWEVENT ) {
                if( event.window.event == SDL_WINDOWEVENT_RESIZED ) {
                    SDL_Log("Window resized to %d X %d", event.window.data1, event.window.data2 );
                }
            }
            else if( event.type == SDL_MOUSEBUTTONUP ) {
                if( event.button.button == SDL_BUTTON_LEFT ) {
                    lmbPressed = SDL_FALSE;
                }
                else if( event.button.button == SDL_BUTTON_RIGHT ) {
                    rmbPressed = SDL_FALSE;
                }

            } 
            else if( event.type == SDL_MOUSEBUTTONDOWN ) {
                if( event.button.button == SDL_BUTTON_LEFT ) {
                    lmbPressed = SDL_TRUE;
                }
                else if( event.button.button == SDL_BUTTON_RIGHT ) {
                    rmbPressed = SDL_TRUE;
                }

            } 

        }

        // did player click
        if(lmbPressed) {

            windowMousePositionToLogicalMousePosition(gRenderer, gWindow, mouseX, mouseY, &logicalMouseX, &logicalMouseY);

            // set the bitmask of the tile
            int rowTileClicked = logicalMouseY / tileCellSize;
            int colTileClicked = logicalMouseX / tileCellSize;
            if((rowTileClicked < gTilesetRows && colTileClicked < gTilesetCols) ) {
                int tileOffsetX = colTileClicked*tileCellSize;
                int tileOffsetY = rowTileClicked*tileCellSize;
                
                int bitCellSize = tileCellSize*0.3333;
                int relativeMouseX = logicalMouseX - tileOffsetX;
                int relativeMouseY = logicalMouseY - tileOffsetY;

                int bitClickedRow = relativeMouseY / bitCellSize;
                int bitClickedCol = relativeMouseX / bitCellSize;
                
                Uint8 clickedBit = bitmaskCellValues[bitClickedRow][bitClickedCol];
                
                Uint8 currentBitmaskOfClickedTile = acc2DArray(gAutoTileBitmasks, rowTileClicked, colTileClicked, gTilesetCols);

                currentBitmaskOfClickedTile |= clickedBit;

                set2DArrayElm(gAutoTileBitmasks, rowTileClicked, colTileClicked, gTilesetCols, currentBitmaskOfClickedTile);
            }
        }
        if(rmbPressed) {

            windowMousePositionToLogicalMousePosition(gRenderer, gWindow, mouseX, mouseY, &logicalMouseX, &logicalMouseY);

            // set the bitmask of the tile
            int rowTileClicked = logicalMouseY / tileCellSize;
            int colTileClicked = logicalMouseX / tileCellSize;
            if((rowTileClicked < gTilesetRows && colTileClicked < gTilesetCols) ) {
                int tileOffsetX = colTileClicked*tileCellSize;
                int tileOffsetY = rowTileClicked*tileCellSize;
                
                int bitCellSize = tileCellSize*0.3333;
                int relativeMouseX = logicalMouseX - tileOffsetX;
                int relativeMouseY = logicalMouseY - tileOffsetY;

                int bitClickedRow = relativeMouseY / bitCellSize;
                int bitClickedCol = relativeMouseX / bitCellSize;
                
                Uint8 clickedBit = bitmaskCellValues[bitClickedRow][bitClickedCol];
                
                Uint8 currentBitmaskOfClickedTile = acc2DArray(gAutoTileBitmasks, rowTileClicked, colTileClicked, gTilesetCols);
                
                if( (clickedBit & currentBitmaskOfClickedTile ) == clickedBit ) {
                    currentBitmaskOfClickedTile ^= clickedBit;
                }
                
            
                set2DArrayElm(gAutoTileBitmasks, rowTileClicked, colTileClicked, gTilesetCols, currentBitmaskOfClickedTile);
            }
        }

        // save stuff
        if(saveButtonReleased) {
            SDL_bool success = saveResourceToFile(gAutoTileBitmasks, "res/autoTileBitmasks", sizeof(Uint8), gTilesetRows*gTilesetCols);
            if(!success) {
                fprintf(stderr, "%s\n", SDL_GetError());
                exit(1);
            }
        }

        // load stuff
        if(loadButtonReleased) {
            SDL_bool success = loadResourceFromFile(gAutoTileBitmasks, "res/autoTileBitmasks", sizeof(Uint8), gTilesetRows*gTilesetCols);
            if(!success) {
                fprintf(stderr, "%s\n", SDL_GetError());
                exit(1);
            }
        }

        SDL_SetRenderDrawColor(gRenderer, black.r,black.g,black.b,black.a);
        SDL_RenderClear(gRenderer);


        /****
         * Visualize the Autotiling bitmask for each tile
         * 
         */
        
        // render the tileset
        SDL_Rect tilesetRenderRect = {0,0,tileset.cols*tileset.tileCellSize,tileset.rows*tileset.tileCellSize};
        SDL_RenderCopy(gRenderer, tileset.texture, NULL, &tilesetRenderRect );


        SDL_SetRenderDrawColor( gRenderer,red.r,red.g,red.b,red.a );
        SDL_Rect visualBitmaskRect;
        visualBitmaskRect.w = gVisualBitmaskCellSize;
        visualBitmaskRect.h = gVisualBitmaskCellSize;

        Uint8 currentAutoTileBitmask;
        for( int row = 0; row < gTilesetRows; row++ ) {
            for( int col = 0; col < gTilesetCols; col++ ) {

                int offsetInTilesetX = col*tileCellSize;
                int offsetInTilesetY = row*tileCellSize;
                // set center
                visualBitmaskRect.x = offsetInTilesetX + gVisualBitmaskCellSize;
                visualBitmaskRect.y = offsetInTilesetY + gVisualBitmaskCellSize;
                SDL_SetRenderDrawColor( gRenderer, yellow.r, yellow.g, yellow.b, yellow.a );
                SDL_RenderFillRect( gRenderer, &visualBitmaskRect);

                SDL_SetRenderDrawColor( gRenderer, red.r,red.g,red.b,red.a);

                Uint8 rightMostOne = 0x01;
                Uint8 value = 0b0;
                // 9 visual cells, but the uint8s only have 8. The middle one isn't really used
                unsigned int visualCellIdx = 0;
                for( int i = 0; i < 8; i++ ) {
                    value = rightMostOne << i;
                    currentAutoTileBitmask = acc2DArray(gAutoTileBitmasks, row, col, gTilesetCols);

                    visualCellIdx = i >= 4 ? i+1 : i; // skip over idx 4 in visual. 
                    if( (currentAutoTileBitmask & value) == value ) { // bit is selected

                        

                        if( visualCellIdx >= 0 & visualCellIdx < numberOfBits ) {
                            visualBitmaskRect.x = offsetInTilesetX + ( (i%numberOfBits)*gVisualBitmaskCellSize);
                            visualBitmaskRect.y = offsetInTilesetY ;
                        }

                        else if( visualCellIdx >= numberOfBits && visualCellIdx < numberOfBits*2) {
                            visualBitmaskRect.x = offsetInTilesetX + ( (visualCellIdx%numberOfBits)*gVisualBitmaskCellSize);
                            visualBitmaskRect.y = gVisualBitmaskCellSize + offsetInTilesetY ;
                        }

                        else if( visualCellIdx >= numberOfBits*2 && visualCellIdx < numberOfBits*3 ) {
                            visualBitmaskRect.x = offsetInTilesetX + ( (visualCellIdx%numberOfBits)*gVisualBitmaskCellSize);
                            visualBitmaskRect.y = 2*gVisualBitmaskCellSize + offsetInTilesetY ;
                        }
                        SDL_RenderFillRect(gRenderer, &visualBitmaskRect);
                    }
                }
            }
        }

        renderGrid(gRenderer, 0,0,gTilesetRows, gTilesetCols, tileCellSize, gridColor);


        SDL_RenderPresent(gRenderer );
        printf("hi\n");
    }

    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    SDL_Quit();
}