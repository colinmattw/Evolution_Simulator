#ifndef Entity_HPP
#define Entity_HPP
#include "NeuralNetwork.hpp"
#include <ctime>
#include <vector>
#include "raylib.h"
#include "Wall.hpp"
#include "Food.hpp"
#include <cmath>
#include <tuple>

class Entity
{
    private:
        //input neurons
        float health;
        float damage;
        float fov = 36.8864985754;
        float viewDist = 153;
        std::tuple <Entity*, float> sight[12];
        std::tuple <Wall*, float> wallSight[12];
        std::tuple <Food*, float> foodSight[12];
        std::vector<Vector2> sightPoints;

        //output neurons
        bool rotationDir;
        bool attack = true;
        float linearSpeed;
        float angularSpeed;

        //genotype
        NeuralNetwork* neuralNet;
        NeuralNetwork* netToPassOn;
        bool isPrey;
        float speedMultiplyer;
        std::vector<Entity*> entitiesOfInterest;
        std::vector<Food*> foodOfInterest;

        //other info
        Vector2 pos;
        float radius;
        float orientation;
        bool willReproduce;
        float killCount;
        clock_t birthTime;
        int gameTicksSurvived;
        int children = 0;
        Entity* parent;
        int lineageCount;
        bool hadDinnerThisTick = false;
        int ticksSinceDinner = 0;
        bool isTruman = false;
        int ticksSinceReproduction = 0;

        float ReturnWallorFood(std::tuple<Wall*, float> wall, std::tuple<Food*, float> food)
        {
            if(std::get<1>(wall) != 0)
            {
                return std::get<1>(wall);
            }
            else if(std::get<1>(food) != 0)
            {
                return std::get<1>(food);
            }
            else
            {
                return 0;
            }
           
        }

        float FastSquareRoot(float x, float epsilon = 1e-3) 
        {
            // Initial guess for the square root
            double guess = x / 2.0;

            while (std::abs(guess * guess - x) > epsilon) {
                guess = 0.5 * (guess + x / guess); // Apply Newton-Raphson iteration
            }

            return guess;
        }
        
        float FastTan(float x) 
        {
            // Map the angle to a range of [-pi/2, pi/2]
            x = fmodf(x, 3.1415);

            if (x > 3.1415 / 2.0f)
                x -= 3.1415;

            if (x < -3.1415 / 2.0f)
                x += 3.1415;

            // Calculate the tangent using a polynomial approximation
            float x2 = x * x;
            float x3 = x * x2;
            float x5 = x3 * x2;
            float x7 = x5 * x2;

            return x + (1.0f / 3.0f) * x3 + (2.0f / 15.0f) * x5 + (17.0f / 315.0f) * x7;
        }
        bool CheckCollisionEntity(Entity* e, Vector2 point)
        {
            float dist = FastSquareRoot(pow((e->GetPos().x - point.x), 2) + pow((e->GetPos().y - point.y), 2));
            return dist < e->GetRadius();
        }

        Vector2* CalculateBoundingTriangle(Vector2* coords) 
        {
            float rFov = fov * (3.1416/180) * 0.5f;
            float hypotenuseLength = viewDist / cosf(rFov);

            coords[0] = pos;
            coords[1] = {(hypotenuseLength * cosf(orientation + rFov)) + pos.x, hypotenuseLength * (sinf(orientation + rFov)) + pos.y};
            coords[2] = {(hypotenuseLength * cosf(orientation - rFov)) + pos.x, hypotenuseLength * (sinf(orientation - rFov)) + pos.y};;

            /*DrawCircle(coords[0].x, coords[0].y, 5, PURPLE);
            DrawCircle(coords[1].x, coords[1].y, 5, PURPLE);
            DrawCircle(coords[2].x, coords[2].y, 5, PURPLE);
            DrawCircle(viewDist + pos.x, 500, 5, PURPLE);*/

            return coords;
        }

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

        bool IsPointInsideTriangle(Vector2 p, Vector2 a, Vector2 b, Vector2 c) 
        {
            // Calculate the barycentric coordinates of point p.
            float detT = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
            float alpha = ((b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y)) / detT;
            float beta = ((c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y)) / detT;
            float gamma = 1.0f - alpha - beta;

            // Check if the point is inside the triangle.
            return (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f);
        }


