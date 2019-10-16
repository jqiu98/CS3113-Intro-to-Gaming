// Press LEFT / RIGHT to change acceleration
// Press UP to activate the thruster to actually apply the acceleration

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
#include <vector>
#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 540
using namespace std;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram programTex;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

enum GameMode { GAME_LEVEL, GAME_OVER, GAME_SUCESS }; // { Playing, mission_failed, mission_successful } respectively

GameMode mode = GAME_LEVEL; // Start out playing

struct GameState {
    Entity spacecraft; // The spacecraft entity player controls
    Entity terrain[3]; // For the map terrain entities
    
    // Vectors holding all the text required when you fail or succeed
    vector<Entity> failText;
    vector<Entity> successText;
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
    
    // Map the texture to a varaible
    GLuint lunar = LoadTexture("lunar.png");
    GLuint ground = LoadTexture("ground.png");
    GLuint font = LoadTexture("font.png");
    
    // Initialize spacecraft
    state.spacecraft = Entity(0.0f, 2.5f, 0.5f, 0.5f, lunar);
    state.spacecraft.velocity = glm::vec3(0.0f, -0.2f, 0.0f);
    state.spacecraft.gravity = glm::vec3(0.0f, -0.1f, 0.0f);
    state.spacecraft. acceleration = glm::vec3(0.0f, 1.0f, 0.0f);
    
    state.spacecraft.thruster = false;
    state.spacecraft.collided = false;
    state.spacecraft.isSuccess = false;
    
    // Initialize the terrain
    state.terrain[0] = ( Entity (0.0f, -2.56f, 10.0f, 0.5f, ground) );
    state.terrain[1] = ( Entity (-2.0f, -1.56f, 1.0f, 1.5f, ground) );
    state.terrain[2] = ( Entity (1.0f, -1.91f, 2.0f, 0.8f, ground) );
    
    // Preload the text into the vectors to be displayed later on
    float textSize = 0.5f; // Size of the text -> width & height
    
    string text = "MISSION FAILED";
    float startInd = -(float)text.size()/2.0 * textSize; // Left most position for first character if text was centered
    
    for (int i = 0; i < text.size(); i++) {
        Entity aCharacter = Entity(startInd + textSize*i, textSize/2, textSize, textSize, font);
        aCharacter.cols = 16;
        aCharacter.rows = 16;
        aCharacter.index = (int)text[i];
        state.failText.push_back(aCharacter);
    }
    
    text = "MISSION SUCCESSFUL";
    startInd = -(float)text.size()/2.0 * textSize; // Left most position for first character if text was centered
    for (int i = 0; i < text.size(); i++) {
        Entity aCharacter = Entity(startInd + textSize*i, textSize/2, textSize, textSize, font);
        aCharacter.cols = 16;
        aCharacter.rows = 16;
        aCharacter.index = (int)text[i];
        state.successText.push_back(aCharacter);
    }
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -2.81f, 2.81f, -1.0f, 1.0f);

    programTex.SetProjectionMatrix(projectionMatrix);
    programTex.SetViewMatrix(viewMatrix);
    programTex.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glUseProgram(programTex.programID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float r = 41;
    float g = 46;
    float b = 43;
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
    
    if (mode == GAME_LEVEL) { // Able to keep changing velocity/acceleration as long as we are playing (didn't collide)
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        // Horizontal accelerations are limited by -1 & 1 so the added velocitiy will never be a negative y
        if (keys[SDL_SCANCODE_LEFT] && state.spacecraft.acceleration.x > -1) { // Left acceleration
            state.spacecraft.acceleration.x -= 0.01f;
            state.spacecraft.acceleration.y = 1.0f - abs(state.spacecraft.acceleration.x); // Change the y accordingly in proportion to the x
        }
        else if (keys[SDL_SCANCODE_RIGHT] && state.spacecraft.acceleration.x < 1) { // Right acceleration
            state.spacecraft.acceleration.x += 0.01f;
            state.spacecraft.acceleration.y = 1.0f - abs(state.spacecraft.acceleration.x); // Change the y accordingly in proportion to the x
        }
        if (keys[SDL_SCANCODE_UP]) { // Add into velocity
            state.spacecraft.thruster = true;
        }
    }
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;


void UpdateGameLevel() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        state.spacecraft.Update(FIXED_TIMESTEP, state.terrain, sizeof(state.terrain)/sizeof(state.terrain[0]));
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    if (state.spacecraft.collided) { // If we've collided, change the mode based on if we landed or crashed
        if (state.spacecraft.isSuccess) mode = GAME_SUCESS;
        else mode = GAME_OVER;
    }
}

void Update() { // Used to keep updating movement unless game has ended
    switch (mode) {
        case GAME_LEVEL:
            UpdateGameLevel();
            break;
            
        case GAME_OVER:
        case GAME_SUCESS:
            break;
    }
}


void RenderGameOver() {
    for (size_t i = 0; i < state.failText.size(); i++) {
        state.failText[i].DrawText(&programTex);
    }
}

void RenderGameSuccess() {
    for (size_t i = 0; i < state.successText.size(); i++) {
        state.successText[i].DrawText(&programTex);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    state.spacecraft.Render(&programTex);
    for (size_t i = 0; i < sizeof(state.terrain)/sizeof(state.terrain[0]); i++) {
        state.terrain[i].Render(&programTex);
    }
    switch (mode) {
        case GAME_LEVEL: // There is no GAME_LEVEL specific render as we want to render everything even if game ends
            break;
        case GAME_OVER:
            RenderGameOver();
            break;
        case GAME_SUCESS:
            RenderGameSuccess();
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
