#pragma once
#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include "Vector3.h"
#include <vector>
#include <string>
#include <cmath>

class Celestial {
public:
        Vector3 position;
        Vector3 velocity;
        double mass;
        double radius;
        double r, g, b;
        std::vector<Vector3> OrbitTrail;

        Celestial(const std::vector<double>&);
        void drawCelestial();
        void update(double dt, const Vector3& totalForce);
        Vector3 getForceFrom(const Celestial& other, double G) const;
};

class SolarSystem: public Celestial {
protected:
    double G;
    double dt;
    int maxTrailPoints;
    bool sunEmission;

    void drawCircle(Vector3 center, double radius, int segments);
    void drawSaturnRings();

public:
    void toggleSunEmission();
    std::vector<Celestial> bodies;

    SolarSystem();
    ~SolarSystem();

    void update();
    void draw();
    void reset();

    size_t getBodyCount() const { return bodies.size(); };
    Vector3 getBodyPosition(size_t index) const;

    void setGravityConstant(double g) { G = g; };
    void setTimeStep(double timeStep) { dt = timeStep; };
    void setMaxTrailPoints(int maxPoints) { maxTrailPoints = maxPoints; };

};

#endif  // SOLARSYSTEM_H
