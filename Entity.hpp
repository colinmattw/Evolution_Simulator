#ifndef Entity_HPP
#define Entity_HPP
#include "NeuralNetwork.hpp"
#include <ctime>
#include <vector>
#include "raylib.h"
#include <cmath>
#include <tuple>

class Entity
{
    private:
        //input neurons
        int health;
        std::tuple <Entity*, float> sight[12];

        //output neurons
        bool rotationDir;
        bool attack;
        float linearSpeed;
        float angularSpeed;

        //genotype
        NeuralNetwork* neuralNet;
        NeuralNetwork* netToPassOn;
        bool isPrey;

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
        bool hadDinnerThisTick = false;
        int ticksSinceDinner = 0;
        bool isTruman = false;

        bool CheckCollisionEntity(Entity* e, Vector2 point)
        {
            float dist = sqrt(pow((e->GetPos().x - point.x), 2) + pow((e->GetPos().y - point.y), 2));
            return dist < e->GetRadius();
        }

    public:

        //use this for first gen
        Entity(bool inIsPrey)
        {
            if(inIsPrey)
            {
                health = 10;
            }
            else
            {
                health = 100;
            }
            for(int i = 0; i < 12; ++i)
            {
                std::make_tuple(nullptr, 0);
            }
            killCount = 0;
            rotationDir = 0;
            attack = 0;
            linearSpeed = 0;
            angularSpeed = 0;
            neuralNet = new NeuralNetwork(13, 4, 1, 8);
            netToPassOn = nullptr;
            birthTime = clock();

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> dist(0,1000);

            pos = {(float)dist(mt), (float)dist(mt)};
            radius = 5.0f;
            orientation = 0;
            isPrey = inIsPrey;
            willReproduce = false;
            gameTicksSurvived = 0;
            parent = nullptr;

        }

        Entity(Entity* inParent)
        {
            if(inParent->GetIsPrey())
            {
                health = 10;
            }
            else
            {
                health = 100;
            }
            for(int i = 0; i < 12; ++i)
            {
                std::make_tuple(nullptr, 0);
            }
            killCount = 0;
            rotationDir = 0;
            attack = 0;
            linearSpeed = 0;
            angularSpeed = 0;
            if(inParent->GetNetToPassOn() == nullptr)
            {   
                inParent->CreateNetToPassOn(4);
                neuralNet = inParent->GetNetToPassOn()->CopyNet();
            }
            else
            {
                neuralNet = inParent->GetNetToPassOn()->CopyNet();
            }

            netToPassOn = nullptr;
            birthTime = clock();

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> dist(0,1000);

            pos = {(float)dist(mt), (float)dist(mt)};
            radius = 5.0f;
            orientation = 0;
            isPrey = inParent->GetIsPrey();
            willReproduce = false;
            gameTicksSurvived = 0;
            SetParent(inParent);

        }

         Entity(Vector2 inPos, bool inIsPrey)
        {
            if(inIsPrey)
            {
                health = 10;
            }
            else
            {
                health = 100;
            }
            for(int i = 0; i < 12; ++i)
            {
                sight[i] = std::make_tuple(nullptr, 0.0f);
            }
            killCount = 0;
            rotationDir = 0;
            attack = 0;
            linearSpeed = 3;
            angularSpeed = 0.025;
            neuralNet = new NeuralNetwork(13, 4, 1, 8);
            netToPassOn = nullptr;
            birthTime = clock();


            pos = inPos;
            radius = 5.0f;
            orientation = 0;
            isPrey = inIsPrey;
            willReproduce = false;
            gameTicksSurvived = 0;
            parent = nullptr;
        }

