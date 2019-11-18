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

#include "Map.h"

enum  EntityType { PLAYER, ENEMY, COIN, PLAYER_BULLET, ENEMY_BULLET};


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
    
    float speed;
    
    int enemyMode;
    
    GLuint textureID;
    
    int cols;
    int rows;
    int *animLeftIndices;
    int *animRightIndices;
    int direction;
    int animFrames;
    int animIndex;
    float animTime;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
    
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);
    
    void Update(float deltaTime, Entity *coins, int coinCount, Map *map);
    void Render(ShaderProgram *program);
    void RenderAnim(ShaderProgram *program);
    
    void Jump();
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    
};



