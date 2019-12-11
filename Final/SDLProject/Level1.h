#include "Scene.h"
#include <random>

class Level1 : public Scene {
public:
    int enemyIndex;
    int pBulletIndex;
    int eBulletIndex;
    
    
    
    float lastClear;
    float lastSpawn;
    
    bool boss;
    
    bool clear;
    int wave;
    int nextSpawn;
    
    bool tutorialDone;
    int tutorialCount;
    
    int spawnCount;
    int spawnMax;
    
    
    
    
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
    void Fire() override;
    
    void EnemyFire();
    
    void SpawnEnemies();
    void SpawnStraight();
    void SpawnVertical();
    void SpawnSeek();
    void SpawnShoot();
    void SpawnMix();
    void SpawnBoss();
    
    void CheckClear();
    void DisplayWave(ShaderProgram *program);
    void DisplayGameCondition(ShaderProgram *program);
    void DisplayKillCount(ShaderProgram *program);
    
    void CheckEnemyAlive();
    
    void Tutorial();
    
    void CheckBullets();
    
    void CheckGameCondition();
    
};
