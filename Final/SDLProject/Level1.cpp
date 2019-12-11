#include "Level1.h"

//#define MENU_WIDTH 6
//#define MENU_HEIGHT 2

int GenRandom(int a, int b) {
    std::random_device randDev;
    std::mt19937 gen(randDev());
    std::uniform_real_distribution<> distr(a, b);
    return distr(gen);
}

void Level1::Initialize() {
    enemyIndex = 0;
    pBulletIndex = 0;
    eBulletIndex = 0;
    spawnCount = 0;
    spawnMax = 50;
    
    lastClear = (float)SDL_GetTicks() / 1000.0f;
    
    boss = false;
    
    clear = true;
    wave = 1;
    nextSpawn = 2;
    lastSpawn = 0;
    tutorialDone = false;
    tutorialCount = 1;
}

void Level1::Fire() {
    float timeNow = (float)SDL_GetTicks() / 1000.0f;

    float deltaTime = timeNow - state.player.lastFire;
    state.player.lastFire = timeNow;
    state.player.fireAccumulator += deltaTime;

    if (state.player.fireAccumulator > FIXED_PLAYER_FIRE_RATE) {
        state.player.fireAccumulator = 0.0f;
        
        if (!state.player_bullets[pBulletIndex].isActive) { // Check if we are out of bullets, if not then create
            state.player_bullets[pBulletIndex].isActive = true;
            state.player_bullets[pBulletIndex].position.x = state.player.position.x;
            state.player_bullets[pBulletIndex].position.y = state.player.position.y + state.player.height/2 + 0.2f;
            state.player_bullets[pBulletIndex].velocity.y = 7.0f;
        }
        pBulletIndex = (pBulletIndex+1) % P_MAX_BULLETS;
    }
}

void Level1::EnemyFire() {
    float timeNow = (float)SDL_GetTicks() / 1000.0f;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state.enemies[i].isActive && state.enemies[i].shooter) {
            
            float deltaTime = timeNow - state.enemies[i].lastFire;
            state.enemies[i].lastFire = timeNow;
            state.enemies[i].fireAccumulator += deltaTime;
            
            if (state.enemies[i].fireAccumulator > FIXED_ENEMY_FIRE_RATE) {
                state.enemies[i].fireAccumulator = 0.0f;
                
                if (!state.enemy_bullets[eBulletIndex].isActive) {
                    state.enemy_bullets[eBulletIndex].isActive = true;
                    state.enemy_bullets[eBulletIndex].position.x = state.enemies[i].position.x;
                    state.enemy_bullets[eBulletIndex].position.y = state.enemies[i].position.y - state.enemies[i].height/2 - 0.2f;
                    state.enemy_bullets[eBulletIndex].velocity.y = -7.0f;
                }
                eBulletIndex = (eBulletIndex+1) % E_MAX_BULLETS;
            }
        }
    }
}

void Level1::CheckBullets() {
    for (int i = 0; i < P_MAX_BULLETS; i++) {
        if  (state.player_bullets[i].isActive &&
             state.player_bullets[i].position.y > 5.25) state.player_bullets[i].isActive = false;
    }
    for (int i = 0; i < E_MAX_BULLETS; i++) {
        if  (state.enemy_bullets[i].isActive &&
             state.enemy_bullets[i].position.y > 5.25) state.enemy_bullets[i].isActive = false;
    }
}

void Level1::SpawnStraight() {
    
    int xPos = GenRandom(-6.5, 6.5);
    int enemyCount = GenRandom(3, 8);
    
    bool toSeek = (GenRandom(0, 6) == 2);
    bool toShoot = (GenRandom(0, 6) ==  2);

    if (!tutorialDone) {
        clear = false;
        enemyCount = 5;
        spawnCount -= 5;
        toSeek = false;
        toShoot = false;
    }
//    if (spawnCount + enemyCount > spawnMax) enemyCount = spawnMax - spawnCount;
    

    for (int i = 0; i < enemyCount; i++) {
        state.enemies[enemyIndex].isActive = true;
        state.enemies[enemyIndex].seeker = toSeek;
        state.enemies[enemyIndex].shooter = toShoot;
        state.enemies[enemyIndex].position = glm::vec3(xPos, 6.25f+(i*1.5), 0);
        state.enemies[enemyIndex].velocity = glm::vec3(0, -3.0f, 0);
        enemyIndex = (enemyIndex + 1) % MAX_ENEMIES;
    }
}

