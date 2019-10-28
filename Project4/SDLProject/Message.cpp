#include "Message.h"

Message::Message() {}

void Message::DrawText(ShaderProgram *program) { // Drawing the text onto the screen
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