    public:

        //use this for first gen
        Entity(bool inIsPrey, int xBound, int yBound)
        {
            for(int i = 0; i < 12; ++i)
            {
                sight[i] = std::make_tuple(nullptr, 0);
                wallSight[i] = std::make_tuple(nullptr, 0);
                foodSight[i] = std::make_tuple(nullptr, 0);
            }
            killCount = 0;
            rotationDir = 0;
            linearSpeed = 0;
            angularSpeed = 0;
            neuralNet = new NeuralNetwork(24, 3, 1, 12);
            netToPassOn = nullptr;
            birthTime = clock();

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> xDist(50, xBound);
            std::uniform_int_distribution<int> yDist(50, yBound);
            std::uniform_int_distribution<int> size(5,15);

            pos = {(float)xDist(mt), (float)yDist(mt)};
            radius = size(mt);
            damage = (radius * radius) / 50.0f;
            health = radius * radius * 0.85f;
            speedMultiplyer = 100.0f / health;
            orientation = 0;
            isPrey = inIsPrey;
            willReproduce = false;
            gameTicksSurvived = 0;
            parent = nullptr;
            lineageCount = 0;
            

        }

         Entity(Vector2 inPos, bool inIsPrey)
        {
            for(int i = 0; i < 12; ++i)
            {
                sight[i] = std::make_tuple(nullptr, 0.0f);
                wallSight[i] = std::make_tuple(nullptr, 0.0f);
                wallSight[i] = std::make_tuple(nullptr, 0.0f);
            }
            killCount = 0;
            rotationDir = 0;
            linearSpeed = 0;
            angularSpeed = 0;
            neuralNet = new NeuralNetwork(24, 3, 1, 12);
            netToPassOn = nullptr;
            birthTime = clock();

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> size(5,15);


            radius = size(mt);
            damage = (radius * radius) / 50.0f;
            health = radius * radius * 0.85f;
            speedMultiplyer = 100.0f / health;

            pos = inPos;
            orientation = 0;
            isPrey = inIsPrey;
            willReproduce = false;
            gameTicksSurvived = 0;
            parent = nullptr;
            lineageCount = 0;
        }

        //use this for subsiquent gens
        Entity(Entity* inParent, Vector2 spawnPoint)
        {
            for(int i = 0; i < 12; ++i)
            {
                sight[i] = std::make_tuple(nullptr, 0);
                wallSight[i] = std::make_tuple(nullptr, 0);
                foodSight[i] = std::make_tuple(nullptr, 0);
            }
            killCount = 0;
            rotationDir = 0;
            linearSpeed = 0;
            angularSpeed = 0;
            birthTime = clock();
            
            radius = 10.0f;
            orientation = inParent->GetOrientation();
            pos = spawnPoint;
            isPrey = inParent->GetIsPrey();
            willReproduce = false;
            gameTicksSurvived = 0;
            netToPassOn = nullptr;
            SetParent(inParent);
            neuralNet = inParent->GetNetToPassOn()->CopyNet();
            lineageCount = parent->GetLineageCount() + 1;
            
        }
        ~Entity(){}

        void DeleteNets()
        {
            neuralNet->DeleteContents();
            delete neuralNet;
            neuralNet = nullptr;

            if(netToPassOn != nullptr)
            {
                netToPassOn->DeleteContents();
                delete netToPassOn;
                netToPassOn = nullptr;
            }
        }

        bool CheckCollisionWall(Wall* wall, Vector2 loc, float r)
        {
            Vector2* coords = wall->GetCoords();
            bool collisionDetected = false;

            if(CheckCircleAndLineSegmentCollision(loc, r, coords[0], coords[1]))
            {
                return true;
            }
            else if(CheckCircleAndLineSegmentCollision(loc, r, coords[0], coords[2]))
            {
                return true;
            }
            else if(CheckCircleAndLineSegmentCollision(loc, r, coords[1], coords[3]))
            {
                return true;
            }
            else if(CheckCircleAndLineSegmentCollision(loc, r, coords[2], coords[3]))
            {
                return true;
            }
            else
            {
                return false;
            }
        }


