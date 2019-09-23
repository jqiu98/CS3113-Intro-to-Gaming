#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DISPLAY_WIDTH 960
#define DISPLAY_HEIGHT 540

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program, program2;
glm::mat4 viewMatrix, projectionMatrix, cloudMatrix, cloudMatrix2, cloudMatrix3, sunMatrix, ufoMatrix, groundMatrix, mountainMatrix;

float cloud_x = 0.92;
float cloud2_x = 4.25;
float cloud3_x = 7.58;
float ufo_rotate = 0;

GLuint cloudTextureID, sunTextureID, ufoTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    program2.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    cloudTextureID = LoadTexture("cloud.png");
    cloudMatrix = glm::mat4(1.0f);
    cloudMatrix2 = glm::mat4(1.0f);
    cloudMatrix3 = glm::mat4(1.0f);
    
    sunTextureID = LoadTexture("sun.png");
    sunMatrix = glm::mat4(1.0f);
    
    ufoTextureID = LoadTexture("ufo.png");
    ufoMatrix = glm::mat4(1.0f);
    
    groundMatrix = glm::mat4(1.0f);
    mountainMatrix = glm::mat4(1.0f);
    

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -2.81f, 2.81f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    program2.SetProjectionMatrix(projectionMatrix);
    program2.SetViewMatrix(viewMatrix);
    program2.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(program.programID);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float r = 53.0;
    float g = 217.0;
    float b = 242.0;
    glClearColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float lastTicks = 0;
float rotate_z = 0;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    ufo_rotate += 300.0 * deltaTime;
    cloud_x += 1.5f * deltaTime;
    cloud2_x += 1.5f * deltaTime;
    cloud3_x += 1.5f * deltaTime;

    if (cloud_x > 10) cloud_x = 0;
    if (cloud2_x > 10) cloud2_x = 0;
    if (cloud3_x > 10) cloud3_x = 0;

    cloudMatrix = glm::mat4(1.0f);
    cloudMatrix = glm::translate(cloudMatrix, glm::vec3(cloud_x, 0.0f, 0.0f));
    
    cloudMatrix2 = glm::mat4(1.0f);
    cloudMatrix2 = glm::translate(cloudMatrix2, glm::vec3(cloud2_x, 0.0f, 0.0f));
    
    cloudMatrix3 = glm::mat4(1.0f);
    cloudMatrix3 = glm::translate(cloudMatrix3, glm::vec3(cloud3_x, 0.0f, 0.0f));

    ufoMatrix = glm::mat4(1.0f);
    ufoMatrix = glm::rotate(ufoMatrix, glm::radians(ufo_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

    
    mountainMatrix = glm::mat4(1.0f);
}

void drawCloud() {
    float vertices[]  = {
        -5.0, 1.90,
        -3.5, 1.90,
        -3.5, 2.60,
        -5.0, 1.90,
        -3.5, 2.60,
        -5.0, 2.60 };
    
    float texCoords[] = {
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, cloudTextureID);

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    program.SetmodelMatrix(cloudMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetmodelMatrix(cloudMatrix2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetmodelMatrix(cloudMatrix3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
}

void drawSun() {
    float vertices[] = {
        -4.7, 0.75,
        -2.7, 0.75,
        -2.7, 2.60,
        -4.7, 0.75,
        -2.7, 2.60,
        -4.7, 2.60 };
    
    float texCoords[] = {
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, sunTextureID);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetmodelMatrix(sunMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

void drawGround() {
    float vertices[]  = {
        -5.0, -2.81,
        5.0, -2.81,
        5.0, -1.81,
        -5.0, -1.81 };

    glVertexAttribPointer(program2.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program2.positionAttribute);
    
    float r = 10.0;
    float g = 166.0;
    float b = 18.0;
    program2.SetColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
    program2.SetmodelMatrix(groundMatrix);
    glDrawArrays(GL_QUADS, 0, 4);
    
    glDisableVertexAttribArray(program2.positionAttribute);

}

void drawUFO() {
    float vertices[] = {
        -0.45, -0.53,
        0.45, -0.53,
        0.45, 0.53,
        -0.45, -0.53,
        0.45, 0.53,
        -0.45, 0.53 };
    
    float texCoords[] = {
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, ufoTextureID);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    program.SetmodelMatrix(ufoMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
}

void drawMountain() {
    for (int i = 0; i < 2; i++) {
        if (i == 1) {
            mountainMatrix = glm::scale(mountainMatrix, glm::vec3(0.8f, 0.8f, 1.0f));
            mountainMatrix = glm::translate(mountainMatrix, glm::vec3(6.0f, -0.45f, 0.0f));
        }
        float vertices[] {
            -4.9, -1.81,
            0.1, -1.81,
            -1.9, 1.0 };
        
        glVertexAttribPointer(program2.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program2.positionAttribute);
        
        float r = 158.0;
        float g = 138.0;
        float b = 120.0;
        program2.SetColor(r/255.0f, g/255.0f, b/255.0f, 1.0f);
        program2.SetmodelMatrix(mountainMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDisableVertexAttribArray(program2.positionAttribute);
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawCloud();
    drawSun();
    drawGround();
    drawMountain();
    drawUFO();
    SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
