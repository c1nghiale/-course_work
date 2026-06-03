#include <windows.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "SolarSystem.h"
#include <cmath>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>

void drawSphere(Vector3 center, double radius, int stacks, int slices)
{
    for (int i = 0; i < stacks; i++) {
        double lat1 = M_PI * (-0.5 + (double)i / stacks);
        double lat2 = M_PI * (-0.5 + (double)(i + 1) / stacks);
        double sinLat1 = sin(lat1), cosLat1 = cos(lat1);
        double sinLat2 = sin(lat2), cosLat2 = cos(lat2);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++) {
            double lon = 2 * M_PI * (double)j / slices;
            double cosLon = cos(lon), sinLon = sin(lon);

            glNormal3d(cosLat1 * cosLon, sinLat1, cosLat1 * sinLon);
            glVertex3d(center.x() + radius * cosLat1 * cosLon,
                center.y() + radius * sinLat1,
                center.z() + radius * cosLat1 * sinLon);

            glNormal3d(cosLat2 * cosLon, sinLat2, cosLat2 * sinLon);
            glVertex3d(center.x() + radius * cosLat2 * cosLon,
                center.y() + radius * sinLat2,
                center.z() + radius * cosLat2 * sinLon);
        }
        glEnd();
    }
}

void drawOrbits(Celestial& body) {
    glLineWidth(1.0);
    if (body.OrbitTrail.size() > 1) {
        glColor4d(body.r, body.g, body.b, 0.3);
        glBegin(GL_LINE_STRIP);
        for (const auto& point : body.OrbitTrail) {
            glVertex3d(point.x(), point.y(), point.z());
        }
        glEnd();
    }
}

// ============== Celestial ==============

Celestial::Celestial(const std::vector<double>& values)
    : position(values[0], values[1], values[2])
    , velocity(values[3], values[4], values[5])
    , mass(values[6])
    , radius(values[7])
    , r(values[8]), g(values[9]), b(values[10])
{
    OrbitTrail.reserve(10000);
}

void Celestial::drawCelestial()
{
    drawSphere(position, radius, 32, 32);
}

void Celestial::update(double dt, const Vector3& totalForce)
{
    Vector3 acceleration = totalForce * (1.0 / mass);
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;

    OrbitTrail.push_back(position);
    if (OrbitTrail.size() > 5000) {
        OrbitTrail.erase(OrbitTrail.begin());
    }
}

Vector3 Celestial::getForceFrom(const Celestial& other, double G) const
{
    Vector3 r = other.position - position;
    double distance = r.length();
    if (distance < 0.1) distance = 0.1;

    double forceMagnitude = G * mass * other.mass / (distance * distance);
    return r.normalized() * forceMagnitude;
}

// ============== SolarSystem ==============

SolarSystem::SolarSystem()
    : Celestial({ 0, 0, 0, 0, 0, 0, 0, 0, 1.0, 1.0, 1.0 })
    , G(1000.0)
    , dt(0.0001)
    , maxTrailPoints(5000)
    , sunEmission(true)
{
    double M = 10000.0;

    // Солнце
    bodies.push_back(Celestial({ 0, 0, 0, 0, 0, 0, M, 2.0, 1.0, 0.7, 0.0 }));

    bodies.push_back(Celestial({ 8, 0, 0, 0, sqrt(G * M / 8), 0, 0.1, 0.4, 0.7, 0.7, 0.7 }));   // Меркурий
    bodies.push_back(Celestial({ 12, 0, 0, 0, sqrt(G * M / 12), 0, 0.8, 0.8, 0.9, 0.7, 0.2 }));  // Венера
    bodies.push_back(Celestial({ 16, 0, 0, 0, sqrt(G * M / 16), 0, 1.0, 0.9, 0.2, 0.5, 1.0 }));  // Земля
    bodies.push_back(Celestial({ 20, 0, 0, 0, sqrt(G * M / 20), 0, 0.3, 0.5, 1.0, 0.3, 0.1 }));  // Марс
    bodies.push_back(Celestial({ 28, 0, 0, 0, sqrt(G * M / 28), 0, 5.0, 1.8, 0.8, 0.6, 0.4 }));  // Юпитер
    bodies.push_back(Celestial({ 36, 0, 0, 0, sqrt(G * M / 36), 0, 3.0, 1.5, 0.9, 0.8, 0.5 }));  // Сатурн
    bodies.push_back(Celestial({ 44, 0, 0, 0, sqrt(G * M / 44), 0, 1.5, 1.2, 0.4, 0.7, 0.9 }));  // Уран
    bodies.push_back(Celestial({ 52, 0, 0, 0, sqrt(G * M / 52), 0, 1.7, 1.1, 0.2, 0.3, 0.9 }));  // Нептун
}

void SolarSystem::toggleSunEmission()
{
    sunEmission = !sunEmission;
}


void SolarSystem::update()
{
    for (size_t i = 1; i < bodies.size(); i++) {
        Vector3 r_vec = bodies[0].position - bodies[i].position;
        double r = r_vec.length();

        double a = G * bodies[0].mass / (r * r);
        Vector3 acceleration = r_vec.normalized() * a;

        bodies[i].velocity = bodies[i].velocity + acceleration * dt;
        bodies[i].position = bodies[i].position + bodies[i].velocity * dt;

        bodies[i].OrbitTrail.push_back(bodies[i].position);
    }
}

void SolarSystem::draw()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_TEXTURE_2D);

    
    glDisable(GL_LIGHTING);
    for (auto& body : bodies) {
        drawOrbits(body);
    }

    if (sunEmission) {
        glEnable(GL_LIGHTING);

        float sunEmis[] = { 1.0f, 0.7f, 0.0f, 1.0f };
        float sunDif[] = { 1.0f, 0.7f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, sunEmis);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, sunDif);
        bodies[0].drawCelestial();

        for (size_t i = 1; i < bodies.size(); i++) {
            float dif[] = { static_cast<float>(bodies[i].r),
                           static_cast<float>(bodies[i].g),
                           static_cast<float>(bodies[i].b), 1.0f };
            float zeroEmis[] = { 0, 0, 0, 1.0f };
            glMaterialfv(GL_FRONT, GL_EMISSION, zeroEmis);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
            bodies[i].drawCelestial();
        }
    }
    else {
        glDisable(GL_LIGHTING);

        glColor3d(1.0, 0.7, 0.0);
        bodies[0].drawCelestial();

        for (size_t i = 1; i < bodies.size(); i++) {
            glColor3d(bodies[i].r, bodies[i].g, bodies[i].b);
            bodies[i].drawCelestial();
        }
    }

    glPopAttrib();
}

void SolarSystem::reset()
{
    bodies.clear();
    *this = SolarSystem();
}

SolarSystem::~SolarSystem() {}