#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_opengles2.h> 
#include <stdio.h>

/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; float w ; 
} Vec4f ; 
/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; 
} Vec3f ; 
/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; float w ; 
} Vec4i ; 
/*............................................................*/
typedef struct {
    float x ; float y ; float z  ; 
} Vec3i ; 
/*............................................................*/
/*............................................................*/

typedef enum {
    ALICE_ID,
    BOB_ID,
    CHARLIE_ID
} SceneNodeType;

typedef struct SceneNode SceneNode; // forward declaration

typedef struct {
    int id ; 
    Vec3f position ; 
    Vec3f rotation ; 
    Vec3f scale ; 
    SceneNodeType typ ; 
    SceneNode **childreen ;
    SceneNode *parent ; 
} SceneNode ; 

typedef struct {
    
} Scene ; 

typedef struct {
    
} GameData ; 

typedef struct {
    GameData* data ; // saved game to saved here , they should loaded from engine infrastructure to here on game started 

} Game ; 

void on_game_started(Game* game)