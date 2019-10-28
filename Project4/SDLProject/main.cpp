

/*
 =======================================================
 ====================[INSTRUCTIONS]=====================
 =======================================================
 ARROW KEY LEFT: Move left
 ARROW KEY RIGHT: Move right
 SPACEBAR: Jump
 LEFT/RIGHT SHIFT: Shoot bullets
 =======================================================
 =======================================================
 =======================================================
 */

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
#include "Message.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program, programUntexture;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

#define PLATFORM_COUNT 14
#define ENEMY_COUNT 3
#define P_MAX_BULLETS 6
#define E_MAX_BULLETS 3
using namespace std;



struct GameState {
    Entity player; // player.isActive will be used to see if the game has ended or not throughout the code
    Entity platforms[PLATFORM_COUNT];
    Entity enemies[ENEMY_COUNT];
    Entity player_bullets[P_MAX_BULLETS];
    Entity enemy_bullets[E_MAX_BULLETS];
    vector<Message> gameOverText;
    vector<Message> gameWinText;
};

enum GameMode { GAME_LEVEL, GAME_OVER, GAME_WIN };
GameMode mode = GAME_LEVEL;

GameState state;
int pNextBullet = 0;
int eNextBullet = 0;
int enemiesLeft = ENEMY_COUNT;

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
    displayWindow = SDL_CreateWindow("AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    state.player.position = glm::vec3(-4, 3, 0);
    state.player.acceleration = glm::vec3(0, -9.81f, 0);
    state.player.textureID = LoadTexture("me.png");
    
    // Map the textures
    GLuint dirtTextureID = LoadTexture("dirt.png");
    GLuint grassTextureID = LoadTexture("grass.png");
    GLuint enemyTextureID = LoadTexture("evil.png");
    GLuint playerBulletTextureID = LoadTexture("playerLaser.png");
    GLuint enemyBulletTextureID = LoadTexture("enemyLaser.png");
    GLint fontTextureID = LoadTexture("font.png");
    
    // Initialize terrain
    for (int i = 0; i < 11; i++)
    {
        state.platforms[i].textureID = dirtTextureID;
        state.platforms[i].position = glm::vec3(i - 5.0f, -3.25f, 0);
    }
    
    state.platforms[11].textureID = grassTextureID;
    state.platforms[11].position = glm::vec3(-5.0f, -2.25f, 0);
    
    state.platforms[12].textureID = grassTextureID;
    state.platforms[12].position = glm::vec3(-4.0f, -2.25f, 0);

    state.platforms[13].textureID = grassTextureID;
    state.platforms[13].position = glm::vec3(4.5f, -2.25f, 0);
    
    // Initialize enemies
    for (int i = 0; i < 3; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID= enemyTextureID;
        state.enemies[i].isStatic = false;
        state.enemies[i].ID = i;
        state.enemies[i].direction = -1;

    }
    
    state.enemies[0].position = glm::vec3(4.5f, -1.25f, 0);
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    
    state.enemies[1].position = glm::vec3(3.5f, -2.25f, 0);
    
    state.enemies[2].position = glm::vec3(1.5f, -2.25f, 0);
    state.enemies[2].velocity.x = -1.5f;
    
    // Initialize player bullets
    for (int i = 0; i < P_MAX_BULLETS; i++) {
        state.player_bullets[i].isActive = false;
        state.player_bullets[i].isStatic = false;
        state.player_bullets[i].width = 0.5f;
        state.player_bullets[i].height = 0.1f;
        state.player_bullets[i].entityType = PLAYER_BULLET;
        state.player_bullets[i].textureID = playerBulletTextureID;
    }
    
    // Initialize enemy bullets
    for (int i = 0; i < E_MAX_BULLETS; i++) {
        state.enemy_bullets[i].isActive = false;
        state.enemy_bullets[i].isStatic = false;
        state.enemy_bullets[i].width = 0.5f;
        state.enemy_bullets[i].height = 0.1f;
        state.enemy_bullets[i].entityType = ENEMY_BULLET;
        state.enemy_bullets[i].textureID = enemyBulletTextureID;
    }
    
    // Preload the text into the vectors to be displayed later on
    float textSize = 0.5f; // Size of the text -> width & height
    
    string text = "GAME OVER";
    float startInd = -(float)text.size()/2.0 * textSize; // Left most position for first character if text was centered
    
    for (int i = 0; i < text.size(); i++) {
        Message aCharacter = Message();
        aCharacter.position = glm::vec3(startInd + textSize*i, textSize/2, 0.0f);
        aCharacter.width = textSize;
        aCharacter.height = textSize;
        aCharacter.textureID = fontTextureID;
        aCharacter.cols = 16;
        aCharacter.rows = 16;
        aCharacter.index = (int)text[i];
        state.gameOverText.push_back(aCharacter);
    }
    
    text = "YOU WIN";
    startInd = -(float)text.size()/2.0 * textSize; // Left most position for first character if text was centered
    for (int i = 0; i < text.size(); i++) {
        Message aCharacter = Message();
        aCharacter.position = glm::vec3(startInd + textSize*i, textSize/2, 0.0f);
        aCharacter.width = textSize;
        aCharacter.height = textSize;
        aCharacter.textureID = fontTextureID;
        aCharacter.cols = 16;
        aCharacter.rows = 16;
        aCharacter.index = (int)text[i];
        state.gameWinText.push_back(aCharacter);
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
        state.player_bullets[pNextBullet].position.x = state.player.position.x + 1.0f * state.player.direction;
        state.player_bullets[pNextBullet].position.y = state.player.position.y;
        state.player_bullets[pNextBullet].velocity.x = 3.0f * state.player.direction;
        pNextBullet++;
        if (pNextBullet == P_MAX_BULLETS) pNextBullet = 0;
    }
}