void Level1::SpawnVertical() {
    int enemyCount = GenRandom(3, 8);
    bool toSeek = (GenRandom(0, 6) == 2);
    bool toShoot = (GenRandom(0, 6) ==  2);
    
    if (!tutorialDone) {
        clear = false;
        enemyCount = 9;
        spawnCount -= 9;
        toSeek = false;
        toShoot = false;
    }
//    if (spawnCount + enemyCount > spawnMax) enemyCount = spawnMax - spawnCount;
    int xPos = GenRandom(-6.5, 6.5 - 1.5*enemyCount);
    for (int i = 0; i < enemyCount; i++) {
        state.enemies[enemyIndex].isActive = true;
        state.enemies[enemyIndex].seeker = toSeek;
        state.enemies[enemyIndex].shooter = toShoot;
        state.enemies[enemyIndex].position = glm::vec3(xPos + i*1.5, 6.25f, 0);
        state.enemies[enemyIndex].velocity = glm::vec3(0, -3.0f, 0);
        enemyIndex = (enemyIndex + 1) % MAX_ENEMIES;
    }
    return;
}

void Level1::SpawnSeek() {
    if (!tutorialDone) {
        clear = false;
    }
    int xPos = GenRandom(-6.5, 6.5);
    state.enemies[enemyIndex].isActive = true;
    state.enemies[enemyIndex].seeker = true;
    state.enemies[enemyIndex].shooter = false;
    state.enemies[enemyIndex].position = glm::vec3(xPos, 6.25f, 0);
    state.enemies[enemyIndex].velocity = glm::vec3(0, -3.0f, 0);
    enemyIndex = (enemyIndex + 1) % MAX_ENEMIES;
}

void Level1::SpawnShoot() {
    if (!tutorialDone) {
        clear = false;
    }
    int xPos = GenRandom(-6.5, 6.5);
    state.enemies[enemyIndex].isActive = true;
    state.enemies[enemyIndex].seeker = false;
    state.enemies[enemyIndex].shooter = true;
    state.enemies[enemyIndex].position = glm::vec3(xPos, 6.25f, 0);
    state.enemies[enemyIndex].velocity = glm::vec3(0, -3.0f, 0);
    enemyIndex = (enemyIndex + 1) % MAX_ENEMIES;
}

void Level1::SpawnMix() {
    int xPos = GenRandom(-6.5, 6.5);
    state.enemies[enemyIndex].isActive = true;
    state.enemies[enemyIndex].seeker = true;
    state.enemies[enemyIndex].shooter = true;
    state.enemies[enemyIndex].position = glm::vec3(xPos, 6.25f, 0);
    state.enemies[enemyIndex].velocity = glm::vec3(0, -3.0f, 0);
    enemyIndex = (enemyIndex + 1) % MAX_ENEMIES;
}

void Level1::SpawnBoss() {
    state.enemies[enemyIndex].isActive = true;
    state.enemies[enemyIndex].boss = true;
    state.enemies[enemyIndex].shooter = true;
    state.enemies[enemyIndex].life = 10;
    state.enemies[enemyIndex].width = 1.5;
    state.enemies[enemyIndex].height = 1.5;
    state.enemies[enemyIndex].position = glm::vec3(0, 4.0f, 0);
    state.enemies[enemyIndex].velocity.x = 3;
    boss = true;
}

void Level1::SpawnEnemies() {
    if (!tutorialDone) {
        Tutorial();
        CheckClear();
        return;
    }
    if (state.player.killCount == spawnMax) {
        CheckClear();
        if (clear && !boss) SpawnBoss();
    }
    else {
        float timeNow = (float)SDL_GetTicks() / 1000.0f;
        float timeDiff = timeNow - lastSpawn;
        if (timeDiff > nextSpawn) {
            lastSpawn = timeNow;
            int spawnPicker = GenRandom(0, 4);
            
            switch (spawnPicker) {
                case 0:
                    SpawnStraight();
                    nextSpawn = GenRandom(1, 3);
                    break;
                case 1:
                    SpawnVertical();
                    nextSpawn = GenRandom(1, 3);
                    break;
                case 2:
                    SpawnSeek();
                    nextSpawn = GenRandom(1, 2);
                    break;
                case 3:
                    SpawnShoot();
                    nextSpawn = GenRandom(1, 2);
                    break;
                case 4:
                    SpawnMix();
                    nextSpawn = GenRandom(1, 2);
                    break;
            }
        }
    }
} 

