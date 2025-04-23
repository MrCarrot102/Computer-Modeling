#pragma once 

struct Body {
    double x, y;         // aktualna pozycja
    double vx, vy;       // aktualna prędkość (potrzebna do inicjalizacji)
    double x_prev, y_prev; // poprzednia pozycja (do Verleta)
    double mass;

    Body(double x_, double y_, double vx_, double vy_, double m)
        : x(x_), y(y_), vx(vx_), vy(vy_), mass(m)
    {
        x_prev = x - vx;
        y_prev = y - vy;
    }
};

