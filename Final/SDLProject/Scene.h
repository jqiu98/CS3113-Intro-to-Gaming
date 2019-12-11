#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"
//#include "Map.h"


#define P_MAX_BULLETS 100
#define MAX_ENEMIES 50
#define E_MAX_BULLETS 50

#define FIXED_PLAYER_FIRE_RATE 0.2f
#define FIXED_ENEMY_FIRE_RATE 1.0f




//struct GameState {
//    Entity player;
//    Entity coins[MAX_COIN_COUNT];
//    Entity enemies[MAX_ENEMIES];
//    Map *map;
//    int nextLevel;
//    int lives;
//};

struct GameState {
    Entity player;
    Entity player_bullets[P_MAX_BULLETS];
    Entity enemies[MAX_ENEMIES];
    Entity enemy_bullets[E_MAX_BULLETS];
    bool gameWon = false;
    
};

class Scene {
public:
    GameState state;
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram *program) = 0;
    virtual void Fire() = 0;
};
