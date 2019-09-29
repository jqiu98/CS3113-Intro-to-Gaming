#define GL_SILENCE_DEPRECATION

#include "Ball.h"
#include <random>

Ball::Ball() {
    position = glm::vec3(0, 0, 0);
    movement = glm::vec3(0.75f, 0, 0);
    speed = 4 ;
    colorMode = 0;
    
    // Generates a random number - Code taken from stackoverflow
    std::random_device randDev;
    std::mt19937 gen(randDev());
    std::uniform_real_distribution<> distr(-1.0f, 1.0f); // Define the range
    
    movement.y = distr(gen);
    if (glm::length(movement) > 1.0f) {
        movement = glm::normalize(movement);
    }
}

void Ball::Update(float deltaTime) {
    position += movement * speed * deltaTime;
}

void Ball::Render(ShaderProgram *program) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    // Color changes based on who hits the ball
    if (colorMode == 1) program->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    else if (colorMode == 2) program->SetColor(0.0f, 0.0f, 1.0f, 1.0f);
    else program->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
    
    float vertices[] = {
        -0.15, 0.15,
        0.15, 0.15,
        0.15, -0.15,
        -0.15, -0.15};
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glDrawArrays(GL_QUADS, 0, 4);
    
    glDisableVertexAttribArray(program->positionAttribute);
}
