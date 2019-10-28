#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

enum  EntityType { PLAYER, PLATFORM, ENEMY, ENEMY_BULLET, PLAYER_BULLET };


class Entity {
public:
    
    EntityType entityType;
    int ID;
    bool isStatic;
    bool isActive;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float width;
    float height;
    float direction;
        
    GLuint textureID;
    
    Entity();
    
    bool CheckCollision(Entity *other);
    
    void CheckCollisionsX(Entity *platforms, int bulletCount, Entity *others, int otherCount);
    void CheckCollisionsY(Entity *platforms, int bulletCount, Entity *others, int otherCount);
    
    void Update(float deltaTime, Entity *platforms, int platformCount, Entity *others, int otherCount);
    void Render(ShaderProgram *program);
    
    void Jump();
    
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    
};



