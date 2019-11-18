#include "Level3.h"

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
{
    3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,
    3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 0, 0, 0, 9, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 0, 0, 0, 9, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 9, 9, 9, 0,
    3, 3, 0, 0, 9, 0, 0, 0, 1, 1, 2, 0, 0, 2, 9, 0, 0, 0, 0, 0, 1,
    3, 3, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0, 2, 9, 9, 1, 1, 0, 0, 0,
    3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 1, 1, 2, 2, 1, 0, 0,
    3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 1, 0,
    3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 0
};

void Level3::Initialize() {
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    
    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    state.player.isActive = true;
    state.player.position = glm::vec3(3, 6, 0);
    state.player.acceleration = glm::vec3(0, -22.0f, 0);
    state.player.textureID = Util::LoadTexture("george.png");
    state.player.cols = 4;
    state.player.rows = 4;
    state.player.animFrames = 4;
    state.player.animLeftIndices = new int[4] {1, 5, 9, 13};
    state.player.animRightIndices = new int[4] {3, 7, 11, 15};
    state.nextLevel = -1;
    
    GLuint coinTextureID = Util::LoadTexture("coin.png");
    int levelCoins = 0;
    
    for (int y = 0; y < LEVEL3_HEIGHT; y++) {
        for (int x = 0; x < LEVEL3_WIDTH; x++) {
            if (level3_data[y * LEVEL3_WIDTH + x] == 9) {
                state.coins[levelCoins].entityType = COIN;
                state.coins[levelCoins].isStatic = false;
                state.coins[levelCoins].isActive = true;
                state.coins[levelCoins].position = glm::vec3(x, -y, 0);
                state.coins[levelCoins].width = 0.5;
                state.coins[levelCoins].height = 0.5;
                state.coins[levelCoins].textureID = coinTextureID;
                state.coins[levelCoins].cols = 6;
                state.coins[levelCoins].rows = 1;
                state.coins[levelCoins].animFrames = 6;
                state.coins[levelCoins].animRightIndices = new int[6] {0, 1, 2, 3, 4, 5};
                state.coins[levelCoins].direction = 1;
                levelCoins++;
            }
        }
    }
}

void Level3::Update(float deltaTime) {
    state.player.Update(deltaTime, state.coins, MAX_COIN_COUNT, state.map);
    if (GameOver == 0) {
        for (int i = 0; i < MAX_COIN_COUNT; i++) {
            if (state.coins[i].isActive) state.coins[i].Update(deltaTime, NULL, 0, state.map);
        }
        
        if (state.player.position.y < -15) GameOver = -1;
    }
    
    bool coinsLeft = false;
    for (int i = 0; i < MAX_COIN_COUNT; i++) {
        if(state.coins[i].isActive) {
            coinsLeft = true;
            break;
        }
    }
    if (!coinsLeft) GameOver = 1;
    
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player.Render(program);
    
    for (int i = 0; i < MAX_COIN_COUNT; i++) {
        if (state.coins[i].isActive) state.coins[i].Render(program);
    }
    
    if (GameOver == -1) {
        GLuint fontTextureID = Util::LoadTexture("font1.png");
        Util::DrawText(program, fontTextureID, "YOU LOSE", 1.0f, -0.5f, glm::vec3(state.player.position.x -2, state.player.position.y + 2, 0));
    }
    else if (GameOver == 1) {
        GLuint fontTextureID = Util::LoadTexture("font1.png");
        Util::DrawText(program, fontTextureID, "YOU WON", 1.0f, -0.5f, glm::vec3(state.player.position.x -2, state.player.position.y + 2, 0));
    }
}