// Enemy fires a bullet
void EnemyFire() {
    // Check if enemy is dead or out of bullets, if not then create
    if (state.enemies[1].isActive && !state.enemy_bullets[eNextBullet].isActive) {
        state.enemy_bullets[eNextBullet].isActive = true;
        state.enemy_bullets[eNextBullet].position.x = state.enemies[1].position.x + 1.0f * state.enemies[1].direction;
        state.enemy_bullets[eNextBullet].position.y = state.enemies[1].position.y;
        state.enemy_bullets[eNextBullet].velocity.x = 3.0f * state.enemies[1].direction;
        eNextBullet++;
        if (eNextBullet == E_MAX_BULLETS) eNextBullet = 0;
    }
}

// Checking if bullets have went out of bound or touched terrain. If so, deactivate them
void CheckBullets() {
    for (int i = 0; i < P_MAX_BULLETS; i++) {
        if (state.player_bullets[i].isActive &&
            (state.player_bullets[i].position.x < -5.5f ||
             state.player_bullets[i].position.x > 5.5f)) state.player_bullets[i].isActive = false;
    }
    
    for (int i = 0; i < E_MAX_BULLETS; i++) {
        if (state.enemy_bullets[i].isActive &&
            (state.enemy_bullets[i].position.x < -5.5f ||
             state.enemy_bullets[i].position.x > 5.5f)) state.enemy_bullets[i].isActive = false;
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
            EnemyFire();
            fireAccumulator = 0.0f;
        }
        state.player.velocity.x = 0;
        
        // Check for pressed/held keys below
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (keys[SDL_SCANCODE_LEFT])
        {
            state.player.velocity.x = -3.0f;
            state.player.direction = -1.0f;
        }
        else if  (keys[SDL_SCANCODE_RIGHT])
        {
            state.player.velocity.x = 3.0f;
            state.player.direction = 1.0f;
        }
    }
}

// Check how many enemies are still alive in our game
void UpdateEnemies() {
    int count = 0;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].isActive) count ++;
    }
    enemiesLeft = count;
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void UpdateGameLevel() {
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
        state.player.Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            if (state.enemies[i].isActive) { // Only update if they are active
                // Enemy collision only checks against platforms, which is why count is set to 0.
                // Player vs Enemy collision is already checked above
                state.enemies[i].Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, &state.player, 0);
            }
        }
        
        for (int i = 0; i < P_MAX_BULLETS; i++) {
            if (state.player_bullets[i].isActive) { // Only update if they are active
                // Player bullets collision only checks against platforms & enemies
                state.player_bullets[i].Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
            }
        }
        
        for (int i = 0; i < E_MAX_BULLETS; i++) {
            if (state.enemy_bullets[i].isActive) { // Only update if they are active
                // Enemy bullets collision only checks against platforms & player
                state.enemy_bullets[i].Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, &state.player, 1);
            }
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    
    accumulator = deltaTime;
    UpdateEnemies();
    
    if (!state.player.isActive) mode = GAME_OVER; // Player is not active, meaning player has died
    else if (enemiesLeft == 0) mode = GAME_WIN; // No more enemies left, meaning player has won
}

void Update() { // Used to keep updating movement unless game has ended
    switch (mode) {
        case GAME_LEVEL:
            UpdateGameLevel();
            break;
            
        case GAME_OVER:
        case GAME_WIN:
            break;
    }
}

void RenderGameOver() {
    for (int i = 0; i < state.gameOverText.size(); i++) {
        state.gameOverText[i].DrawText(&program);
    }
}

void RenderGameWin() {
    for (int i = 0; i < state.gameWinText.size(); i++) {
        state.gameWinText[i].DrawText(&program);
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player.Render(&program);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].isActive) state.enemies[i].Render(&program);
    }
    
    for (int i = 0; i < P_MAX_BULLETS; i++) {
        if (state.player_bullets[i].isActive) state.player_bullets[i].Render(&program);
    }
    for (int i = 0; i < E_MAX_BULLETS; i++) {
        if (state.enemy_bullets[i].isActive) state.enemy_bullets[i].Render(&program);
    }
    switch (mode) {
        case GAME_LEVEL: // There is no GAME_LEVEL render, we want to render everything even if game ends
            break;
        case GAME_OVER:
            RenderGameOver();
            break;
        case GAME_WIN:
            RenderGameWin();
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
