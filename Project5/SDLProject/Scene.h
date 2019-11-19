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
#include "Map.h"


#define MAX_COIN_COUNT 100
#define MAX_ENEMIES 100


struct GameState {
    Entity player;
    Entity coins[MAX_COIN_COUNT];
    Entity enemies[MAX_ENEMIES];
    Map *map;
    int nextLevel;
    int lives;
};

class Scene {
public:
    int GameOver = 0;
    GameState state;
    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram *program) = 0;
};
