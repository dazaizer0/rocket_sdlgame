#pragma once

#include "config.h"
#include <SDL2/SDL.h>
#include <cmath>

class Line {
public:
    mathy::vec2<int> start;
    mathy::vec2<int> end;

    Line(mathy::vec2<int> startv, mathy::vec2<int> endv);
    void render_line(SDL_Renderer* rend);
    ~Line();

private:
    void draw_thick_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness);
};

Line::Line(mathy::vec2<int> startv, mathy::vec2<int> endv)
    : start(startv), end(endv)
{
}

void Line::render_line(SDL_Renderer* rend) {
    int thickness = 10;
    draw_thick_line(rend, start.x, start.y, end.x, end.y, thickness);
}

void Line::draw_thick_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    double angle = atan2(y2 - y1, x2 - x1);

    int dx = static_cast<int>(thickness * sin(angle) / 2);
    int dy = static_cast<int>(thickness * cos(angle) / 2);

    for (int i = -thickness / 2; i < thickness / 2; ++i) {
        int offsetX = static_cast<int>(i * sin(angle));
        int offsetY = static_cast<int>(i * cos(angle));
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderDrawLine(renderer, x1 + offsetX, y1 - offsetY, x2 + offsetX, y2 - offsetY);
    }
}

Line::~Line() {
}
