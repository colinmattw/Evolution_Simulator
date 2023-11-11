#ifndef GAME_HPP
#define GAME_HPP
#include "Entity.hpp"
#include "Wall.hpp"
#include "Food.hpp"
#include "raylib.h"
#include <vector>
#include <algorithm>

class Simulation
{
    private:
        int ticks;
        int preyPop;
        int predPop;
        Vector2 size;
        std::vector<Entity*> preyList;
        std::vector<Entity*> predList;
        std::vector<Food*> foodList;
        Entity* lastPrey;
        Entity* lastPred;
        std::vector<Neuron*> trumanNeurons;
        std::vector<Weight*> trumanWeights;
        std::vector<Wall*> outerWalls;

    public:

        Simulation(int prey, int pred, int xSize, int ySize)
        {
            ticks = 0;
            predPop = pred;
            preyPop = prey;
            size = {(float)xSize, (float)ySize};

            for(int i = 0; i < predPop; ++i)
            {   
                Entity* e = new Entity(false, xSize - 50, ySize - 50);
                predList.push_back(e);
            }
            for(int i = 0; i < preyPop; ++i)
            {   
                Entity* e = new Entity(true, xSize - 50, ySize - 50);
                preyList.push_back(e);
            }
            for(int i = 0; i < preyPop; ++i)
            {
                Food* f = new Food(xSize, ySize);
                foodList.push_back(f);
            }

            outerWalls.push_back(new Wall(0, 0, 1000, 0, 20));
            outerWalls.push_back(new Wall(1000, 0, 1000, 1000, 20));
            outerWalls.push_back(new Wall(0, 980, 1000, 980, 20));
            outerWalls.push_back(new Wall(20, 0, 20, 1020, 20));
        }

