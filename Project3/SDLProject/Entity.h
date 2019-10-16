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

enum EntityType { SPACECRAFT, PLATFORM};

class Entity {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 gravity; // Gravity constantly acted upon the spacecraft
    glm::vec3 acceleration; // User controlled acceleration
    
    float width;
    float height;
    
    bool thruster; // Toggle for when the thruster/booster is on in order to update acceleration
    bool collided; // Boolean to determine if we've collided
    bool isSuccess; // Boolean to check if the mission was successful
    GLuint textureID;
    
    int cols; // columns for sprite sheet
    int rows; // rows for sprite sheet
    int index; // index position in sprite sheet
    
    Entity();
    Entity(float x, float y, float width_, float height_, GLuint textureID_);
    
    void Update(float deltaTime, Entity *objects, int objectCount);
    void Render(ShaderProgram *program);
    bool CheckCollision(Entity other);
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckInBound(); // Check if object is within the bounds of the screen, deal accordingly
    void CheckMission(Entity other); // Determine if the mission was successful or not
    void DrawText(ShaderProgram *program);
};
