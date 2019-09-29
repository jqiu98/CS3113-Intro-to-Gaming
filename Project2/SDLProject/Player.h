#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include <SDL.h>
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

class Player {
public:
    glm::vec3 position;
    glm::vec3 movement;
    float speed;
    int colorMode;

    Player(float x, float y, int c);

    void Update(float deltatime);
    void Render(ShaderProgram *program);
};