        void DoGameTick()
        {   
            //use this for random spawing
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> xDist(50, size.x - 50);
            std::uniform_int_distribution<int> yDist(50, size.y - 50);
            std::uniform_int_distribution<int> entFlagGen(0, 1);

            std::vector<Entity*> entitiesToKill;
            std::vector<Entity*> entList;

            entList.reserve(predList.size() + preyList.size());
            entList.insert(entList.end(), predList.begin(), predList.end());
            entList.insert(entList.end(), preyList.begin(), preyList.end());
            bool entFlag;
            if(!ticks % 1000)
            {
                entFlag = entFlagGen(mt);
            }

            if(entFlag)
            {
                entList[predList.size()]->SetIsTruman();
                trumanNeurons = entList[predList.size()]->GetNeuralNetwork()->GetNeurons();
                trumanWeights = entList[predList.size()]->GetNeuralNetwork()->GetWeights();
            }
            else
            {
                entList.front()->SetIsTruman();
                trumanNeurons = entList.front()->GetNeuralNetwork()->GetNeurons();
                trumanWeights = entList.front()->GetNeuralNetwork()->GetWeights();
            }

            for(int currentEntIndex = 0; currentEntIndex < entList.size(); ++currentEntIndex)
            {   
                //collect input for each entity
                Entity* currentEnt = entList[currentEntIndex];
                currentEnt->CollectRayData(entList, outerWalls, foodList, size);
               
                //Process the neural network inputs
                currentEnt->ProcessInput();
            }
            for(int currentEntIndex = 0; currentEntIndex < entList.size(); ++currentEntIndex)
            {   
                //Execute move for each entity
                Entity* currentEnt = entList[currentEntIndex];
                currentEnt->UpdateOrientation();
                currentEnt->UpdatePosMove(outerWalls);
                Vector2 currentEntPos = currentEnt->GetPos();

                //Damage everyone for merely existing
                currentEnt->IncrementHealth(-0.05 * currentEnt->GetDamage());

                //Damage all prey colliding with predator
                if(!currentEnt->GetIsPrey())
                {
                    for(int i = 0; i < preyList.size(); ++i)
                    {
                        if(CheckCollisionCircles(currentEnt->GetPos(), currentEnt->GetRadius(), preyList[i]->GetPos(), preyList[i]->GetRadius()))
                        {
                            preyList[i]->IncrementHealth(-1.0f * currentEnt->GetDamage());
                            if(preyList[i]->GetHealth() <= 0)
                            {
                                currentEnt->SetDinnerThisTick();
                                currentEnt->ResetTicksSinceDinner();
                                currentEnt->SetKillCount(currentEnt->GetKillCount() + 1);
                                currentEnt->IncrementHealth(pow(preyList[i]->GetRadius(), 2));
                                if(currentEnt->GetHealth() > pow(currentEnt->GetRadius(), 2))
                                {
                                    currentEnt->SetHealth(pow(currentEnt->GetRadius(), 2));
                                }
                            }
                        }
                    }
                }
                
                //Damage predator if colliding with food
                if(!currentEnt->GetIsPrey())
                {
                    for(int i = 0; i < foodList.size(); ++i)
                    {
                        if(CheckCollisionCircles(currentEnt->GetPos(), currentEnt->GetRadius(), foodList[i]->GetPos(), foodList[i]->GetRadius()))
                        {
                            currentEnt->IncrementHealth( -1.0f * pow(foodList[i]->GetRadius(), 2) / 50.0f);
                        }
                    }
                }


                std::vector<Food*> foodToRemove;
                //Damage all food colliding with prey
                if(currentEnt->GetIsPrey())
                {
                    for(int i = 0; i < foodList.size(); ++i)
                    {
                        if(CheckCollisionCircles(currentEnt->GetPos(), currentEnt->GetRadius(), foodList[i]->GetPos(), foodList[i]->GetRadius()))
                        {
                            foodList[i]->UpdateValueRemaining(currentEnt->GetDamage());
                            if(foodList[i]->GetValueRemaining() <= 0)
                            {
                                currentEnt->SetDinnerThisTick();
                                currentEnt->ResetTicksSinceDinner();
                                currentEnt->SetKillCount(currentEnt->GetKillCount() + 1);
                                currentEnt->IncrementHealth(pow(foodList[i]->GetRadius(), 2));
                                if(currentEnt->GetHealth() > pow(currentEnt->GetRadius(), 2))
                                {
                                    currentEnt->SetHealth(pow(currentEnt->GetRadius(), 2));
                                }

                                foodToRemove.push_back(foodList[i]);
                        
                            }
                        }
                    }
                }
                
                //Remove the eaten food from the simulation
                for(int i = 0; i < foodToRemove.size(); ++i)
                {
                    for(int j = 0; j < foodList.size(); ++j)
                    {
                        if(foodToRemove[i] == foodList[j])
                        {
                            delete foodList[i];
                            foodToRemove[i] = nullptr;
                            foodList.erase(std::remove(foodList.begin(), foodList.end(), nullptr), foodList.end());
                        }
                    }
                }
                foodToRemove.clear();

                //Attempt kill
                if(currentEnt->GetHealth() <= 0)
                {   
                    entitiesToKill.push_back(currentEnt);
                }

                if(!currentEnt->DidHaveDinnerThisTick())
                {
                    currentEnt->IncrementTicksSinceDinner();
                }
            }
            //delete entities with <= 0 health
            for(int entIndex = 0; entIndex < entitiesToKill.size(); ++entIndex)
            {
                Entity* currentEnt = entitiesToKill[entIndex];
                currentEnt->DeleteNets();
                if(currentEnt->GetIsPrey())
                {
                    delete currentEnt;
                    for(int i = 0; i < preyList.size(); ++i)
                    {
                        if(preyList[i] == currentEnt)
                        {
                            preyList[i] = nullptr;
                        }
                    }
                    preyList.erase(std::remove(preyList.begin(), preyList.end(), nullptr), preyList.end());
                }
                else
                {
                    delete currentEnt;
                    for(int i = 0; i < predList.size(); ++i)
                    {
                        if(predList[i] == currentEnt)
                        {
                            predList[i] = nullptr;
                        }
                    }
                    predList.erase(std::remove(predList.begin(), predList.end(), nullptr), predList.end());
                }
            }
            entitiesToKill.clear();

            //Incremnt gameTicksSurvived for everyone thats made it this far and attempt reproduction
            if(preyList.size() == 0)
            {
                for(int _ = 0; _ < preyPop / 2; ++_)
                {
                    preyList.push_back(new Entity({(float)xDist(mt), (float)yDist(mt)}, true));
                }
            }
            else
            {
                for(int i = 0; i < preyList.size(); ++i)
                {
                    preyList[i]->IncrementTicksSurvived();
                    if((preyList[i]->GetTicksSurvived() % 300 == 0) && ((preyList[i]->GetHealth()) > (pow(preyList[i]->GetRadius(), 2)) * 0.75f))
                    {   
                        if(preyList.size() < (4 * preyPop))
                        {   
                            preyList.push_back(preyList[i]->Reproduce({preyList[i]->GetPos().x, preyList[i]->GetPos().y}, 60));
                        }
                    }
                }
            }
            if(predList.size() == 0)
            {   

                for(int _ = 0; _ < predPop / 2; ++_)
                {
                    predList.push_back(new Entity({(float)xDist(mt), (float)yDist(mt)}, false));
                }
                //std::cout << "Pred: " << predList[0]->GetNeuralNetwork()->NetsSimilarityPercentage(lastPred->GetNeuralNetwork()) << std::endl;
            }
            else
            {
                for(int i = 0; i < predList.size(); ++i)
                {   
                    predList[i]->IncrementTicksSurvived();
                    if((predList[i]->GetTicksSurvived() % 300 == 0) && ((predList[i]->GetHealth()) > (pow(predList[i]->GetRadius(), 2)) * 0.75f))
                    {
                        if(predList.size() < (4 * predPop))
                        {
                            predList.push_back(predList[i]->Reproduce({predList[i]->GetPos().x, predList[i]->GetPos().y}, 60));
                        }
                    }
                }
            }
            ++ticks;

        }


        std::vector<Entity*> GetPreyList()
        {
            return preyList;
        }
        std::vector<Entity*> GetPredList()
        {
            return predList;
        }

        Entity* GetLastPrey()
        {
            return lastPrey;
        }

        Entity* LastPred()
        {
            return lastPred;
        }
        std::vector<Neuron*> GetTrumanNeurons()
        {
            return trumanNeurons;
        }
         std::vector<Weight*> GetTrumanWeights()
        {
            return trumanWeights;
        }

        std::vector<Wall*> GetWalls()
        {
            return outerWalls;
        }

        std::vector<Food*> GetFoodList()
        {
            return foodList;
        }
        

};

#endif