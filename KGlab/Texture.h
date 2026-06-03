#pragma once
#include <string>
#include <windows.h>
#include <GL/gl.h>

class Texture
{
    unsigned int texId = 0;

  public:
    Texture(){};
    ~Texture();

    void LoadTexture(const std::string& texture_file_name);
    void Bind();
    GLuint getTexId() const { return texId; };
};
