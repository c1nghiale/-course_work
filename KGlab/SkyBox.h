#pragma once
#include <windows.h>
#include <GL/gl.h>

class SkyBox {
    GLuint textureId;
    float radius;

public:
    bool needTexture = true;
    SkyBox(float r = 60.0f);
    ~SkyBox();
    void setTextureId(GLuint id);
    void draw();
    
};