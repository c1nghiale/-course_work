#include "SkyBox.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SkyBox::SkyBox(float r) : radius(r), textureId(0) {}

SkyBox::~SkyBox()
{}

void SkyBox::setTextureId(GLuint id)
{
    textureId = id;
}

void SkyBox::draw()
{
    if (!textureId) return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    if (needTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glColor3d(1, 1, 1);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3d(0.0, 0.0, 0.0);
    }

    int stacks = 64;
    int slices = 128;

    for (int i = 0; i < stacks; i++) {
        double lat1 = M_PI * (-0.5 + (double)i / stacks);
        double lat2 = M_PI * (-0.5 + (double)(i + 1) / stacks);
        double sinLat1 = sin(lat1), cosLat1 = cos(lat1);
        double sinLat2 = sin(lat2), cosLat2 = cos(lat2);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++) {
            double lon = 2 * M_PI * (double)j / slices;
            double cosLon = cos(lon), sinLon = sin(lon);

            if (needTexture) {
                double tx1 = (double)j / slices;
                double ty1 = (double)i / stacks;
                double tx2 = (double)j / slices;
                double ty2 = (double)(i + 1) / stacks;
                glTexCoord2d(tx1, ty1);
            }
            glVertex3d(radius * cosLat1 * cosLon, radius * sinLat1, radius * cosLat1 * sinLon);

            if (needTexture) {
                double tx2 = (double)j / slices;
                double ty2 = (double)(i + 1) / stacks;
                glTexCoord2d(tx2, ty2);
            }
            glVertex3d(radius * cosLat2 * cosLon, radius * sinLat2, radius * cosLat2 * sinLon);
        }
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glPopAttrib();
}