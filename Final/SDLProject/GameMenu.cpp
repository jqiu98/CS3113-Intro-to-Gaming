#include "GameMenu.h"

//#define MENU_WIDTH 6
//#define MENU_HEIGHT 2


unsigned int menu_data[] =
{
    0, 0, 0, 7, 7, 0,
    1, 1, 1, 1, 1, 1
};


void GameMenu::Initialize() {
//    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);
    state.gameWon = false;
    
    state.player.entityType = PLAYER;
    state.player.life = 3;
    state.player.killCount = 0;
    state.player.isActive = true;
    state.player.textureID = Util::LoadTexture("playerShip.png");
    
    GLuint enemeyTexture = Util::LoadTexture("enemyBlack1.png");
    for (int i = 0; i < MAX_ENEMIES; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemeyTexture;
    }
    
    GLuint playerBulletTextureID = Util::LoadTexture("playerLaser.png");
    GLuint enemyBulletTextureID = Util::LoadTexture("laserRed03.png");
    
    // Initialize player bullets
    for (int i = 0; i < P_MAX_BULLETS; i++) {
        state.player_bullets[i].width = 0.1f;
        state.player_bullets[i].height = 0.5f;
        state.player_bullets[i].entityType = PLAYER_BULLET;
        state.player_bullets[i].textureID = playerBulletTextureID;
    }
    
    for (int i = 0; i < E_MAX_BULLETS; i++) {
        state.enemy_bullets[i].width = 0.1f;
        state.enemy_bullets[i].height = 0.5f;
        state.enemy_bullets[i].entityType = ENEMY_BULLET;
        state.enemy_bullets[i].textureID = enemyBulletTextureID;
    }
}


void GameMenu::Update(float deltaTime) {}

void GameMenu::Render(ShaderProgram *program) {
    state.player.Render(program);
    GLuint fontTextureID = Util::LoadTexture("font1.png");

    Util::DrawText(program, fontTextureID, "Shoot Till You Die", 0.8f, -0.4f, glm::vec3(0, 2, 0));
    Util::DrawText(program, fontTextureID, "ENTER TO START", 0.5f, -0.2f, glm::vec3(0, 0.5, 0));
}

void GameMenu::Fire() {};