        //use this for subsiquent gens
        Entity(Entity* inParent, Vector2 spawnPoint)
        {
            if(inParent->GetIsPrey())
            {
                health = 10;
            }
            else
            {
                health = 100;
            }
            for(int i = 0; i < 12; ++i)
            {
                std::make_tuple(nullptr, 0);
            }
            killCount = 0;
            rotationDir = 0;
            attack = 0;
            linearSpeed = 0;
            angularSpeed = 0;
            if(inParent->GetNetToPassOn() == nullptr)
            {   
                inParent->CreateNetToPassOn(4);
                neuralNet = inParent->GetNetToPassOn()->CopyNet();
            }
            else
            {
                neuralNet = inParent->GetNetToPassOn()->CopyNet();
            }
            birthTime = clock();

            radius = 5.0f;
            orientation = 0;
            pos = spawnPoint;
            isPrey = inParent->GetIsPrey();
            willReproduce = false;
            gameTicksSurvived = 0;
            netToPassOn = nullptr;
            SetParent(inParent);
            
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

        //do the neural network stuff
        void ProcessInput()
        {   
            //feed inputs into neural net
            std::vector<float> input;
            for(int i = 0; i < 12; ++i)
            {
                input.push_back(std::get<1>(sight[i]));
            }
            if(isPrey)
            {
                input.push_back((float)(health / 10));
            }
            else
            {
                input.push_back((float)(health / 100));
            }

            neuralNet->ComputeOutputLayer(input);

            //writeback outputs
            rotationDir = round(neuralNet->returnOutput()[0]->getValue());
            angularSpeed = neuralNet->returnOutput()[1]->getValue();
            attack = round(neuralNet->returnOutput()[2]->getValue());
            linearSpeed = neuralNet->returnOutput()[3]->getValue();

            hadDinnerThisTick = false;
        }

        void drawEntity()
        {   
            if(isPrey)
            {
                DrawCircle((int)pos.x, (int)pos.y, radius, GREEN);
            }
            else
            {
                if(isTruman)
                {
                    DrawCircle((int)pos.x, (int)pos.y, radius + 2, BLUE);
                    DrawCircle((int)pos.x, (int)pos.y, radius, RED);
                }
                else
                {
                    DrawCircle((int)pos.x, (int)pos.y, radius, RED);
                }
            }
            Vector2 v1 = {(radius * (float)cos(orientation)) + pos.x, (radius * (float)sin(orientation)) + pos.y};
            Vector2 v2 = {(radius * (float)cos(((2*3.1416)/3) + orientation)) + pos.x, (radius * (float)sin(((2*3.1416)/3) + orientation)) + pos.y};
            Vector2 v3 = {(radius * (float)cos(((4*3.1416)/3) + orientation)) + pos.x, (radius * (float)sin(((4*3.1416)/3) + orientation)) + pos.y};
            DrawTriangle(v3, v2, v1, GRAY);
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
                orientation -= angularSpeed;
            }
            else
            {
                orientation += angularSpeed;
            }
        }
        
        void ChangeRotationDir()
        {
            rotationDir = !rotationDir;
        }

