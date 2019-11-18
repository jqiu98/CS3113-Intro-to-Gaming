#include "Entity.h"

Entity::Entity()
{
    isStatic = true;
    isActive = false;
    position = glm::vec3(0);
    speed = 0;
    width = 1;
    height = 1;
    
    animTime = 0;
    animIndex = 0;
    direction = 0;
}

bool Entity::CheckCollision(Entity *other)
{
    if (isStatic == true) return false;
    if (isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0)
    {
        if (entityType == PLAYER && other->entityType == COIN)
        {
            other->isActive = false;
        }
        
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity object = objects[i];
        
        if (CheckCollision(&objects[i]))
        {
            if (entityType == PLAYER && object.entityType == COIN) continue;
            
            float ydist = fabs(position.y - object.position.y);
            float penetrationY = fabs(ydist - (height / 2) - (object.height / 2));
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
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++)
    {
        Entity object = objects[i];
        
        if (CheckCollision(&objects[i]))
        {
            if (entityType == PLAYER && object.entityType == COIN) continue;
            
            float xdist = fabs(position.x - object.position.x);
            float penetrationX = fabs(xdist - (width / 2) - (object.width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::CheckCollisionsY(Map *map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if ((map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) ||
            (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) ||
            (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0))
    {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }


    if ((map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) ||
        (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) ||
        (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0))
    {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    
    if(map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}

void Entity::Jump()
{
    if (collidedBottom)
    {
        velocity.y = 10.0f;
    }
}




void Entity::Update(float deltaTime, Entity *coins, int coinCount, Map *map)
{
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;        // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(coins, coinCount);    // Fix if needed

    
    position.x += velocity.x * deltaTime;        // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(coins, coinCount);    // Fix if needed

    
    animTime += deltaTime;
    if (animTime >= 0.25f) {
        animTime = 0;
        animIndex++;
        if (animIndex >= animFrames) animIndex = 0;
    }
}


void Entity::RenderAnim(ShaderProgram *program) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    if (entityType == COIN) {
    }
    float u, v;
    switch (direction) {
        case -1:
            u = (float)(animLeftIndices[animIndex] % cols) / (float)cols;
            v = (float)(animLeftIndices[animIndex] / cols) / (float)rows;
            break;
        case 1:
            u = (float)(animRightIndices[animIndex] % cols) / (float)cols;
            v = (float)(animRightIndices[animIndex] / cols) / (float)rows;
            break;
        case 0:
            u = (float)(0 % cols) / (float)cols;
            v = (float)(0 / cols) / (float)rows;
            break;
    }
    
    float width_ = 1.0f / (float)cols;
    float height_ = 1.0f / (float)rows;
    
    float texCoords[] = { u, v + height_, u + width_, v + height_, u + width_, v,
        u, v + height_, u + width_, v, u, v};
    
    float vertices[]  = { -width/2, -height/2, width/2, -height/2, width/2, height/2, -width/2, -height/2, width/2, height/2, -width/2, height/2 };

    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    if (entityType == PLAYER || entityType == COIN) {
        RenderAnim(program);
        return;
    }
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    float vertices[]  = { -width/2, -height/2, width/2, -height/2, width/2, height/2, -width/2, -height/2, width/2, height/2, -width/2, height/2 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
