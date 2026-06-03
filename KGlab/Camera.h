#pragma once
#include "MyOGL.h"
#include "Vector3.h"

class Camera
{
    double camDist = 5;
    int camNz = 1;
    double camX;
    double camY;
    double camZ;
    int mouseX = -1, mouseY = -1;
    bool drag = false;

    Vector3 position;
    Vector3 front;
    Vector3 up;
    Vector3 right;
    double yaw;
    double pitch;
    double speed;
    bool freeMode;
    bool freeMouseLook;
    int lastFreeX = -1, lastFreeY = -1;

public:
    double _fi1 = 1;
    double _fi2 = 0.5;

    Camera();
    void setPosition(double x, double y, double z);

    double distance() const { return camDist; }
    int nZ() const { return camNz; }
    double x() const { return freeMode ? position.x() : camX; }
    double y() const { return freeMode ? position.y() : camY; }
    double z() const { return freeMode ? position.z() : camZ; }
    double fi1() const { return _fi1; }
    double fi2() const { return _fi2; }
    Vector3 getPosition() const { return position; }
    Vector3 getFront() const { return front; }
    double getRoll() const { return roll; }

    void caclulateCameraPos();
    void Zoom(OpenGL* sender, MouseWheelEventArg arg);
    void MouseMovie(OpenGL* sender, MouseEventArg arg);

    void MouseLeave(OpenGL* sender, MouseEventArg arg) { mouseX = -1; }
    void MouseStartDrag(OpenGL* sender, MouseEventArg arg) { drag = true; }
    void MouseStopDrag(OpenGL* sender, MouseEventArg arg) { drag = false; mouseX = -1; }

    void SetUpCamera();
    void processKeyboard(double deltaTime);
    void toggleFreeMode();
    bool isFreeMode() const { return freeMode; }

private:
    void updateVectors();
    double roll = 0.0;
};