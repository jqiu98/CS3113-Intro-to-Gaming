#include "Entity.h"

Entity::Entity()
{
    entityType = PLATFORM;
    isStatic = true;
    isActive = true;
    position = glm::vec3(0);
    width = 1;
    height = 1;
    direction = 1;
}

bool Entity::CheckCollision(Entity *other)
{
    if (isStatic == true) return false;
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
        // For bullets that it platforms/terrain
        if ((entityType == PLAYER_BULLET || entityType == ENEMY_BULLET) && other->entityType == PLATFORM) {
            isActive = false;
        }
        
        // For every collision not involving platforms/terrain
        else if (other->entityType != PLATFORM ) {
            isActive = false;
            other->isActive = false;
        }
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionsY(Entity *platforms, int platformCount, Entity *others, int otherCount)
{
    for (int i = 0; i < platformCount; i++)
    {
        if (CheckCollision(&platforms[i]))
        {
            float ydist = fabs(position.y - platforms[i].position.y);
            float penetrationY = fabs(ydist - (height / 2) - (platforms[i].height / 2));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
    for (int i = 0; i < otherCount; i++) CheckCollision(&others[i]); // Checking against non-platforms
}

void Entity::CheckCollisionsX(Entity *platforms, int platformCount, Entity *others, int otherCount)
{
    for (int i = 0; i < platformCount; i++)
    {
        if (CheckCollision(&platforms[i]))
        {
            float xdist = fabs(position.x - platforms[i].position.x);
            float penetrationX = fabs(xdist - (width / 2) - (platforms[i].width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                collidedLeft = true;
            }
            if (entityType == ENEMY) velocity.x *= -1.0f;
            else velocity.x = 0;
        }
    }
    
    for (int i = 0; i < otherCount; i++) CheckCollision(&others[i]); // Checking against non-platforms
}


void Entity::Jump()
{
    if (collidedBottom)
    {
        if (entityType == PLAYER) velocity.y = 5.0f;
        else velocity.y = 9.0f; // Enemies have higher jump
    }
}


void Entity::Update(float deltaTime)
{
    if (entityType == ENEMY && ID == 0) Jump(); // Jumping Enemies
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
//    CheckCollisionsY(platforms, platformCount, others, otherCount);
    
    position.x += velocity.x * deltaTime;
//    CheckCollisionsX(platforms, platformCount, others, otherCount);
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

