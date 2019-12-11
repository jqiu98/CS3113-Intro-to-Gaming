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
#include "Util.h"
#include "Scene.h"
#include "Level1.h"
#include "GameMenu.h"
#define WIDTH 7.0f
#define HEIGHT 5.25f

using namespace std;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool GameStart = false;
bool GameOver = false;

Mix_Music *background;
Mix_Chunk *shoot;


Scene *currentScene;
Scene *sceneList[2];

void SwitchToScene(Scene *scene) {
    if (GameStart) {
        scene->state = currentScene->state;
    };
    currentScene = scene;
    currentScene->Initialize();
}


ShaderProgram program, programUntexture;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

int pNextBullet = 0;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Final", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 960, 720);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-WIDTH, WIDTH, -HEIGHT, HEIGHT, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    sceneList[0] = new GameMenu();
    sceneList[1] = new Level1();;
    SwitchToScene(sceneList[0]);
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    background = Mix_LoadMUS("backgroundSpace.mp3");
    shoot = Mix_LoadWAV("gunShoot.wav");
    
    Mix_PlayMusic(background, -1);
    Mix_Volume(-1, MIX_MAX_VOLUME);
}

void playShoot() {
    Mix_PlayChannel(-1, shoot, 0);
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
                    case SDLK_RETURN:
                        if (!GameStart) {
                            GameStart = true;
                            SwitchToScene(sceneList[1]);
                        }
                        break;
                }
                break;
        }
    }
    if (currentScene->state.player.isActive) { // Basically if the game is still going
        
        currentScene->state.player.velocity.x = 0;
        currentScene->state.player.velocity.y = 0;
        
        // Check for pressed/held keys below & making sure we stay within screen boundary
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (keys[SDL_SCANCODE_LEFT] && currentScene->state.player.position.x > -WIDTH + currentScene->state.player.width/2)
        {
            currentScene->state.player.velocity.x = -5.0f;
        }
        else if  (keys[SDL_SCANCODE_RIGHT] && currentScene->state.player.position.x < WIDTH - currentScene->state.player.width/2)
        {
            currentScene->state.player.velocity.x = 5.0f;
        }
        if  (keys[SDL_SCANCODE_UP] && currentScene->state.player.position.y < HEIGHT - currentScene->state.player.height/2)
        {
            currentScene->state.player.velocity.y = 3.0f;
        }
        else if  (keys[SDL_SCANCODE_DOWN] && currentScene->state.player.position. y > -HEIGHT + currentScene->state.player.height/2)
        {
            currentScene->state.player.velocity.y = -3.0f;
        }
        if (glm::length(currentScene->state.player.velocity) > 5.0f) {
            currentScene->state.player.velocity = glm::normalize(currentScene->state.player.velocity) * 5.0f;
        }
        if (keys[SDL_SCANCODE_LSHIFT]) {
            currentScene->Fire();
            playShoot();
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
    Mix_FreeChunk(shoot);
    Mix_FreeMusic(background);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
//        if (currentScene->state.nextLevel >= 0) SwitchToScene(sceneList[currentScene->state.nextLevel]);
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
