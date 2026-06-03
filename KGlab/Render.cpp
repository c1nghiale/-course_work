#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "Render.h"
#include "GUItextRectangle.h"
#include "MyShaders.h"
#include "ObjLoader.h"
#include "Texture.h"
#include "SolarSystem.h"
#include "SkyBox.h"

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iomanip>
#include <iostream>
#include <sstream>


#include "debout.h"

// Внутренняя логика "движка"
#include "MyOGL.h"
extern OpenGL gl;
#include "Light.h"
Light light;
#include "Camera.h"
Camera camera;
SkyBox skybox(100.0f);
static SolarSystem solarSystem;

bool texturing = true;
bool lightning = true;
bool fighter = false;

double full_time = 0;

// Переключение режимов
void switchModes(OpenGL* sender, KeyEventArg arg)
{
    auto key = LOWORD(MapVirtualKeyA(arg.key, MAPVK_VK_TO_CHAR));

    switch (key)
    {
    case 'L':
        solarSystem.toggleSunEmission();
        lightning = !lightning;
        break;
    case 'T':
        texturing = !texturing;
        skybox.needTexture = texturing;
        break;
    case 'R':
        solarSystem.reset();
        full_time = 0;
        break;
    case 'F':
        camera.toggleFreeMode();
        fighter = !fighter;
        break;
    }
}


// Умножение матриц c[M1][N1] = a[M1][N1] * b[M2][N2]
template <typename T, int M1, int N1, int M2, int N2> void MatrixMultiply(const T* a, const T* b, T* c)
{
    for (int i = 0; i < M1; ++i)
    {
        for (int j = 0; j < N2; ++j)
        {
            c[i * N2 + j] = T(0);
            for (int k = 0; k < N1; ++k)
            {
                c[i * N2 + j] += a[i * N1 + k] * b[k * N2 + j];
            }
        }
    }
}

// Текстовый прямоугольник в верхнем правом углу.
// OGL не предоставляет возможности для хранения текста;
// внутри этого класса создается картинка с текстом (через GDI),
// в виде текстуры накладывается на прямоугольник и рисуется на экране.
// Это самый простой, но очень неэффективный способ написать что-либо на экране.
GuiTextRectangle text;

// ID для текстуры
GLuint texId;

ObjModel f;

Texture skyboxTexture;

// Выполняется один раз перед первым рендером
void initRender()
{
    skyboxTexture.LoadTexture("C:\\Users\\pedro\\KGlabs\\LAB4\\KGlab\\textures\\SkyBox.png");
    skybox.setTextureId(skyboxTexture.getTexId());

    f.LoadModel("models//spaceship.obj");
    //==============НАСТРОЙКА ТЕКСТУР================
    // 4 байта на хранение пикселя
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    //================НАСТРОЙКА КАМЕРЫ======================
    camera.caclulateCameraPos();

    // привязываем камеру к событиям "движка"
    gl.WheelEvent.reaction(&camera, &Camera::Zoom);
    gl.MouseMovieEvent.reaction(&camera, &Camera::MouseMovie);
    gl.MouseLeaveEvent.reaction(&camera, &Camera::MouseLeave);
    gl.MouseLdownEvent.reaction(&camera, &Camera::MouseStartDrag);
    gl.MouseLupEvent.reaction(&camera, &Camera::MouseStopDrag);
    //==============НАСТРОЙКА СВЕТА===========================
    // Привязываем свет к событиям "движка"
    gl.MouseMovieEvent.reaction(&light, &Light::MoveLight);
    gl.KeyDownEvent.reaction(&light, &Light::StartDrug);
    gl.KeyUpEvent.reaction(&light, &Light::StopDrug);
    //========================================================
    //====================Прочее==============================
    gl.KeyDownEvent.reaction(switchModes);
    text.setSize(512, 180);
    //========================================================

    camera.setPosition(2, 1.5, 1.5);
}

float view_matrix[16];

int location = 0;

