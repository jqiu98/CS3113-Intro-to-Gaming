#define GL_SILENCE_DEPRECATION

#include "Player.h"

Player::Player(float x, float y, int c) {
    position = glm::vec3(x, y, 0);
    movement = glm::vec3(0, 0, 0);
    speed = 5;
    colorMode = c;
    
}

void Player::Update(float deltaTime) {
    position += movement * speed * deltaTime;
}

void Player::Render(ShaderProgram *program) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    if (colorMode == 1) program->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    else program->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
    
    float vertices[] = {
        -0.15, 0.75,
        0.15, 0.75,
        0.15, -0.75,
        -0.15, -0.75};
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glDrawArrays(GL_QUADS, 0, 4);
    
    glDisableVertexAttribArray(program->positionAttribute);
}