void Level1::Tutorial() {
    float timeNow = (float)SDL_GetTicks() / 1000.0f;
    float timeDiff = timeNow - lastClear;
    if (clear && timeDiff > 2) {
        switch (tutorialCount++) {
            case 1:
            case 2:
                SpawnStraight();
                break;
            case 3:
            case 4:
                SpawnVertical();
                break;
            case 5:
            case 6:
                SpawnSeek();
                break;
            case 7:
            case 8:
                SpawnShoot();
                break;
            case 9:
                tutorialDone = true;
                clear = false;
                wave = 1;
                state.player.killCount = 0;
        }
    }
}

void Level1::CheckEnemyAlive() {
    if (!clear) {
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (state.enemies[i].isActive && state.enemies[i].position.y < -6.25f) {
                state.enemies[i].isActive = false;
            }
        }
    }
}

void Level1::CheckClear() {
    if (clear) return;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state.enemies[i].isActive) return;
    }
    lastClear = (float)SDL_GetTicks() / 1000.0f;
    clear = true;
    wave++;
}

void Level1::CheckGameCondition() {
    if (boss) {
        if (!state.enemies[enemyIndex].isActive) {
            state.gameWon = true;
            state.player.isActive = false;
        }
    }
}

void Level1::Update(float deltaTime) {
    
    if (state.player.isActive) {
        EnemyFire();
        SpawnEnemies();
        CheckBullets();
        
        state.player.Update(deltaTime, 0, state.enemies, MAX_ENEMIES);
        for (int i = 0; i < P_MAX_BULLETS; i++) {
            if (state.player_bullets[i].isActive) {
                state.player_bullets[i].Update(deltaTime, 0, 0, 0);
            }
        }
        
        for (int i = 0; i < E_MAX_BULLETS; i++) {
            if (state.enemy_bullets[i].isActive) {
                state.enemy_bullets[i].Update(deltaTime, 0, &state.player, 1);
            }
        }
        
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (state.enemies[i].isActive) {
                state.enemies[i].Update(deltaTime, &state.player, state.player_bullets, P_MAX_BULLETS);
            }
        }
        CheckEnemyAlive();
        CheckGameCondition();
    }
}




void Level1::DisplayWave(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");

    string text;
    text = "TUTORIAL WAVE ";

    Util::DrawText(program, fontTextureID, text + std::to_string(wave), 0.8f, -0.2f, glm::vec3(0, 2, 0));
}

void Level1::DisplayGameCondition(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");

    string text;
    if (state.gameWon) text = "YOU WON!";
    else text = "YOU LOSE!";
    
    Util::DrawText(program, fontTextureID, text, 0.8f, -0.2f, glm::vec3(0, 0, 0));
}

void Level1::DisplayKillCount(ShaderProgram *program) {
    GLuint fontTextureID = Util::LoadTexture("font1.png");
    
    Util::DrawText(program, fontTextureID, "Kills: " + std::to_string(state.player.killCount), 0.4f, -0.2f, glm::vec3(5, -4.5, 0));
    
}

void Level1::Render(ShaderProgram *program) {
    
    if (!tutorialDone && clear) DisplayWave(program);
    
    GLuint healthTexture = Util::LoadTexture("playerShip.png");
    for (int i = 0; i < state.player.life; i++) {
        glm::vec3 position = glm::vec3(-7.0f + 0.75 + i, -4.5, 0);
        Util::RenderObject(program, healthTexture, 0.5f, 0.5f, position);
    }
    
    DisplayKillCount(program);
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state.enemies[i].isActive) {
            state.enemies[i].Render(program);
        }
    }
    
    state.player.Render(program);
    
    for (int i = 0; i < P_MAX_BULLETS; i++) {
        if (state.player_bullets[i].isActive) state.player_bullets[i].Render(program);
    }
    
    for (int i = 0; i < E_MAX_BULLETS; i++) {
        if (state.enemy_bullets[i].isActive) state.enemy_bullets[i].Render(program);
    }
    
    if (!state.player.isActive || state.gameWon) DisplayGameCondition(program);
}
