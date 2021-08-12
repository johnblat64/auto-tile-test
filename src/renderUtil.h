#ifndef RENDER_UTIL_H
#define RENDER_UTIL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct Tileset {
    Uint32 tileCellSize;
    Uint32 rows;
    Uint32 cols;
    SDL_Texture *texture;
} Tileset;


void
windowMousePositionToLogicalMousePosition(SDL_Renderer *renderer, SDL_Window *window, int realMouseX, int realMouseY, int *logicalMouseX, int *logicalMouseY) {
    int wWidth, wHeight;
    int rLogicalWidth, rLogicalHeight;
    int rRealWidth, rRealHeight;
    float rScaleX, rScaleY;
    int rMidpointY, wMidpointY;
    int rMidpointX, wMidpointX;
    int rY, rX;

    SDL_GetWindowSize(window, &wWidth, &wHeight);
    wMidpointY = wHeight/2;
    wMidpointX = wWidth/2;

    SDL_RenderGetLogicalSize(renderer, &rLogicalWidth, &rLogicalHeight);
    SDL_RenderGetScale(renderer, &rScaleX, &rScaleY);
    rRealWidth = (float)rLogicalWidth*(float)rScaleX;
    rRealHeight = (float)rLogicalHeight*(float)rScaleY;
    rMidpointY = rRealHeight/2;
    rMidpointX = rRealWidth/2;
    rY = wMidpointY - rMidpointY;
    rX = wMidpointX - rMidpointX;

    int adjustedMouseY = realMouseY - rY; // takes into account any border when keeping aspect ratio
    int adjustedMouseX = realMouseX - rX;
    *logicalMouseX = (float)adjustedMouseX / (float)rRealWidth * (float)rLogicalWidth;
    *logicalMouseY = (  (float) adjustedMouseY) / (float)rRealHeight * (float)rLogicalHeight;

}

// must have SDL_image initialized with PNG
Tileset
loadTilesetImageFromFile(SDL_Renderer *renderer, const char *filename, Uint8 tileCellSize){
    Tileset ts;
    ts.tileCellSize = tileCellSize;

    SDL_Surface *surface = IMG_Load(filename);
    if(!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Could not load surface from %s. %s.", filename, SDL_GetError());
        exit(1);
    }
    ts.cols = surface->w / tileCellSize;
    ts.rows = surface->h / tileCellSize;

    ts.texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(!ts.texture){
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Could not load texture from surface created from %s. %s.", filename, SDL_GetError());
        exit(1);
    }

    SDL_FreeSurface(surface);
    surface = NULL;

    return ts;
}

void 
renderGrid(SDL_Renderer *renderer, unsigned int startingCanvasX, unsigned int startingCanvasY, unsigned int rows, unsigned int cols, unsigned int cellSize, SDL_Color color) {
    //float ratio = (float)((float)canvasWidth/(float)viewportWidth);

    int endingCanvasX = startingCanvasX+(cols*cellSize);
    int endingCanvasY = startingCanvasY+(rows*cellSize);


    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for(int x = startingCanvasX; x <= cols; x++ ) {
        SDL_RenderDrawLine(renderer, x*cellSize, startingCanvasY, x*cellSize, endingCanvasY  );
    }
    for( int y = startingCanvasY; y <= rows; y++ ) {
        SDL_RenderDrawLine(renderer, startingCanvasX, y*cellSize, endingCanvasX, y*cellSize);
    }
}

#endif