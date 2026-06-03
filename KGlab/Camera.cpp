#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "Camera.h"
#include <windows.h>
#include <GL/glu.h>
#include <cmath>

Camera::Camera()
    : camDist(5), camNz(1), _fi1(1), _fi2(0.5)
    , position(0, 0, 60), front(0, 0, -1), up(0, 1, 0)
    , yaw(0), pitch(-30), speed(20), roll(0), freeMode(false)
{
    caclulateCameraPos();
    updateVectors();
}

void Camera::setPosition(double x, double y, double z)
{
    camX = x; camY = y; camZ = z;
    camDist = sqrt(x * x + y * y + z * z);
    _fi1 = atan2(y, x);
    _fi2 = atan2(z, sqrt(x * x + y * y));
}

void Camera::caclulateCameraPos()
{
    camX = camDist * cos(_fi2) * cos(_fi1);
    camY = camDist * cos(_fi2) * sin(_fi1);
    camZ = camDist * sin(_fi2);
    camNz = (cos(_fi2) <= 0) ? -1 : 1;
}

void Camera::updateVectors()
{
    double rYaw = yaw * M_PI / 180.0;
    double rPitch = pitch * M_PI / 180.0;
    Vector3 f(cos(rPitch) * cos(rYaw), cos(rPitch) * sin(rYaw), sin(rPitch));
    front = f.normalized();
    Vector3 worldUp(0, 0, 1);
    right = (front ^ worldUp).normalized();
    up = (right ^ front).normalized();
}

void Camera::processKeyboard(double deltaTime)
{
    if (!freeMode) return;
    double v = speed * deltaTime;

if (freeMode) {
    double v = speed * deltaTime;
    
    if (OpenGL::isKeyPressed('W')) position = position + front * v;
    if (OpenGL::isKeyPressed('S')) position = position - front * v;
    if (OpenGL::isKeyPressed('D')) yaw -= v * 1.5;
    if (OpenGL::isKeyPressed('A')) yaw += v * 1.5;
    if (OpenGL::isKeyPressed('Q')) roll += v * 1.5;
    if (OpenGL::isKeyPressed('E')) roll -= v * 1.5;

    updateVectors();
}

    updateVectors();
}

void Camera::toggleFreeMode()
{
    freeMode = !freeMode;
    if (!freeMode) caclulateCameraPos();
}

void Camera::Zoom(OpenGL* sender, MouseWheelEventArg arg)
{
    if (freeMode)
    {
        speed += arg.value * 2;
        if (speed < 5) speed = 5;
        if (speed > 200) speed = 200;
        return;
    }
    if (arg.value < 0 && camDist <= 1) return;
    if (arg.value > 0 && camDist >= 100) return;
    camDist += 0.01 * arg.value;
    caclulateCameraPos();
}

void Camera::MouseMovie(OpenGL* sender, MouseEventArg arg)
{
    if (freeMode)
    {
        if (!OpenGL::isKeyPressed(VK_LBUTTON)) { lastFreeX = -1; return; }
        if (lastFreeX == -1) { lastFreeX = arg.x; lastFreeY = arg.y; return; }
        double dx = arg.x - lastFreeX, dy = arg.y - lastFreeY;
        lastFreeX = arg.x; lastFreeY = arg.y;
        yaw += dx * 0.15; pitch += dy * 0.15;
        if (pitch > 89) pitch = 89;
        if (pitch < -89) pitch = -89;
        updateVectors();
        return;
    }
    if (OpenGL::isKeyPressed('G')) return;
    if (mouseX == -1) { mouseX = arg.x; mouseY = arg.y; return; }
    int dx = mouseX - arg.x, dy = mouseY - arg.y;
    mouseX = arg.x; mouseY = arg.y;
    if (drag) { _fi1 += 0.01 * dx; _fi2 -= 0.01 * dy; caclulateCameraPos(); }
}

void Camera::SetUpCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (freeMode)
    {
        double r = roll * M_PI / 180.0;
        Vector3 rolledUp = up * cos(r) + right * sin(r);
        Vector3 target = position + front;
        gluLookAt(position.x(), position.y(), position.z(),
            target.x(), target.y(), target.z(),
            rolledUp.x(), rolledUp.y(), rolledUp.z());
    }
    else
    {
        gluLookAt(camX, camY, camZ, 0, 0, 0, 0, 0, camNz);
    }
}