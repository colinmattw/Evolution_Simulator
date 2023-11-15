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
        Entity* truman;
        std::vector<Wall*> outerWalls;
        bool preyIsTrumanFlag = true;

    public:

        Simulation(int prey, int pred, int xSize, int ySize)
        {
            ticks = 0;
            predPop = pred;
            preyPop = prey;
            size = {(float)xSize, (float)ySize};

            outerWalls.push_back(new Wall(0, 0, 1000, 0, 20, true));
            outerWalls.push_back(new Wall(1000, 0, 1000, 1000, 20, true));
            outerWalls.push_back(new Wall(0, 980, 1000, 980, 20, true));
            outerWalls.push_back(new Wall(20, 0, 20, 1020, 20, true));

            std::random_device dev;
            std::mt19937 mt(dev());

            std::uniform_int_distribution<int> newPosX(50, xSize - 50);
            std::uniform_int_distribution<int> newPosY(50, ySize - 50);
            for(int _ = 0; _ < 3; ++_)
            {
                outerWalls.push_back(new Wall(50, xSize - 50, 50, ySize - 50, 10));
            }

            for(int i = 0; i < predPop; ++i)
            {   
                Entity* e = new Entity(false, xSize - 50, ySize - 50);
                for(Wall* wall : outerWalls)
                {
                    while(e->CheckCollisionWall(wall, e->GetPos(), e->GetRadius()))
                    {
                        e->Teleport({(float)newPosX(mt), (float)newPosY(mt)});
                    }
                }
                predList.push_back(e);
            }
            for(int i = 0; i < preyPop; ++i)
            {   
                Entity* e = new Entity(true, xSize - 50, ySize - 50);
                for(Wall* wall : outerWalls)
                {
                    while(e->CheckCollisionWall(wall, e->GetPos(), e->GetRadius()))
                    {
                        e->Teleport({(float)newPosX(mt), (float)newPosY(mt)});
                    }
                }
                preyList.push_back(e);
            }
            for(int i = 0; i < preyPop / 2; ++i)
            {
                Food* f = new Food(xSize, ySize);
                for(Wall* wall : outerWalls)
                {
                    while(f->CheckCollisionWall(wall))
                    {
                        f->Teleport({(float)newPosX(mt), (float)newPosY(mt)});
                    }
                }
                foodList.push_back(f);
            }

            predList.front()->SetIsTruman();
            truman = predList.front();

        }

        void DoGameTick()
        {   
            //use this for random spawing
            std::random_device dev;
            std::mt19937 mt(dev());

            std::uniform_int_distribution<int> foodSpawnChance(0, 1000);
            std::uniform_int_distribution<int> xDist(-10, 10);
            std::uniform_int_distribution<int> yDist(-10, 10);

            std::vector<Entity*> entitiesToKill;
            std::vector<Entity*> entList;

            entList.reserve(predList.size() + preyList.size());
            entList.insert(entList.end(), predList.begin(), predList.end());
            entList.insert(entList.end(), preyList.begin(), preyList.end());

            //Attempt to spawn in new food if theres space
            int foodSize = foodList.size();
            for(int i = 0; i < (preyPop - (foodSize * 2)); ++i)
            {
                if(foodSpawnChance(mt) < 25)
                {
                    Food* f = new Food(size.x, size.y);
                    for(Wall* wall : outerWalls)
                    {
                        while(f->CheckCollisionWall(wall))
                        {
                            f->Teleport({(float)xDist(mt) + f->GetPos().x, (float)yDist(mt) + f->GetPos().y});
                        }
                    }
                    foodList.push_back(f);
                }
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

                //Damage everyone for merely existing, based on either how idle they are, or how fast they're spinning
                float linearDamageFactor = 1.1f - (currentEnt->GetNeuralNetwork()->GetOutputLayer()[2]->getValue());
                float angularDamageFactor = currentEnt->GetNeuralNetwork()->GetOutputLayer()[1]->getValue() + 0.1f;
                float damageFactor = fmax(linearDamageFactor, angularDamageFactor);
                currentEnt->IncrementHealth(-0.5f * damageFactor * currentEnt->GetDamage());

                //Damage all entities colliding with walls
                for(Wall* wall : outerWalls)
                {
                    if(currentEnt->CheckCollisionWall(wall, currentEnt->GetPos(), currentEnt->GetRadius() + 5))
                    {
                        currentEnt->IncrementHealth(-1.0f * currentEnt->GetDamage());
                    }
                }

                //Damage all prey colliding with predator
                if(!currentEnt->GetIsPrey())
                {
                    for(int i = 0; i < preyList.size(); ++i)
                    {
                        if(CheckCollisionCircles(currentEnt->GetPos(), currentEnt->GetRadius(), preyList[i]->GetPos(), preyList[i]->GetRadius()))
                        {
                            preyList[i]->IncrementHealth(-1.0f * currentEnt->GetDamage());
                            currentEnt->IncrementHealth(currentEnt->GetDamage());
                            if(currentEnt->GetHealth() > pow(currentEnt->GetRadius(), 2))
                            {
                                currentEnt->SetHealth(pow(currentEnt->GetRadius(), 2));
                            }
                            if(preyList[i]->GetHealth() <= 0)
                            {
                                currentEnt->SetDinnerThisTick();
                                currentEnt->ResetTicksSinceDinner();
                                currentEnt->SetKillCount(currentEnt->GetKillCount() + 1);
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
                            currentEnt->IncrementHealth(-1.0f * pow(foodList[i]->GetRadius(), 2) / 50.0f);
                        }
                    }
                }

                //Damage all food colliding with prey
                if(currentEnt->GetIsPrey())
                {
                    for(int i = 0; i < foodList.size(); ++i)
                    {
                        if(CheckCollisionCircles(currentEnt->GetPos(), currentEnt->GetRadius(), foodList[i]->GetPos(), foodList[i]->GetRadius()))
                        {
                            foodList[i]->UpdateValueRemaining(currentEnt->GetDamage());
                            currentEnt->IncrementHealth(pow(foodList[i]->GetRadius(), 2) / 50.0f);
                            if(currentEnt->GetHealth() > pow(currentEnt->GetRadius(), 2))
                            {
                                currentEnt->SetHealth(pow(currentEnt->GetRadius(), 2));
                            }
                            if(foodList[i]->GetValueRemaining() <= 0)
                            {
                                currentEnt->SetDinnerThisTick();
                                currentEnt->ResetTicksSinceDinner();
                                currentEnt->SetKillCount(currentEnt->GetKillCount() + 1);
                            }
                        }
                    }
                }

                //Degrade food
                for(Food* food : foodList)
                {
                    float r = food->GetRadius();
                    food->UpdateValueRemaining((r * r) / 500000.0f);
                }
                
                //Remove the eaten food from the simulation
                for(int i = 0; i < foodList.size(); ++i)
                {
                    for(int j = 0; j < foodList.size(); ++j)
                    {
                        if(foodList[j]->GetValueRemaining() <= 0)
                        {
                            delete foodList[j];
                            foodList[j] = nullptr;
                            foodList.erase(std::remove(foodList.begin(), foodList.end(), nullptr), foodList.end());
                        }
                    }
                }

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
                if(currentEnt->GetIsTruman())
                {
                    truman = nullptr;
                }
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
            if(preyList.size() < preyPop)
            {
                for(int _ = 0; _ < preyPop - preyList.size(); ++_)
                {
                    Entity* e = new Entity(true, size.x - 50, size.y - 50);
                    for(Wall* wall : outerWalls)
                    {
                        while(e->CheckCollisionWall(wall, e->GetPos(), e->GetRadius()))
                        {
                            e->Teleport({(float)xDist(mt) + e->GetPos().x, (float)yDist(mt) + e->GetPos().y});
                        }
                    }
                    preyList.push_back(e);
                }
            }
            else
            {
                for(int i = 0; i < preyList.size(); ++i)
                {
                    preyList[i]->IncrementTicksSurvived();
                    if((preyList[i]->GetHealth() > pow(preyList[i]->GetRadius(), 2) * 0.9f) && (preyList[i]->GetTicksSinceReproduction() > 50))
                    {   
                        if(preyList.size() < (2 * preyPop))
                        {   
                            preyList.push_back(preyList[i]->Reproduce({preyList[i]->GetPos().x + xDist(mt), preyList[i]->GetPos().y + yDist(mt)}, 40));
                        }
                    }
                    else
                    {
                        preyList[i]->IncrementTicksSinceReproduction();
                    }
                }
            }
            if(predList.size() < predPop)
            {   
                for(int _ = 0; _ < predPop - predList.size(); ++_)
                {
                    Entity* e = new Entity(false, size.x - 50, size.y - 50);
                        for(Wall* wall : outerWalls)
                        {
                            while(e->CheckCollisionWall(wall, e->GetPos(), e->GetRadius()))
                            {
                                e->Teleport({(float)xDist(mt) + e->GetPos().x, (float)yDist(mt) + e->GetPos().y});
                            }
                        }
                        predList.push_back(e);
                }
            }
            else
            {
                for(int i = 0; i < predList.size(); ++i)
                {   
                    predList[i]->IncrementTicksSurvived();
                    if((predList[i]->GetHealth() > pow(predList[i]->GetRadius(), 2) * 0.9f) && (predList[i]->GetTicksSinceReproduction() > 50))
                    {
                        if(predList.size() < (2 * predPop))
                        {
                            predList.push_back(predList[i]->Reproduce({predList[i]->GetPos().x + xDist(mt), predList[i]->GetPos().y + xDist(mt)}, 40));
                        }
                    }
                    else
                    {
                        predList[i]->IncrementTicksSinceReproduction();
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

        Entity* GetTruman()
        {
            return truman;
        }

        std::vector<Wall*> GetWalls()
        {
            return outerWalls;
        }

        std::vector<Food*> GetFoodList()
        {
            return foodList;
        }

        void SetTruman(Vector2 mouseCoords)
        {
            for(Entity* prey : preyList)
            {
                prey->SetTrumanFalse();
            }
            for(Entity* pred : predList)
            {
                pred->SetTrumanFalse();
            }
            for(Entity* prey : preyList)
            {
                if(CheckCollisionPointCircle(mouseCoords, prey->GetPos(), prey->GetRadius()))
                {
                    prey->SetIsTruman();
                    truman = prey;
                    break;
                }
            }
            for(Entity* pred : predList)
            {
                if(CheckCollisionPointCircle(mouseCoords, pred->GetPos(), pred->GetRadius()))
                {
                    pred->SetIsTruman();
                    truman = pred;
                    break;
                }
            }
        }

        void DrawSim()
        {
            //Draw the walls
            for(Wall* wall : outerWalls)
            {
                wall->DrawWall();
            }
            //Draw the food
            for(Food* f : foodList)
            {
                f->DrawFood();
            }
            //Draw the little guys
            for(Entity* e : predList)
            {   
                e->drawEntity();
            }
            for(Entity* e : preyList)
            {   
                e->drawEntity();
            }
            //Draw truman's sight data
            if(truman)
            {
                for(Vector2 p : truman->GetSightPoints())
                {
                    DrawCircle(p.x, p.y, 2.0f, GRAY);
                }
            }
        }
        

};

#endif