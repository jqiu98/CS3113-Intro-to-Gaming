#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "SDL_mixer.h"

#include "Entity.h"
#include "Map.h"
#include "Util.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "GameMenu.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool GameStart = false;


Scene *currentScene;
Scene *sceneList[4];

int lives;

Mix_Music *background;
Mix_Chunk *jump;



void SwitchToScene(Scene *scene) {
    if (GameStart) lives = currentScene->state.lives;
    currentScene = scene;
    currentScene->Initialize();
    currentScene->state.lives = lives;
}

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Project_5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
        
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
    
    lives = 3;
    sceneList[0] = new GameMenu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    background = Mix_LoadMUS("backgroundSneak.mp3");
    jump = Mix_LoadWAV("bounce.wav");
    
    Mix_PlayMusic(background, -1);
    Mix_Volume(-1, MIX_MAX_VOLUME);
}

void playJump() {
    Mix_PlayChannel(-1, jump, 0);
}

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
                        if (!GameStart) {
                            GameStart = true;
                            SwitchToScene(sceneList[3]);
                        }
                        else {
                            currentScene->state.player.Jump();
                            playJump();
                        }
                        break;
                        
                }
                break;
        }
    }
    if (currentScene->GameOver == 0) {
        currentScene->state.player.direction = 0;
        currentScene->state.player.velocity.x = 0;
        
        // Check for pressed/held keys below
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (keys[SDL_SCANCODE_A])
        {
            currentScene->state.player.velocity.x = -2.5f;
            currentScene->state.player.direction = -1;
        }
        else if  (keys[SDL_SCANCODE_D])
        {
            currentScene->state.player.velocity.x = 2.5f;
            currentScene->state.player.direction = 1;
        }
    }
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

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
        
        currentScene->Update(FIXED_TIMESTEP);
        
        viewMatrix = glm::mat4(1.0f);
        if (currentScene->state.player.position.x > 5 || currentScene->state.player.position.y < 3.75) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player.position.x, -currentScene->state.player.position.y, 0));
        }
        else viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        
        program.SetViewMatrix(viewMatrix);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    currentScene->Render(&program);
        
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    
    Mix_FreeChunk(jump);
    Mix_FreeMusic(background);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        if (currentScene->state.nextLevel >= 0) SwitchToScene(sceneList[currentScene->state.nextLevel]);
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
