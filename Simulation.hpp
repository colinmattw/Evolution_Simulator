#ifndef GAME_HPP
#define GAME_HPP
#include "Entity.hpp"
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
        Entity* lastPrey;
        Entity* lastPred;
        std::vector<Neuron*> trumanNeurons;
        std::vector<Weight*> trumanWeights;
        



    public:

        Simulation(int prey, int pred)
        {
            ticks = 0;
            predPop = pred;
            preyPop = prey;
            size = {1000, 1000};

            for(int i = 0; i < predPop; ++i)
            {   

                Entity* e = new Entity(false);
                predList.push_back(e);
            }
            for(int i = 0; i < preyPop; ++i)
            {   

                Entity* e = new Entity(true);
                preyList.push_back(e);
            }
            lastPrey = new Entity({0, 0}, true);
            lastPred = new Entity({0, 0}, false);
        }

        void DoGameTick()
        {   
            //use this for random spawing
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> xDist(0, size.x);
            std::uniform_int_distribution<int> yDist(0, size.y);

            //Update the last predetor and prey every 100 ticks cause its expensive
            if(!(ticks % 100))
            {   
                lastPrey->DeleteNets();
                lastPrey->SetNeuralNetwork(preyList.back()->GetNeuralNetwork()->CopyNet());
                lastPrey->SetTicksSurvived(preyList.back()->GetTicksSurvived());

                lastPred->DeleteNets();
                lastPred->SetNeuralNetwork(predList.back()->GetNeuralNetwork()->CopyNet());
                lastPred->SetKillCount(predList.back()->GetKillCount());
            }

            std::vector<Entity*> entitiesToKill;
            std::vector<Entity*> entList;

            entList.reserve(predList.size() + preyList.size());
            entList.insert(entList.end(), predList.begin(), predList.end());
            entList.insert(entList.end(), preyList.begin(), preyList.end());
            for(int currentEntIndex = 0; currentEntIndex < entList.size(); ++currentEntIndex)
            {   
                //collect input for each entity
                Entity* currentEnt = entList[currentEntIndex];
                currentEnt->CollectRayData(entList, size);
               
                //Process the neural networks inputs
                currentEnt->ProcessInput();
            }
            for(int currentEntIndex = 0; currentEntIndex < entList.size(); ++currentEntIndex)
            {   
                //Execute move for each entity
                Entity* currentEnt = entList[currentEntIndex];
                currentEnt->UpdateOrientation();
                currentEnt->UpdatePosMove();
                Vector2 currentEntPos = currentEnt->GetPos();

                //If entity moves out of bounds, teleport him back
                if(currentEntPos.x > size.x)
                {
                    currentEnt->Teleport({0, currentEntPos.y});
                }
                else if(currentEntPos.x < 0)
                {
                    currentEnt->Teleport({size.x, currentEntPos.y});
                }
                if(currentEntPos.y > size.y)
                {
                    currentEnt->Teleport({currentEntPos.x, 0});
                }
                else if(currentEntPos.y < 0)
                {
                    currentEnt->Teleport({currentEntPos.x, size.y});
                }

                //Damage Predators who haven't eaten in a while
                if(currentEnt->GetTicksSinceDinner() > 500)
                {
                    currentEnt->UpdateHealth(1);
                }

                //Attempt kill
                if(currentEnt->GetHealth() <= 0)
                {   
                    entitiesToKill.push_back(currentEnt);
                }
                else
                {
                    //Attempt attack
                    if(currentEnt->WillAttack() && !currentEnt->GetIsPrey())
                    {
                        currentEnt->Attack();
                    }
                }
                if(!currentEnt->GetIsPrey())
                {
                    if(!currentEnt->DidHaveDinnerThisTick())
                    {
                        currentEnt->IncrementTicksSinceDinner();
                    }
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
                //std::cout << "Prey: " << preyList[0]->GetNeuralNetwork()->NetsSimilarityPercentage(lastPrey->GetNeuralNetwork()) << std::endl;
            }
            else
            {
                for(int i = 0; i < preyList.size(); ++i)
                {
                    preyList[i]->IncrementTicksSurvived();
                    if((preyList[i]->GetTicksSurvived() % 300 == 0))
                    {   
                        if(preyList.size() < (2 * preyPop))
                        {
                            preyList.push_back(preyList[i]->Reproduce(preyList[i]->GetPos()));
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
                    if(predList[i]->DidHaveDinnerThisTick())
                    {
                        if(predList.size() < (2 * predPop))
                        {
                            predList.push_back(predList[i]->Reproduce(predList[i]->GetPos()));
                        }
                    }
                }
            }
            predList.front()->SetIsTruman();
            trumanNeurons = predList.front()->GetNeuralNetwork()->GetNeurons();
            trumanWeights = predList.front()->GetNeuralNetwork()->GetWeights();
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

        

};

#endif