void starfighter() {
    glPushMatrix();

    Vector3 shipPos = camera.getPosition() + camera.getFront() * 5.0;
    glTranslated(shipPos.x(), shipPos.y(), shipPos.z());

    Vector3 front = camera.getFront().normalized();
    Vector3 worldUp(0, 0, 1);
    Vector3 right = (front ^ worldUp).normalized();
    Vector3 up = (right ^ front).normalized();

    double mat[16] = {
        right.x(), right.y(), right.z(), 0,
           up.x(),    up.y(),    up.z(), 0,
       -front.x(),-front.y(),-front.z(), 0,
               0,        0,        0, 1
    };

    glMultMatrixd(mat);
    glRotated(-90, 0, 1, 0);
    glRotated(camera.getRoll(), 1, 0, 0);
    glScaled(0.2, 0.2, 0.2);

    glDisable(GL_TEXTURE_2D);
    if (lightning) glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glColor3d(0.2, 0.5, 0.8);

    f.Draw();

    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
}

void Render(double delta_time)
{
    
    full_time += delta_time;
    camera.processKeyboard(delta_time);
    camera.SetUpCamera();
    // Забираем матрицу MODELVIEW сразу после установки камеры,
    // так как в ней отсутствуют трансформации glRotate
    glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix);

    light.SetUpLight();

    glBindTexture(GL_TEXTURE_2D, 0);

    // Включаем нормализацию нормалей
    // чтобы glScaled не влияли на них.

    glEnable(GL_NORMALIZE);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    // Переключаем режимы (см void switchModes(OpenGL *sender, KeyEventArg arg))
    if (lightning)
        glEnable(GL_LIGHTING);
    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0); // Сбрасываем текущую текстуру
    }
    //=============НАСТРОЙКА МАТЕРИАЛА==============

    // Настройка материала, все что рисуется ниже будет иметь этот материал.
    // Массивы с настройками материала
    float amb[] = {0.2, 0.2, 0.1, 1.};
    float dif[] = {0.4, 0.65, 0.5, 1.};
    float spec[] = {0.9, 0.8, 0.3, 1.};
    float sh = 0.2f * 256;

    // Фоновая
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
    // Дифузная
    glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
    // Зеркальная
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    // Размер блика
    glMaterialf(GL_FRONT, GL_SHININESS, sh);

    // Сглаживание освещения
    glShadeModel(GL_SMOOTH); // закраска по Гуро
                             //(GL_SMOOTH - плоская закраска)

    //============ РИСОВАТЬ ТУТ ==============
    
    skybox.draw();
    solarSystem.update();
    solarSystem.draw();

    if (camera.isFreeMode()) {
        starfighter();
    }


    //===============================================

    // Сбрасываем все трансформации
    glLoadIdentity();
    camera.SetUpCamera();

    //================Сообщение в верхнем левом углу=======================
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, gl.getWidth(), 0, gl.getHeight(), -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    std::wstringstream ss;
    ss << std::fixed << std::setprecision(3) << "T - " << (texturing ? L"[вкл]выкл" : L"вкл[выкл]") << L" текстур\n"
       << "L - " << (lightning ? L"[вкл]выкл" : L"вкл[выкл]") << L" освещение\n"
       << L"F - " << (fighter ? L"[вкл]выкл" : L"вкл[выкл]") << L" режим starfighter\n"
	   << L"W - вперед "
       << L"S - назад "
       << L"A - влево "
       << L"D - вправо "
       << L"QE - крен\n"
       << L"R - сбросить солнечную систему\n"
       << L"Координаты камеры: (" << std::setw(7) << camera.x() << "," << std::setw(7) << camera.y() << ","
       << std::setw(7) << camera.z() << ")\n"
       << L"Параметры камеры: R=" << std::setw(7) << camera.distance() << ", fi1=" << std::setw(7) << camera.fi1()
       << ", fi2=" << std::setw(7) << camera.fi2() << '\n'
       << L"delta_time: " << std::setprecision(5) << delta_time << " full_time: " << std::setprecision(2) << full_time << std::endl;
      

    text.setPosition(10, gl.getHeight() - 10 - 180);
    text.setText(ss.str().c_str(), 255, 255, 255);
    text.Draw();

    // Восстанавливаем матрицу проекции на перспективу, которую сохраняли ранее.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
