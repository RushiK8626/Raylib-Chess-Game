#ifndef BUTTONS_H
#define BUTTONS_H

#include "raylib.h"

void DrawButton(Rectangle rect, const char* text, const Font& font, const Font& hoverFont, 
                Color baseColor, Color hoverColor,
                float padX = 12.0f, float padY = 8.0f);

void DrawGradientButton(
    const Rectangle& rect,
    const char* text,
    const Font& font,
    const Font& hoverFont,
    Color baseColor,
    Color hoverColor,
    float padX = 16.0f,
    float padY = 10.0f,
    float fontSize = 40.0f,
    float spacing = 2.0f
);

void DrawButtonShadow(const Rectangle& r);

#endif