        //do the neural network stuff
        void ProcessInput()
        {   
            //feed inputs into neural net
            std::vector<float> input;
            for(int i = 0; i < 12; ++i)
            {
                input.push_back(std::get<1>(sight[i]));
            }
            for(int i = 0; i < 12; ++i)
            {
                
                input.push_back(ReturnWallorFood(wallSight[i], foodSight[i]));
            }
            neuralNet->ComputeOutputLayer(input);

            //writeback outputs
            rotationDir = round(neuralNet->returnOutput()[0]->getValue());
            angularSpeed = neuralNet->returnOutput()[1]->getValue();
            linearSpeed = neuralNet->returnOutput()[2]->getValue();

            hadDinnerThisTick = false;

            if(false)
            {
                //std::cout << rotationDir << ", " << angularSpeed << ", " << linearSpeed << std::endl;
                std::cout << "[ ";
                for(int i = 0; i < input.size(); ++i)
                {
                    if(i == input.size() - 1)
                    {
                        std::cout << input[i];
                    }
                    else
                    {
                        std::cout << input[i] << ", ";
                    }
                }
                std::cout << " ]" << std::endl;
            }
        }

        void drawEntity()
        {   
            bool debug = false;
            Color g;
            Color r;
            Color b;
            if(isTruman)
            {
                b = BLUE;
                b.a = ((health / pow(radius, 2)) * 255);
                DrawCircle((int)pos.x, (int)pos.y, radius + 3, b);
            }
            if(isPrey)
            {
                g = GREEN;
                g.a = ((health / pow(radius, 2)) * 255);
                DrawCircle(pos.x, pos.y, radius, g);
            }
            else
            {
                r = RED;
                r.a = ((health / pow(radius, 2)) * 255);
                DrawCircle((int)pos.x, (int)pos.y, radius, r);
            }
            Vector2 v1 = {(radius * (float)cos(orientation)) + pos.x, (radius * (float)sin(orientation)) + pos.y};
            Vector2 v2 = {(radius * (float)cos(((2*3.1416)/3) + orientation)) + pos.x, (radius * (float)sin(((2*3.1416)/3) + orientation)) + pos.y};
            Vector2 v3 = {(radius * (float)cos(((4*3.1416)/3) + orientation)) + pos.x, (radius * (float)sin(((4*3.1416)/3) + orientation)) + pos.y};
            DrawTriangle(v3, v2, v1, GRAY);

            if(debug)
            {
                DrawText((std::to_string((int)pos.x) + ", " + std::to_string((int)pos.y)).c_str(), pos.x, pos.y, 10, BLACK);
            }

            //std::cout << v1.x << " " << v1.y << " " << v2.x << " " << v2.y << " " << v3.x << " " << v3.y << std::endl;
        }

        NeuralNetwork* GetNetToPassOn()
        {
            return netToPassOn;
        }

        NeuralNetwork* GetNeuralNetwork()
        {
            return neuralNet;
        }
        

        Vector2 GetPos()
        {
            return pos;
        }
        
        void UpdateOrientation()
        {
            if(rotationDir)
            {
                orientation -= angularSpeed * speedMultiplyer;
            }
            else
            {
                orientation += angularSpeed * speedMultiplyer;
            }
        }
        
        void ChangeRotationDir()
        {
            rotationDir = !rotationDir;
        }

        void UpdatePosMove(std::vector<Wall*> inWalls)
        {   
            bool wallCollision = false;
            Vector2 newCoords = {(linearSpeed * (float)(cos(orientation)) * speedMultiplyer) + pos.x, (linearSpeed * (float)(sin(orientation)) * speedMultiplyer) + pos.y};
            for(Wall* wall : inWalls)
            {
                if(CheckCollisionWall(wall, newCoords, radius))
                {
                    wallCollision = true;
                    break;
                }
            }
            if(!wallCollision)
            {
                pos.x += linearSpeed * (float)(cos(orientation)) * speedMultiplyer;
                pos.y += linearSpeed * (float)(sin(orientation)) * speedMultiplyer;
            }
        }

