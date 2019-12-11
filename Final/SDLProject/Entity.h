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

//#include "Map.h"

enum  EntityType { PLAYER, ENEMY, ENEMY_BULLET, PLAYER_BULLET, LIVES, PLATFORM };


class Entity {
public:
    
    EntityType entityType;
    bool isStatic;
    bool isActive;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float width;
    float height;
    
    int life;
    int killCount;
    
    bool seeker;
    bool shooter;
    bool boss;
    
    float fireAccumulator;
    float lastFire;
        
    GLuint textureID;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    
    void CheckCollisionsX(Entity *player, Entity *others, int otherCount);
    void CheckCollisionsY(Entity *player, Entity *others, int otherCount);
    
    void UpdateAlive();
    
    void UpdateEnemy(int playerX);
    void Update(float deltaTime, Entity *player, Entity *others, int otherCount);
    void Render(ShaderProgram *program);
};



