#ifndef FOOD_HPP
#define FOOD_HPP
#include "raylib.h"
#include <random>

class Food
{
    private:
        float radius;
        Vector2 pos;
        float valueRemaining;

    public:
        Food(Vector2 spawnPoint)
        {
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> radSeed (5, 25);

            radius = radSeed(mt);
            pos = spawnPoint;
            valueRemaining = radius * radius;
        }

        Food(int xBound, int yBound)
        {
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> maxVal (25, 625);
            std::uniform_int_distribution<int> locX (50, xBound - 50);
            std::uniform_int_distribution<int> locY (50, yBound - 50);

            radius = sqrt(maxVal(mt));
            pos = {(float)locX(mt), (float)locY(mt)};
            valueRemaining = radius * radius;
        }


        void DrawFood()
        {
            Color c = GOLD;
            c.a = (valueRemaining / (radius * radius)) * 255.0f;
            DrawCircle((int)pos.x, (int)pos.y, radius, c);
            DrawText(std::to_string(valueRemaining).c_str(), pos.x, pos.y, 10, BLACK);
        }

        void UpdateValueRemaining(float damage)
        {
            valueRemaining -= damage;
        }

        float GetRadius()
        {
            return radius;
        }

        Vector2 GetPos()
        {
            return pos;
        }

        float GetValueRemaining()
        {
            return valueRemaining;
        }



};




#endif