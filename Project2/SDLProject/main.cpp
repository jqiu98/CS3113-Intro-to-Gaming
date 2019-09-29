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
#include "Player.h"
#include "Ball.h"
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 540
#define SPEED 5

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram programUntex;
glm::mat4 viewMatrix, projectionMatrix, modelMatrix;

// Generate player paddles
Player Player1 = Player(-4.75, 0, 1);
Player Player2 = Player(4.75, 0, 2);

// Generate game ball
Ball GameBall = Ball();

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 2: Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    programUntex.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -2.81f, 2.81f, -1.0f, 1.0f);
    modelMatrix = glm::mat4(1.0f);
    
    programUntex.SetProjectionMatrix(projectionMatrix);
    programUntex.SetViewMatrix(viewMatrix);
    programUntex.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float r = 242;
    float g = 232;
    float b = 92;
    glClearColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
        }
    }
    
    // Reset players movement matrix
    Player1.movement = glm::vec3(0, 0, 0);
    Player2.movement = glm::vec3(0, 0, 0);
    
    if (GameBall.position.x > -4.85 && GameBall.position.x < 4.85) { // Ball within x-cords of screen
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
         // Update Player 1 movement: Keys W & S
        if (keys[SDL_SCANCODE_W] && Player1.position.y < 2.06) Player1.movement.y += 1.0f;
        else if (keys[SDL_SCANCODE_S] && Player1.position.y > -2.06) Player1.movement.y -= 1.0f;
        
        // Update Player 2 movement: Keys UP & DOWN
        if (keys[SDL_SCANCODE_UP] && Player2.position.y < 2.06) Player2.movement.y += 1.0f;
        else if (keys[SDL_SCANCODE_DOWN] && Player2.position.y > -2.06) Player2.movement.y -= 1.0f;
    }
    else { // Game ended
        GameBall.movement = glm::vec3(0, 0, 0); // Stop the ball
    }
    
    // Keeps ball within y-cord of screen, extra 0.1 padding just in case
    if (GameBall.position.y > 2.66 || GameBall.position.y < -2.66) {
        GameBall.movement.y = -GameBall.movement.y;
    }
    
    // Collision Control between: Player1 & ball, Player2 & ball;
    float xDistP1 = fabs(Player1.position.x - GameBall.position.x) - ((0.3 + 0.3) / 2.0f);
    float yDistP1 = fabs(Player1.position.y - GameBall.position.y) - ((1.5 + 0.3) / 2.0f);
    
    float xDistP2 = fabs(Player2.position.x - GameBall.position.x) - ((0.3 + 0.3) / 2.0f);
    float yDistP2 = fabs(Player2.position.y - GameBall.position.y) - ((1.5 + 0.3) / 2.0f);
    
    if (xDistP1 < 0 && yDistP1 < 0) {
        GameBall.movement.x = -GameBall.movement.x;
        GameBall.colorMode = 1;
    }
    else if (xDistP2 < 0 && yDistP2 < 0) {
        GameBall.movement.x = -GameBall.movement.x;
        GameBall.colorMode = 2;
    }
}

float lastTicks = 0;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    Player1.Update(deltaTime);
    Player2.Update(deltaTime);
    GameBall.Update(deltaTime);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    Player1.Render(&programUntex);
    Player2.Render(&programUntex);
    GameBall.Render(&programUntex);
    
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