        void SetLinearSpeed(float inSpeed)
        {
            linearSpeed = inSpeed;
        }

        void setPos(int x, int y)
        {
            pos.x = x;
            pos.y = y;
        }

        bool GetIsPrey()
        {
            return isPrey;
        }

        void CollectRayData(std::vector<Entity*> entities, std::vector<Wall*> wallList, std::vector<Food*> foodList, Vector2 arenaSize)
        {
            entitiesOfInterest.clear();
            foodOfInterest.clear();
            sightPoints.clear();

            float rfov = fov * (3.1416/180);

            Vector2 triangleCoords[3];

            Vector2* boundingTriangle = CalculateBoundingTriangle(triangleCoords);

            //We dont need to check collisions with ourself
            entities.erase(std::remove(entities.begin(), entities.end(), this), entities.end());

            //Remove all the entities and food that arent either inside the bounding triangle or colliding with it
            for(int i = 0; i < entities.size(); ++i)
            {
                Vector2 entPos = entities[i]->GetPos();
                float entRadius = entities[i]->GetRadius();

                //If ent is not 
                bool keepForConsideration = false;
                if(IsPointInsideTriangle(entPos, boundingTriangle[0], boundingTriangle[1], boundingTriangle[2]))
                {
                    keepForConsideration = true;
                }
                
                //If the center of the ent is not inside the bounding triangle, check to see if it it at least intersects it
                else if(CheckCircleAndLineSegmentCollision(entPos, entRadius, boundingTriangle[0], boundingTriangle[1]))
                {
                    keepForConsideration = true;
                }
                else if(CheckCircleAndLineSegmentCollision(entPos, entRadius, boundingTriangle[0], boundingTriangle[2]))
                {
                    keepForConsideration = true;
                }
                else if(CheckCircleAndLineSegmentCollision(entPos, entRadius, boundingTriangle[1], boundingTriangle[2]))
                {
                    keepForConsideration = true;
                }
                if(keepForConsideration)
                {
                    entitiesOfInterest.push_back(entities[i]);
                }
            }

            for(int i = 0; i < foodList.size(); ++i)
            {
                Vector2 foodPos = foodList[i]->GetPos();
                float foodRadius = foodList[i]->GetRadius();

                //If food is not inside triangle
                bool keepForConsiderationFood = false;
                if(IsPointInsideTriangle(foodPos, boundingTriangle[0], boundingTriangle[1], boundingTriangle[2]))
                {
                    keepForConsiderationFood = true;
                }

                //If the center of the ent is not inside the bounding triangle, check to see if it it at least intersects it
                else if(CheckCircleAndLineSegmentCollision(foodPos, foodRadius, boundingTriangle[0], boundingTriangle[1]))
                {
                    keepForConsiderationFood = true;
                }
                else if(CheckCircleAndLineSegmentCollision(foodPos, foodRadius, boundingTriangle[0], boundingTriangle[2]))
                {
                    keepForConsiderationFood = true;
                }
                else if(CheckCircleAndLineSegmentCollision(foodPos, foodRadius, boundingTriangle[1], boundingTriangle[2]))
                {
                    keepForConsiderationFood = true;
                }
                if(keepForConsiderationFood)
                {
                    foodOfInterest.push_back(foodList[i]);
                }
            }

            bool doSight = true;
            bool doFood = true;
            if(entitiesOfInterest.size() == 0)
            {
                doSight = false;
            }
            if(foodOfInterest.size() == 0)
            {
                doFood = false;
            }

            //Start placing rays and checking if things are colliding with them
            for(int currentRayindex = 0; currentRayindex < 12; ++currentRayindex)
            {   
                //need direction of ray to convert to x and y coords
                float rayDir = orientation + (0.5 * rfov) - (currentRayindex * rfov / 11);

                bool entCollisionDetected = false;
                bool wallCollisionDetected = false;
                bool foodCollisionDetected = false;
                for(int point = 0; point <= viewDist; point += 9)
                {   
                    
                    //Get the location of the ccurrent ray point
                    Vector2 currentPoint = {(point * cos(rayDir) + pos.x), (point * sin(rayDir) + pos.y)};

                    //First check if the point hits a wall before checking if it hits other entities
                    for(int i = 0; i < wallList.size(); ++i)
                    {   
                        wallCollisionDetected = CheckCollisionWall(wallList[i], currentPoint, 5);
                        if(wallCollisionDetected)
                        {
                            wallSight[currentRayindex] = std::make_tuple(wallList[i], (-1.0f * (1.0f - ((float)point / viewDist))));
                            foodSight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                            sight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                            break;
                        }
                        else
                        {
                            wallSight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                        }
                    }
                    if(wallCollisionDetected)
                    {
                        break;
                    }

                    //First check if the point hits a food before checking if it hits other entities
                    if(doFood)
                    {
                        for(int i = 0; i < foodOfInterest.size(); ++i)
                        {   
                            Vector2 foodPos = foodOfInterest[i]->GetPos();
                            foodCollisionDetected = CheckCollisionPointCircle(currentPoint, foodPos, foodOfInterest[i]->GetRadius());
                            if(foodCollisionDetected)
                            {
                                foodSight[currentRayindex] = std::make_tuple(foodOfInterest[i], (1.0f - ((float)point / viewDist)));
                                wallSight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                                sight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                                break;
                            }
                            else
                            {
                                foodSight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                            }
                        }
                        if(foodCollisionDetected)
                        {
                            break;
                        }
                    }
                    else
                    {
                        foodSight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                    }

                    //Check collisions with all other entities
                    if(doSight)
                    {
                        for(int i = 0; i < entitiesOfInterest.size(); ++i)
                        {
                            entCollisionDetected = CheckCollisionEntity(entitiesOfInterest[i], currentPoint);
                            if(entCollisionDetected)
                            {
                                if(entitiesOfInterest[i]->GetIsPrey())
                                {
                                    sight[currentRayindex] = std::make_tuple(entitiesOfInterest[i], (-1.0f * (1.0f - ((float)point / viewDist))));
                                }
                                else
                                {
                                    sight[currentRayindex] = std::make_tuple(entitiesOfInterest[i], (1.0f - ((float)point / viewDist)));
                                    
                                }
                                wallSight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                                foodSight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                                break;
                            }
                            else
                            {
                                sight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                            }
                        }
                        if(entCollisionDetected)
                        {
                            break;
                        }
                    }
                    else
                    {
                        sight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                    }

                    sightPoints.push_back(currentPoint);
                }
            }
        }

