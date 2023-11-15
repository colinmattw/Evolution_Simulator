#ifndef FOOD_HPP
#define FOOD_HPP
#include "raylib.h"
#include "Entity.hpp"
#include <random>

class Food
{
    private:
        float radius;
        Vector2 pos;
        float valueRemaining;

        bool CheckCircleAndLineSegmentCollision(Vector2 circleCenter, float circleRadius, Vector2 p1, Vector2 p2) {
            // Calculate the squared radius of the circle to avoid square roots.
            float radiusSquared = circleRadius * circleRadius;

            // Calculate the vector from the start of the line segment to the circle's center.
            float dx = circleCenter.x - p1.x;
            float dy = circleCenter.y - p1.y;

            // Calculate the squared length of the line segment.
            float segmentLengthSquared = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);

            // Calculate the dot product of the vector and the line segment.
            float dotProduct = dx * (p2.x - p1.x) + dy * (p2.y - p1.y);

            if (dotProduct < 0.0f) {
                // The closest point on the line is before the start of the segment.
                return (dx * dx + dy * dy <= radiusSquared);
            }

            if (dotProduct > segmentLengthSquared) {
                // The closest point on the line is after the end of the segment.
                float dx2 = circleCenter.x - p2.x;
                float dy2 = circleCenter.y - p2.y;
                return (dx2 * dx2 + dy2 * dy2 <= radiusSquared);
            }

            // The closest point on the line is within the segment.
            float closestX = p1.x + (dotProduct / segmentLengthSquared) * (p2.x - p1.x);
            float closestY = p1.y + (dotProduct / segmentLengthSquared) * (p2.y - p1.y);

            float distanceSquared = (circleCenter.x - closestX) * (circleCenter.x - closestX) + (circleCenter.y - closestY) * (circleCenter.y - closestY);

            return (distanceSquared <= radiusSquared);
        }    

    public:
        Food(Vector2 spawnPoint)
        {
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> radSeed (5, 15);

            radius = radSeed(mt);
            pos = spawnPoint;
            valueRemaining = radius * radius;
        }

        Food(int xBound, int yBound)
        {
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> maxVal (5, 15);
            std::uniform_int_distribution<int> locX (50, xBound - 50);
            std::uniform_int_distribution<int> locY (50, yBound - 50);

            radius = maxVal(mt);
            pos = {(float)locX(mt), (float)locY(mt)};
            valueRemaining = radius * radius;
        }


        void DrawFood()
        {
            Color c = GOLD;
            c.a = (valueRemaining / (radius * radius)) * 255.0f;
            DrawCircle((int)pos.x, (int)pos.y, radius, c);
            DrawText(std::to_string((int)(valueRemaining)).c_str(), pos.x, pos.y, 10, BLACK);
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
        void Teleport(Vector2 newPos)
        {
            pos = newPos;
        }


        bool CheckCollisionWall(Wall* wall)
        {
            Vector2* coords = wall->GetCoords();
            bool collisionDetected = false;

            if(CheckCircleAndLineSegmentCollision(pos, radius, coords[0], coords[1]))
            {
                return true;
            }
            else if(CheckCircleAndLineSegmentCollision(pos, radius, coords[0], coords[2]))
            {
                return true;
            }
            else if(CheckCircleAndLineSegmentCollision(pos, radius, coords[1], coords[3]))
            {
                return true;
            }
            else if(CheckCircleAndLineSegmentCollision(pos, radius, coords[2], coords[3]))
            {
                return true;
            }
            else
            {
                return false;
            }
        }



};




#endif