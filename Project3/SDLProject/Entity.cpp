#include "Entity.h"

Entity::Entity() {}

Entity::Entity(float x, float y, float width_, float height_, GLuint textureID_) {    
    position = glm::vec3(x, y, 0.0f);
    width = width_;
    height = height_;
    textureID = textureID_;
}

void Entity::Update(float deltaTime, Entity *objects, int objectCount)
{
    velocity += gravity * deltaTime;
    
    if (thruster) { // User pressed up to activate the thruster/booster
        velocity += glm::normalize(acceleration) / 4.0f * deltaTime;
        thruster = false; // Turn off thruster/booster
    }
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(objects, objectCount);
    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(objects, objectCount);
    
    CheckInBound();
}

void Entity::Render(ShaderProgram *program) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
    // Rotate - mainly used for spacecraft to indicate the current horizontal acceleration
    modelMatrix = glm::rotate(modelMatrix, glm::radians(acceleration.x * -90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    
    program->SetModelMatrix(modelMatrix);
    
    float vertices[]  = {
        -width/2, -height/2,
        width/2, -height/2,
        width/2, height/2,
        -width/2, -height/2,
        width/2, height/2,
        -width/2, height/2 };
    
    float texCoords[] = {
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

bool Entity::CheckCollision(Entity other) {
    float xdist = fabs(position.x - other.position.x) - ((width + other.width) / 2.0f);
    float ydist = fabs(position.y - other.position.y) - ((height + other.height) / 2.0f);
    return (xdist < 0 && ydist < 0);
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity object = objects[i];
        if (CheckCollision(object)) {
            collided = true; // We've collided
            CheckMission(object); // Check if we crashed or landed safetly
            float ydist = fabs(position.y - object.position.y);
            float penetrationY = fabs(ydist - (height / 2) - (object.height / 2));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
            }
            break;
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity object = objects[i];
        if (CheckCollision(object)) {
            collided = true; // We've collided
            isSuccess = false; // We crashed since we ran into a wall (There was a horizontal collision)
            float xdist = fabs(position.x - object.position.x);
            float penetrationX = fabs(xdist - (width / 2) - (object.width / 2));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
            }
            break;
        }
    }
}

// Check if object is within the bounds of the screen, deal accordingly
// Wrap the left/right borders
// Cap the horizontally border - cannot go higher
void Entity::CheckInBound() {
    if (position.x > 5.0 + width/2) position.x = -5.0 - width/2;
    else if (position.x < -5.0 - width/2) position.x = 5.0 + width/2;
    
    if (position.y > 2.81 - height) position.y = 2.81 - height;
}

void Entity::CheckMission(Entity other) { // Determine if the mission was successful or not
    if (position.x - width/2 > other.position.x - other.width/2 && // Spacecraft within the collided object's leftmost x
        position.x + width/2 < other.position.x + other.width/2 && // Spacecraft within the collided object's rightmost x
        acceleration.x > -0.35 && // Spacecraft isn't tilted too much left
        acceleration.x < 0.35 && // Spacecraft isn't tilted too much to the right
        velocity.y > -0.2f) isSuccess = true; // Negative-y velocity isn't too fast
}

void Entity::DrawText(ShaderProgram *program) { // Drawing the text onto the screen
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
    
    
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    
    float w = 1.0f / (float)cols;
    float h = 1.0f / (float)rows;
    
    float texCoords[] = {
        u, v+h,
        u+w, v+h,
        u+w, v,
        u, v+h,
        u+w, v,
        u, v};
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
