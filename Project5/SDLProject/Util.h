#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

using namespace std;

class Util {
public:
    static GLuint fontTextureID;
    
    static GLuint LoadTexture(const char* filepath);
    static void DrawText(ShaderProgram *program, int fontTexture, string text, float size, float spacing, glm::vec3 position);
};
