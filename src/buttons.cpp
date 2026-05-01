#include "buttons.h"

void DrawButton(const Rectangle rect, 
                const char* text, 
                const Font& font, 
                const Font& hoverFont,
                Color baseColor, 
                Color hoverColor,
                float padX, 
                float padY
)
{
    bool isHover = CheckCollisionPointRec(GetMousePosition(), rect);
    Color currentColor = isHover ? hoverColor : baseColor;

    // Shadow
    DrawRectangleRounded({rect.x + 3, rect.y + 3, rect.width, rect.height},
                         0.3f, 10, Fade(BLACK, 0.3f));

    // Button
    DrawRectangleRounded(rect, 0.3f, 10, currentColor);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);

    // Measure text
    float fontSize = 40;
    float spacing = 2;

    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);

    // Ensure padding constraint (shrink text if needed)
    float maxWidth = rect.width - 2 * padX;
    float maxHeight = rect.height - 2 * padY;

    if (textSize.x > maxWidth)
    {
        float scale = maxWidth / textSize.x;
        fontSize *= scale;
        textSize = MeasureTextEx(font, text, fontSize, spacing);
    }

    if (textSize.y > maxHeight)
    {
        float scale = maxHeight / textSize.y;
        fontSize *= scale;
        textSize = MeasureTextEx(font, text, fontSize, spacing);
    }

    // Final centered position WITH padding respected
    Vector2 pos = {
        rect.x + padX + (maxWidth - textSize.x) / 2.0f,
        rect.y + padY + (maxHeight - textSize.y) / 2.0f
    };

    DrawTextEx(font, text, pos, fontSize, spacing, BLACK);
}

void DrawGradientButton(
    const Rectangle& rect,
    const char* text,
    const Font& font,
    const Font& hoverFont,
    Color baseColor,
    Color hoverColor,
    float padX,
    float padY,
    float fontSize,
    float spacing
)
{
    bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    Font textFont = hovered ? hoverFont : font;

    // Pick colors
    Color top = hovered ? hoverColor : baseColor;
    Color bottom = {
        (unsigned char)(top.r * 0.8f),
        (unsigned char)(top.g * 0.8f),
        (unsigned char)(top.b * 0.8f),
        top.a
    };

    // Shadow
    DrawRectangleRounded({rect.x + 4, rect.y + 4, rect.width, rect.height},
                         0.2f, 8, Fade(BLACK, 0.35f));

    // Gradient body
    DrawRectangleGradientV(rect.x, rect.y, rect.width, rect.height, top, bottom);

    // Border (adaptive)
    DrawRectangleRoundedLines(rect, 0.2f, 8,
        hovered ? Color{255,255,255,120} : Color{0,0,0,60});

    // Measure text
    Vector2 textSize = MeasureTextEx(font, text, fontSize, spacing);

    float maxW = rect.width - 2 * padX;
    float maxH = rect.height - 2 * padY;

    // Scale text if needed
    float scale = 1.0f;
    if (textSize.x > maxW) scale = maxW / textSize.x;
    if (textSize.y * scale > maxH) scale = maxH / textSize.y;

    float finalSize = fontSize * scale;
    textSize = MeasureTextEx(textFont, text, finalSize, spacing);

    Vector2 pos = {
        rect.x + padX + (maxW - textSize.x) / 2.0f,
        rect.y + padY + (maxH - textSize.y) / 2.0f
    };

    // Text shadow
    DrawTextEx(textFont, text, {pos.x + 1, pos.y + 1}, finalSize, spacing, Fade(BLACK, 0.6f));

    // Main text
    DrawTextEx(textFont, text, pos, finalSize, spacing, WHITE);
}

void DrawButtonShadow(const Rectangle& r)
{
    DrawRectangleRounded(
        Rectangle{r.x - 4, r.y - 4, r.width + 8, r.height + 8},
        0.3f, 8,
        Color{0, 0, 0, 90}
    );
}