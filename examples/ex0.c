#include <SDL3/SDL.h>
#include <stdio.h>

// Define window dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    int quit = 0;
    SDL_Event e;

    // 1. Initialize SDL: Must initialize the Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        // Use SDL_Log for better SDL-specific logging
        SDL_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Create window and renderer.
    // NOTE: SDL3's signature for this function now requires the 'title' string first.
    if (SDL_CreateWindowAndRenderer(
            "My SDL3 Blue Window",  // 1. The window title
            SCREEN_WIDTH,           // 2. Window width
            SCREEN_HEIGHT,          // 3. Window height
            SDL_WINDOW_RESIZABLE,   // 4. Window flags
            &window,                // 5. Pointer to the created window pointer
            &renderer) < 0) {       // 6. Pointer to the created renderer pointer

        SDL_Log("Window and Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        // If creation failed, clean up initialized subsystems before exiting
        SDL_Quit();
        return 1;
    }

    // The Main Event and Rendering Loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // Check for the new SDL3 event type for quit
            if (e.type == SDL_EVENT_QUIT) {
                quit = 1;
            }
        }

        // --- Rendering Phase ---
        
        // Set the color to blue (RGBA: 0, 0, 255, 255)
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
        
        // Clear the screen with the draw color
        SDL_RenderClear(renderer); 

        // Update the screen with what we've drawn
        SDL_RenderPresent(renderer);
    }

    // 4. Cleanup and Shutdown
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    // Shut down all initialized SDL subsystems
    SDL_Quit();

    return 0;
}