        float GetRadius()
        {
            return radius;
        }

        void SetRadius(int r)
        {
            radius = r;
        }

        void IncrementHealth(float change)
        {
            health += change;
        }

        void SetHealth(int h)
        {
            health = h;
        }

        float GetHealth()
        {
            return health;
        }

        void Teleport(Vector2 newPos)
        {
            pos = newPos;
        }

        void SetAttack(bool inAttack)
        {
            attack = inAttack;
        }

        //max input value is 1000, min is 0
        void CreateNetToPassOn(int mutationRate)
        {
            int mutationChance;
            if(mutationRate > 1000)
            {
                mutationChance = 1000;
            }
            else if(mutationRate < 0)
            {
                mutationChance = 0;
            }
            else
            {
                mutationChance = mutationRate;
            }
            NeuralNetwork* mutatedNet = neuralNet->CopyNet();
            std::vector<Weight*> weights = mutatedNet->GetWeights();
            std::vector<Neuron*> neurons = mutatedNet->GetNeurons();

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> dist (0,1000);
            
            float chance;
            Weight* currentWeight;
            Neuron* currentNeuron;

            for(int weightIndex = 0; weightIndex < weights.size(); ++weightIndex)
            {
                chance = dist(mt);
                if(chance < mutationChance)
                {   
                    currentWeight = weights[weightIndex];
                    if(chance < (mutationChance / 2))
                    { 
                        currentWeight->setWeight(currentWeight->getWeight() - (chance / 100.0f));
                    }
                    else
                    {
                        currentWeight->setWeight(currentWeight->getWeight() + (chance / 100.0f));
                    }
                }
            }
            for(int neuronIndex = 0; neuronIndex < neuralNet->GetNeurons().size(); ++neuronIndex)
            {   
                currentNeuron = neurons[neuronIndex];
                if(currentNeuron->GetIsHidden())
                {
                    chance = dist(mt);
                    if(chance < mutationChance)
                    {
                        if(chance < (mutationChance / 2))
                        {
                            currentNeuron->SetBias(currentNeuron->getValue() - (chance / 100.0f));
                        }
                        else
                        {
                            currentNeuron->SetBias(currentNeuron->getValue() + (chance / 100.0f));
                        }
                    }
                }
            }
            netToPassOn = mutatedNet;
        }

