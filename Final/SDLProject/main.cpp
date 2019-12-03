#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program, programUntexture;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define P_MAX_BULLETS 6
using namespace std;



struct GameState {
    Entity player; // player.isActive will be used to see if the game has ended or not throughout the code
    Entity player_bullets[P_MAX_BULLETS];
};

enum GameMode { GAME_LEVEL, GAME_OVER, GAME_WIN };
GameMode mode = GAME_LEVEL;

GameState state;
int pNextBullet = 0;

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
    displayWindow = SDL_CreateWindow("Final", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    // Initialize player
    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    state.player.textureID = LoadTexture("playerShip.png");
    
    // Map the textures
    GLuint playerBulletTextureID = LoadTexture("playerLaser.png");
    
    
    // Initialize player bullets
    for (int i = 0; i < P_MAX_BULLETS; i++) {
        state.player_bullets[i].isActive = false;
        state.player_bullets[i].isStatic = false;
        state.player_bullets[i].width = 0.1f;
        state.player_bullets[i].height = 0.5f;
        state.player_bullets[i].entityType = PLAYER_BULLET;
        state.player_bullets[i].textureID = playerBulletTextureID;
    }
    
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

// Player fires a bullet
void PlayerFire() {
    if (!state.player_bullets[pNextBullet].isActive) { // Check if we are out of bullets, if not then create
        state.player_bullets[pNextBullet].isActive = true;
        state.player_bullets[pNextBullet].position.x = state.player.position.x;
        state.player_bullets[pNextBullet].position.y = state.player.position.y + state.player.height/2 + 0.2f;
        state.player_bullets[pNextBullet].velocity.y = 3.0f;
        pNextBullet++;
        if (pNextBullet == P_MAX_BULLETS) pNextBullet = 0;
    }
}

// Checking if bullets have went out of bound or touched terrain. If so, deactivate them
void CheckBullets() {
    for (int i = 0; i < P_MAX_BULLETS; i++) {
//        if (state.player_bullets[i].isActive &&
//            (state.player_bullets[i].position.x < -5.5f ||
//             state.player_bullets[i].position.x > 5.5f)) state.player_bullets[i].isActive = false;
//
        if (state.player_bullets[i].isActive &&
            (state.player_bullets[i].position.y < -3.75f ||
             state.player_bullets[i].position.y > 3.75f)) state.player_bullets[i].isActive = false;
    }
}

// Used so the enemy is firing at a fixed rate
#define FIXED_ENEMY_FIRE_RATE 1.5f
float fireAccumulator = 0.0f;
float lastFireTicks = 0.0f;

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (state.player.isActive) state.player.Jump();
                        break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        if (state.player.isActive) PlayerFire();
                        break;
                }
                break;
        }
    }
    if (state.player.isActive) { // Basically if the game is still going
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float deltaTime = ticks - lastFireTicks;
        lastFireTicks = ticks;
        
        fireAccumulator += deltaTime;
        if (fireAccumulator > FIXED_ENEMY_FIRE_RATE) {
            fireAccumulator = 0.0f;
        }
        state.player.velocity.x = 0;
        state.player.velocity.y = 0;
        
        // Check for pressed/held keys below & making sure we stay within screen boundary
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (keys[SDL_SCANCODE_LEFT] && state.player.position.x > -5.0f + state.player.width/2)
        {
            state.player.velocity.x = -3.0f;
        }
        else if  (keys[SDL_SCANCODE_RIGHT] && state.player.position.x < 5.0f - state.player.width/2)
        {
            state.player.velocity.x = 3.0f;
        }
        if  (keys[SDL_SCANCODE_UP] && state.player.position.y < 3.75f - state.player.height/2)
        {
            state.player.velocity.y = 3.0f;
        }
        else if  (keys[SDL_SCANCODE_DOWN] && state.player.position. y > -3.75f + state.player.height/2)
        {
            state.player.velocity.y = -3.0f;
        }
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    CheckBullets();
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Player collision checks against platforms & enemies
        state.player.Update(FIXED_TIMESTEP);
        

        for (int i = 0; i < P_MAX_BULLETS; i++) {
            if (state.player_bullets[i].isActive) { // Only update if they are active
                // Player bullets collision only checks against platforms & enemies
                state.player_bullets[i].Update(FIXED_TIMESTEP);
            }
        }
        
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player.Render(&program);

    for (int i = 0; i < P_MAX_BULLETS; i++) {
        if (state.player_bullets[i].isActive) state.player_bullets[i].Render(&program);
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
