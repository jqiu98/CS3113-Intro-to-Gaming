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

class Message {
public:
    int cols;
    int rows;
    int index;
    
    float width;
    float height;
    glm::vec3 position;

    GLuint textureID;
    
    Message();
    
    void DrawText(ShaderProgram *program);
};
