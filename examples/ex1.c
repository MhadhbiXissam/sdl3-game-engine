#include <SDL3/SDL.h>
#include <stdio.h>

// Global variable (or shared context) to store our custom event ID
Uint32 MY_CUSTOM_EVENT_TYPE = (Uint32)-1;

// Function that simulates some action and triggers the custom event
void trigger_my_event() {
    if (MY_CUSTOM_EVENT_TYPE == (Uint32)-1) {
        SDL_Log("Custom event type not registered!");
        return;
    }

    SDL_Event event;
    SDL_zero(event); // Initialize event structure to zero

    // Fill the event structure with our custom type and data
    event.type = MY_CUSTOM_EVENT_TYPE;
    event.user.code = 123;                  // Custom user code
    event.user.data1 = (void*)"Hello";      // Custom pointer data
    event.user.data2 = (void*)"World";      // Custom pointer data

    // Push the event onto the SDL event queue
    if (SDL_PushEvent(&event) == SDL_FALSE) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to push event: %s", SDL_GetError());
    } else {
        SDL_Log("Event pushed to queue from separate function.");
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SDL3 Custom Event Example", 800, 600, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Register a unique ID for our custom event
    MY_CUSTOM_EVENT_TYPE = SDL_RegisterEvents(1);
    if (MY_CUSTOM_EVENT_TYPE == (Uint32)-1) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_RegisterEvents failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Log("Custom Event Type registered with ID: %u", MY_CUSTOM_EVENT_TYPE);


    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while (!quit) {
        // Poll for events in the main loop
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    quit = SDL_TRUE;
                    break;
                
                // Handle our custom event type
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_SPACE) {
                         // Trigger the custom event when spacebar is pressed
                         trigger_my_event();
                    }
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    // Example of using a standard event
                    // SDL_Log("Mouse moved to (%f, %f)", event.motion.x, event.motion.y);
                    break;
                    
                default:
                    if (event.type == MY_CUSTOM_EVENT_TYPE) {
                        // Process our specific user event data
                        SDL_Log("Handled custom event! Code: %d, Data1: %s, Data2: %s",
                                (int)event.user.code, (char*)event.user.data1, (char*)event.user.data2);
                    }
                    break;
            }
        }

        // ... rendering code would go here ...
        SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}