        void UpdatePosMove()
        {   
            
            pos.x += linearSpeed * (float)(cos(orientation));
            pos.y += linearSpeed * (float)(sin(orientation));
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

        void CollectRayData(std::vector<Entity*> entities, Vector2 arenaSize)
        {
            //Fixes bug where entities are seeing other ents that have been killed
            if((entities.size() == 1) && (entities.front() == this))
            {
                for(int i = 0; i < 12; ++i)
                {
                    sight[i] = std::make_tuple(nullptr, 0.0f);
                }
            }
            else
            {
                float fov = 90;
                fov = fov * (3.1416/180);
                int raylength = 75;

                std::vector<Entity*> entitiesOfInterest;
                entities.erase(std::remove(entities.begin(), entities.end(), this), entities.end());
                for(int i = 0; i < entities.size(); ++i)
                {
                    int x = entities[i]->GetPos().x;
                    int y = entities[i]->GetPos().y;

                    int conditionsMet = 0;
                    if( sqrt(std::pow((pos.x - x), 2) + std::pow((pos.y - y), 2)) <= raylength)
                    {
                        ++conditionsMet;
                    }

                    if((arenaSize.x - abs(pos.x - x)) < raylength)
                    {
                        conditionsMet += 2;
                    }
                    if((arenaSize.y - abs(pos.y - y)) < raylength)
                    {
                        ++conditionsMet += 2;
                    }

                    //Check the angle the entity in question makes with the orientation of this entity
                    float slope_ent = (y - pos.y) / (x - pos.x);
                    float slope_this = tan(orientation);
                    float angle = atan((slope_ent - slope_this) / (1 + (slope_ent * slope_this)));
                    if(abs(angle) <= (0.7 * fov))
                    {
                        Vector2 ent_localcoords;
                        //First apply a translation to the coords
                        ent_localcoords = {(x - pos.x), (y - pos.y)};

                        //Next apply a rotation to match this entity's local coordinate system
                        ent_localcoords.x = ent_localcoords.x * cos(orientation) + ent_localcoords.y * sin(orientation);
                        ent_localcoords.y = ((-1) * ent_localcoords.x * sin(orientation)) + ent_localcoords.y * cos(orientation);

                        //Check to see if the local x coordinate is negative (to see if it is behind the entity)
                        if(ent_localcoords.x >= 0)
                        {
                            ++conditionsMet;
                        }
                    }

                    //If enough conditions are met, add the entity to the list of entities to be considered for collision
                    if(conditionsMet >= 2)
                    {
                        entitiesOfInterest.push_back(entities[i]);
                    }
                }
                //std::cout << entitiesOfInterest.size() << std::endl;
                bool doTheRest = true;

                //std::cout << entitiesOfInterest.size() << std::endl;

                if(entitiesOfInterest.size() == 0)
                {
                    for(int i = 0; i < 12; ++i)
                    {
                        sight[i] = std::make_tuple(nullptr, 0.0f);
                    }
                    doTheRest = !doTheRest;
                }
                if(doTheRest)
                {
                    for(int currentRayindex = 0; currentRayindex < 12; ++currentRayindex)
                    {   
                        //need direction of ray to convert to x and y coords
                        float rayDir = orientation + (0.5 * fov) - (currentRayindex * fov / 11);

                        bool collisionDetected = false;
                        int clostestEntityDist = 0;
                        for(int point = 0; point < raylength; point += radius)
                        {   
                            
                            //find the location of the ccurrent ray point
                            Vector2 currentPoint = {((radius + point) * cos(rayDir) + pos.x), ((radius + point) * sin(rayDir) + pos.y)};

                            //Need to wrap the vision around the edge arena if an entity is on the edge in order to maintain consistency
                            if(currentPoint.x < 0)
                            {
                                currentPoint.x = currentPoint.x + arenaSize.x;
                            }
                            if(currentPoint.x > arenaSize.x)
                            {
                                currentPoint.x = currentPoint.x - arenaSize.x;
                            }
                            if(currentPoint.y < 0)
                            {
                                currentPoint.y = currentPoint.y + arenaSize.y;
                            }
                            if(currentPoint.y > arenaSize.y)
                            {
                                currentPoint.y = currentPoint.y - arenaSize.y;
                            }

                            //check collisions with all other entities
                            for(int i = 0; i < entitiesOfInterest.size(); ++i)
                            {
                                if(entitiesOfInterest[i] != this)
                                {
                                    collisionDetected = CheckCollisionEntity(entitiesOfInterest[i], currentPoint);
                                    if(collisionDetected)
                                    {
                                        if(entitiesOfInterest[i]->GetIsPrey())
                                        {
                                            sight[currentRayindex] = std::make_tuple(entitiesOfInterest[i], (-1.0f * (1.0f - ((float)point / (float)raylength))));
                                            if(isTruman)
                                            {
                                                std::cout << std::get<1>(sight[i]) << std::endl;
                                            }
                                            
                                        }
                                        else
                                        {
                                            sight[currentRayindex] = std::make_tuple(entitiesOfInterest[i], (1.0f - ((float)point / (float)raylength)));
                                            if(isTruman)
                                            {
                                                std::cout << std::get<1>(sight[i]) << std::endl;
                                            }
                                            
                                        }
                                        break;
                                    }
                                    sight[currentRayindex] = std::make_tuple(nullptr, 0.0f);
                                }
                            }
                            if(collisionDetected)
                            {
                                break;
                            }
                            if(isTruman)
                            {
                                DrawCircle(currentPoint.x, currentPoint.y, 2.0f, GRAY);
                            }
                        }
                    }
                }
            }
        }

        float GetRadius()
        {
            return radius;
        }

        void UpdateHealth(int damage)
        {
            health -= damage;
        }

        int GetHealth()
        {
            return health;
        }

        void Attack()
        {   
            if(!isPrey)
            {
                std::tuple<Entity*, int> closestEntity = std::make_tuple(nullptr, 0);
                for(int i = 0; i < 12; ++i)
                {   
                    if(abs(std::get<1>(sight[i])) > abs(std::get<1>(closestEntity)))
                    {
                        closestEntity = sight[i];
                    }
                }
                if(std::get<0>(closestEntity) != nullptr)
                {
                    if(std::get<0>(closestEntity)->GetIsPrey())
                    {
                        if(abs(std::get<1>(closestEntity)) > 0.90f)
                        {
                            std::get<0>(closestEntity)->UpdateHealth(1);
                        }
                        if(std::get<0>(closestEntity)->GetHealth() <= 0)
                        {
                            ++killCount;
                            hadDinnerThisTick = true;
                            ticksSinceDinner = 0;
                            health = 100;
                        }
                    }
                }
            }
        }

        void Teleport(Vector2 newPos)
        {
            pos = newPos;
        }

        bool WillAttack()
        {
            return attack;
        }
        void SetAttack(bool inAttack)
        {
            attack = inAttack;
        }

        //max input value is 100, min is 0
        void CreateNetToPassOn(int mutationRate)
        {
            int mutationChance;
            if(mutationRate > 100)
            {
                mutationChance = 100;
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
            std::uniform_int_distribution<int> dist (0,100);
            
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
                        currentWeight->setWeight(currentWeight->getWeight() - (float)(dist(mt) / 50.0f));
                    }
                    else
                    {
                        currentWeight->setWeight(currentWeight->getWeight() + (float)(dist(mt) / 50.0f));
                    }
                }
            }
            for(int neuronIndex = 0; neuronIndex < neuralNet->GetNeurons().size(); ++neuronIndex)
            {   
                chance = dist(mt);
                if(chance < mutationChance)
                {
                    currentNeuron = neurons[neuronIndex];
                    if(chance < (mutationChance / 2))
                    {
                        currentNeuron->SetBias(currentNeuron->getValue() - (float)(dist(mt) / 50.0f));
                    }
                    else
                    {
                        currentNeuron->SetBias(currentNeuron->getValue() + (float)(dist(mt) / 50.0f));
                    }
                }
            }
            netToPassOn = mutatedNet;
        }

        Entity* Reproduce(Vector2 spawnLocation)
        {   
            CreateNetToPassOn(4);
            ++children;
            return new Entity(this, spawnLocation);
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
};



#endif