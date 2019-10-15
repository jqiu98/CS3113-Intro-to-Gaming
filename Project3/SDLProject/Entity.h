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

enum EntityType { SPACECRAFT, PLATFORM, MOUNTAIN };

class Entity {
public:
    
    EntityType entityType;
    EntityType lastCollision;
    
    bool isStatic;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 pressedAcceleration;
    
    float width;
    float height;
    bool thruster;
    
    float speed;
    
    
    GLuint textureID;
    
    Entity(float w, float h);
    
    void Update(float deltaTime, Entity *objects, int objectCount);
    void Render(ShaderProgram *program);
    bool CheckCollision(Entity other);
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
};
