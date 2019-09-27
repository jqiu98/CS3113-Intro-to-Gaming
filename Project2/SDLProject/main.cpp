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

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 540
#define SPEED 5

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram programTex, programUntex;
glm::mat4 viewMatrix, projectionMatrix, stuffMatrix, player1Matrix, player2Matrix;

float p1X = -4.9f;
float p1Y = 1.0f;
float p2X = 4.9f;
float p2Y = 1.0f;

float ballX;
float ballY;

bool winGame = false;

GLuint xTextureID;

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
    displayWindow = SDL_CreateWindow("Project 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    programTex.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    programUntex.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -2.81f, 2.81f, -1.0f, 1.0f);
    
    player1Matrix = glm::mat4(1.0f);
    player2Matrix = glm::mat4(1.0f);
    
    programTex.SetProjectionMatrix(projectionMatrix);
    programTex.SetViewMatrix(viewMatrix);
    programTex.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    programUntex.SetProjectionMatrix(projectionMatrix);
    programUntex.SetViewMatrix(viewMatrix);
    programUntex.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(programTex.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float r = 255;
    float g = 255;
    float b = 255;
    glClearColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}


void Update() {
}


void drawPlayer1() {
    float vertices[] = {
        p1X, p1Y,
        p1X, p1Y-1.5f,
        p1X+0.3f, p1Y-1.5f,
        p1X+0.3f, p1Y };
    
    glVertexAttribPointer(programUntex.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(programUntex.positionAttribute);
    float r = 255;
    float g = 0;
    float b = 0;
    programUntex.SetColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
    programUntex.SetModelMatrix(player1Matrix);
    glDrawArrays(GL_QUADS, 0, 4);
    
    glDisableVertexAttribArray(programUntex.positionAttribute);
}

void drawPlayer2() {
    float vertices[] = {
        p2X, p2Y,
        p2X, p2Y-1.5f,
        p2X-0.3f, p2Y-1.5f,
        p2X-0.3f, p2Y };
    
    glVertexAttribPointer(programUntex.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(programUntex.positionAttribute);
    float r = 255;
    float g = 0;
    float b = 0;
    programUntex.SetColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
    programUntex.SetModelMatrix(player1Matrix);
    glDrawArrays(GL_QUADS, 0, 4);
    
    glDisableVertexAttribArray(programUntex.positionAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawPlayer1();
    drawPlayer2();
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
