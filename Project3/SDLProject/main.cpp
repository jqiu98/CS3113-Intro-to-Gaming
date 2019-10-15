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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"
#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 540

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram programTex;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

enum GameMode { GAME_LEVEL, GAME_OVER };
GameMode mode = GAME_LEVEL;

struct GameState {
    Entity spacecraft;
    Entity terrain[2] ;
};

GameState state;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 3: Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    programTex.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    state.spacecraft.textureID = LoadTexture("me.png");
    state.spacecraft.position = glm::vec3(0, 2, 0);
    state.spacecraft.acceleration = glm::vec3(0, -9.81f, 0);
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    programTex.SetProjectionMatrix(projectionMatrix);
    programTex.SetViewMatrix(viewMatrix);
    programTex.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(programTex.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float r = 255;
    float g = 255;
    float b = 255;
    glClearColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
}


void ProcessInputGameLevel() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
        }
    }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_LEFT] && state.spacecraft.pressedAcceleration.x > -1) {
        state.spacecraft.pressedAcceleration.x -= 0.05;
    }
    else if (keys[SDL_SCANCODE_RIGHT] && state.spacecraft.pressedAcceleration.x < 1) {
        state.spacecraft.pressedAcceleration.x += 0.05;
    }
    if (keys[SDL_SCANCODE_UP]) {
        state.spacecraft.thruster = true;
    }
}

void processInputGameOver() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
        }
    }
}

void ProcessInput() {
    switch (mode) {
        case GAME_LEVEL:
            ProcessInputGameLevel();
            break;
        case GAME_OVER:
            processInputGameOver();
            break;
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void updateGameLevel(float deltaTime) {
    state.spacecraft.Update(deltaTime, &state.terrain, state.terrain.size());
}

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        updateGameLevel(FIXED_TIMESTEP);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}

void RenderGameLevel() {
    state.spacecraft.Render(&programTex);
}

void RenderGameOver() {
    
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    switch (mode) {
        case GAME_LEVEL:
            RenderGameLevel();
            break;
            
        case GAME_OVER:
            RenderGameOver();
            break;
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
