#include "GameMenu.h"

#define MENU_WIDTH 6
#define MENU_HEIGHT 2


unsigned int menu_data[] =
{
    0, 0, 0, 9, 9, 0,
    1, 1, 1, 1, 1, 1
};


void GameMenu::Initialize() {
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);
    
    
    state.player.entityType = PLAYER;
    state.player.isStatic = false;
    state.player.isActive = true;
    state.player.position = glm::vec3(2, 0, 0);
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
    
    for (int y = 0; y < MENU_HEIGHT; y++) {
        for (int x = 0; x < MENU_WIDTH; x++) {
            if (menu_data[y * MENU_WIDTH + x] == 9) {
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

void GameMenu::Update(float deltaTime) {}

void GameMenu::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player.Render(program);
    for (int i = 0; i < MAX_COIN_COUNT; i++) {
        if (state.coins[i].isActive) state.coins[i].Render(program);
    }
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    Util::DrawText(program, fontTextureID, "SPACEBAR TO START", 1.0f, -0.5f, glm::vec3(state.player.position.x -4, state.player.position.y + 2, 0));
}