        Entity* Reproduce(Vector2 spawnLocation, int rate)
        { 
            CreateNetToPassOn(rate);
            ++children;
            Entity* child = new Entity(this, spawnLocation);

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> flag(0,1);
            std::uniform_int_distribution<int> dist(10,100);
            float childR;
            float childVD;
            if(flag(mt))
            {
                childR = radius - (rate / dist(mt));
                if(childR < 5.0f)
                {
                    childR = 5.0f;
                }
                child->SetRadius(childR);
            }
            else
            {
                childR = radius + (rate / dist(mt));
                if(childR > 15.0f)
                {
                    childR = 15.0f;
                }
                child->SetRadius(childR);
            }
            child->SetSpeedMultiplyer(100.0f / (childR * childR));
            child->SetDamage((childR * childR) / 50.0f);
            child->SetHealth(childR * childR * 0.85f);
            ticksSinceReproduction = 0;
            
            return child;
        }

        void IncrementTicksSurvived()
        {
            ++gameTicksSurvived;
        }

        int GetTicksSurvived()
        {
            return gameTicksSurvived;
        }

        int GetKillCount()
        {
            return killCount;
        }

        int GetChildren()
        {
            return children;
        }

        void SetParent(Entity* inParent)
        {
            parent = inParent;
        }

        Entity* GetParent()
        {
            return parent;
        }

        void SetNeuralNetwork(NeuralNetwork* net)
        {
            neuralNet = net;
        }

        void SetTicksSurvived(int ticks)
        {
            gameTicksSurvived = ticks;
        }

        void SetKillCount(int count)
        {
            killCount = count;
        }
        bool DidHaveDinnerThisTick()
        {
            return hadDinnerThisTick;
        }

        int GetTicksSinceDinner()
        {
            return ticksSinceDinner;
        }

        void IncrementTicksSinceDinner()
        {
            ++ticksSinceDinner;
        }

        bool GetIsTruman()
        {
            return isTruman;
        }
        
        void SetIsTruman()
        {
            isTruman = true;
        }

        int GetLineageCount()
        {
            return lineageCount;
        }

        void SetSpeedMultiplyer(float v)
        {
            speedMultiplyer = v;
        }

        float GetSpeedMultiplyer()
        {
            return speedMultiplyer;
        }

        void SetDamage(float d)
        {
            damage = d;
        }

        float GetDamage()
        {
            return damage;
        }
        
        void SetViewDist(float vd)
        {
            viewDist = vd;
        }

        float GetViewDist()
        {
            return viewDist;
        }

        void SetFOV(float f)
        {
            fov = f;
        }

        float GetFOV()
        {
            return fov;
        }

        std::tuple <Entity*, float>* GetSight()
        {
            return sight;
        }

        std::vector<Entity*> GetEntitiesOfInterest()
        {
            return entitiesOfInterest;
        }

        void SetDinnerThisTick()
        {
            hadDinnerThisTick = true;
        }

        void ResetTicksSinceDinner()
        {
            ticksSinceDinner = 0;
        }

        void SetAngularSpeed(float speed)
        {
            angularSpeed = speed;
        }

        float GetOrientation()
        {
            return orientation;
        }

        void IncrementTicksSinceReproduction()
        {
            ticksSinceReproduction++;
        }

        int GetTicksSinceReproduction()
        {
            return ticksSinceReproduction;
        }
        
        void SetTrumanFalse()
        {
            isTruman = false;
        }

        std::vector<Vector2> GetSightPoints()
        {
            return sightPoints;
        }
};



#endif