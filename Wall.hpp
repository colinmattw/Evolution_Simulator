#ifndef Wall_HPP
#define Wall_HPP
#include <ctime>
#include <vector>
#include "raylib.h"
#include <cmath>
#include <tuple>

class Wall
{
    private:
        int width;
        Vector2 coords[4];
        Rectangle rectangle;
        int c;

    public:

        //Random wall generation
        Wall(const int rangeX1, const int rangeX2, const int rangeY1, const int rangeY2, const int recWidth)
        {
            width = recWidth;
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> distX(rangeX1, rangeX2);
            std::uniform_int_distribution<int> distY(rangeY1, rangeY2);

            Vector2 c1 = {(float)distX(mt), (float)distY(mt)};
            Vector2 c2 = {(float)distX(mt), (float)distY(mt)};
            const float dx = c2.x - c1.x;
            const float dy = c2.y - c1.y;
            const float angle = atan2(dy, dx);
            Vector2 c3 = {(recWidth * cosf(angle + 1.5708)) + c1.x, (recWidth * sinf(angle + 1.5708)) + c1.y};
            Vector2 c4 = {(recWidth * cosf(angle + 1.5708)) + c2.x, (recWidth * sinf(angle + 1.5708)) + c2.y};
            coords[0] = c1;
            coords[1] = c2;
            coords[2] = c3;
            coords[3] = c4;
        }

        //Wall with seeded start
        Wall(const int rangeX1, const int rangeX2, const int rangeY1, const int rangeY2, const int recWidth, Vector2 start)
        {
            width = recWidth;
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> distX(rangeX1, rangeX2);
            std::uniform_int_distribution<int> distY(rangeY1, rangeY2);

            Vector2 c1 = start;
            Vector2 c2 = {(float)distX(mt), (float)distY(mt)};
            const float dx = c2.x - c1.x;
            const float dy = c2.y - c1.y;
            const float angle = atan2(dy, dx);
            Vector2 c3 = {(recWidth * cosf(angle + 1.5708)) + c1.x, (recWidth * sinf(angle + 1.5708)) + c1.y};
            Vector2 c4 = {(recWidth * cosf(angle + 1.5708)) + c2.x, (recWidth * sinf(angle + 1.5708)) + c2.y};
            coords[0] = c1;
            coords[1] = c2;
            coords[2] = c3;
            coords[3] = c4;
        }

        //Wall for outer wall
        Wall(const int startX, const int startY, const int endX, const int endY, const int recWidth, bool isOuterWall)
        {
            width = recWidth;

            Vector2 c1 = {(float)startX, (float)startY};
            Vector2 c2 = {(float)endX, (float)endY};
            const float dx = c2.x - c1.x;
            const float dy = c2.y - c1.y;
            const float angle = atan2(dy, dx);
            Vector2 c3 = {(recWidth * cosf(angle + 1.5708)) + c1.x, (recWidth * sinf(angle + 1.5708)) + c1.y};
            Vector2 c4 = {(recWidth * cosf(angle + 1.5708)) + c2.x, (recWidth * sinf(angle + 1.5708)) + c2.y};
            coords[0] = c1;
            coords[1] = c2;
            coords[2] = c3;
            coords[3] = c4;
        }
        void DrawWall()
        {
            /*DrawCircle((int)coords[0].x, (int)coords[0].y, 7.0f, PINK);
            DrawCircle((int)coords[1].x, (int)coords[1].y, 7.0f, PINK);
            DrawCircle((int)coords[2].x, (int)coords[2].y, 7.0f, PINK);
            DrawCircle((int)coords[3].x, (int)coords[3].y, 7.0f, PINK);
            DrawLine((int)coords[0].x, (int)coords[0].y, (int)coords[1].x, (int)coords[1].y, BLACK);
            DrawLine((int)coords[0].x, (int)coords[0].y, (int)coords[2].x, (int)coords[2].y, BLACK);
            DrawLine((int)coords[1].x, (int)coords[1].y, (int)coords[3].x, (int)coords[3].y, BLACK);
            DrawLine((int)coords[2].x, (int)coords[2].y, (int)coords[3].x, (int)coords[3].y, BLACK);*/

            DrawTriangle(coords[2], coords[1], coords[0], BLACK);
            DrawTriangle(coords[2], coords[3], coords[1], BLACK);
        }

        Vector2 GetC3()
        {
            return coords[3];
        }

        Vector2* GetCoords()
        {
            return coords;
        }



};

#endif