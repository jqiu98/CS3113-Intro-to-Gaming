#include "Entity.h"

Entity::Entity()
{
    entityType = PLAYER;
    isStatic = false;
    isActive = false;
    position = glm::vec3(0);
    width = 1;
    height = 1;
    life = 1;
}

bool Entity::CheckCollision(Entity *other)
{
    if (isStatic == true) return false;
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
        if (entityType == PLAYER && other->entityType == ENEMY) {
            life--;
            other->life = 0;
            other->UpdateAlive();
        }
        else if ((entityType == PLAYER && other->entityType == ENEMY_BULLET) ||
                 (entityType == ENEMY && other->entityType == PLAYER_BULLET)) {
            life--;
            other->isActive = false;
            
        }
        else if ((entityType == ENEMY_BULLET && other->entityType == PLAYER) ||
                 (entityType == PLAYER_BULLET && other->entityType == ENEMY)) {
            isActive = false;
            other->life--;
            other->UpdateAlive();
        }
        return true;
    }
    return false;
}

void Entity::CheckCollisionsY(Entity *player, Entity *others, int otherCount)
{
    for (int i = 0; i < otherCount; i++)
    {
        if (&others[i] == this) continue;
        if (CheckCollision(&others[i])) {
            if (others[i].entityType == PLAYER_BULLET && life == 0) {
                player->killCount += 1;
            }
             if (entityType == others[i].entityType) {
                float ydist = fabs(position.y - others[i].position.y);
                float penetrationY = fabs(ydist - (height / 2) - (others[i].height / 2));
                if (velocity.y > 0) {
                    position.y -= penetrationY;
                    velocity.y = 0;
                }
                else if (velocity.y < 0) {
                    position.y += penetrationY;
                    velocity.y = 0;
                }
             }
        }
    }
}

void Entity::CheckCollisionsX(Entity *player, Entity *others, int otherCount)
{
    for (int i = 0; i < otherCount; i++)
    {
        if (&others[i] == this) continue;
        if (CheckCollision(&others[i])) {
            if (others[i].entityType == PLAYER_BULLET && life == 0) player->killCount += 1;
            if (entityType == others[i].entityType) {
                float xdist = fabs(position.x - others[i].position.x);
                float penetrationX = fabs(xdist - (width / 2) - (others[i].width / 2));
                if (velocity.x > 0) {
                    position.x -= penetrationX;
                }
                else if (velocity.x < 0) {
                    position.x += penetrationX;
                }
            }
        }
    }
}


void Entity::UpdateAlive() {
    if (life < 1) {
        isActive = false;
    }
}

void Entity::UpdateEnemy(int playerX) {
    if (seeker) {
        if (playerX > position.x && velocity.x < 2) velocity.x += 0.4;
        else if (velocity.x > -2) velocity.x -= 0.4;
    }
    else if (boss) {
        if (position.x > 5.0) velocity.x *= -1;
        else if (position.x < -5) velocity.x *= -1;
    }
}


void Entity::Update(float deltaTime, Entity *player, Entity *others, int otherCount)
{
    if (entityType == ENEMY) UpdateEnemy(player->position.x);
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(player, others, otherCount);
    UpdateAlive();

    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(player, others, otherCount);
    UpdateAlive();
}




void Entity::Render(ShaderProgram *program) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = {
        -width/2, -height/2,
        width/2, -height/2,
        width/2, height/2,
        -width/2, -height/2,
        width/2, height/2,
        -width/2, height/2 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

