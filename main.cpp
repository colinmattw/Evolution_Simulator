#include <iostream>
#include "NeuralNetwork.hpp"
#include <vector>
#include "raylib.h"
#include "Entity.hpp"
#include "Simulation.hpp"
#include "Food.hpp"
#include <string>
using namespace std;

clock_t tickTime;
bool drawSim = true;
bool simIsPaused = false;

Simulation sim = Simulation(75, 75, 1000, 1000);
std::vector<Wall*> walls = sim.GetWalls();

int main()
{     

    std::string pred_count;
    std::string prey_count;
    std::string food_count;
    if(drawSim)
    {
        InitWindow(1750, 1000, "Evolution Simulatior");
        SetTargetFPS(240);

        while(!WindowShouldClose())
        {   
            if(IsKeyPressed(KEY_P))
            {
                simIsPaused = !simIsPaused;
            }
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 coords = GetMousePosition();
                sim.SetTruman(coords);
            }
            if(!simIsPaused)
            {
                sim.DoGameTick();
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            sim.DrawSim();

            pred_count = std::to_string(sim.GetPredList().size());
            prey_count = std::to_string(sim.GetPreyList().size());
            food_count = std::to_string(sim.GetFoodList().size());
            DrawText(pred_count.c_str(), 1600, 100, 40, RED);
            DrawText(prey_count.c_str(), 1600, 150, 40, GREEN);
            DrawText(food_count.c_str(), 1600, 200, 40, GOLD);
            

            //Draw the weights of the first hidden layer
            if(sim.GetTruman())
            {
                std::vector<Weight*>w = sim.GetTruman()->GetNeuralNetwork()->GetWeights();
                for(int j = 0; j < 3; ++j)
                {
                    Color c;
                    for(int i = 0; i < 12; ++i)
                    {
                        if(w[i * j]->getWeight() < -1)
                        {
                            c = RED;
                            c.a = w[i * j]->getWeight() * (-125.5);
                            DrawLine(1300, 640 + (i * 20), 1500, 760 + (j * 20), c);
                        }
                        else if(w[i * j]->getWeight() > 1)
                        {
                            c = GREEN;
                            c.a = w[i * j]->getWeight() * 125.5;
                            DrawLine(1300, 640 + (i * 20), 1500, 760 + (j * 20), c);
                        }
                    }
                }
                for(int j = 0; j < 12; ++j)
                {
                    Color c;
                    for(int i = 0; i < 24; ++i)
                    {
                        if(w[i * j]->getWeight() < -1)
                        {
                            c = RED;
                            c.a = w[i * j]->getWeight() * (-125.5);
                            DrawLine(1100, 520 + (i * 20), 1300, 640 + (j * 20), c);
                        }
                        else if(w[i * j]->getWeight() > 1)
                        {
                            c = GREEN;
                            c.a = w[i * j]->getWeight() * 125.5;
                            DrawLine(1100, 520 + (i * 20), 1300, 640 + (j * 20), c);
                        }
                
                    }
                }

                std::vector<Neuron*>t = sim.GetTruman()->GetNeuralNetwork()->GetNeurons();
                for(int i = 0; i < 12; ++i)
                {
                    if(t[i + 15]->getValue() < 0)
                    {
                        Color c = GREEN;
                        c.a = t[i + 15]->getValue() * (-255);
                        DrawCircle(1100, 520 + (i * 20), 10, c);
                    }
                    else
                    {
                        Color c = RED;
                        c.a = t[i + 15]->getValue() * 255; 
                        DrawCircle(1100, 520 + (i * 20), 10, c);
                    }
                }
                for(int i = 0; i < 12; ++i)
                {
                    if(t[i + 27]->getValue() < 0)
                    {
                        Color c = BLACK;
                        c.a = t[i + 27]->getValue() * (-255);
                        DrawCircle(1100, 760 + (i * 20), 10, c);
                    }
                    else
                    {
                        Color c = GOLD;
                        c.a = t[i + 27]->getValue() * 255;
                        DrawCircle(1100, 760 + (i * 20), 10, c);
                    }
                }
                //Hidden layers
                for(int i = 0; i < 12; ++i)
                {
                    Color c = GRAY;
                    c.a = t[i]->getValue() * 255;
                    DrawCircle(1300, 640 + (i * 20), 10, c);
                }
                for(int i = 0; i < 3; ++i)
                {
                    Color c = GRAY;
                    c.a = t[i + 12]->getValue() * 255;
                    DrawCircle(1500, 760 + (i * 20), 10, c);
                }
            }
            
            DrawFPS(1600,50);
            EndDrawing();
        }
        CloseWindow();
    }
    else
    {
        while(true)
        {
            tickTime = clock();
            sim.DoGameTick();
            tickTime = clock() - tickTime;
            //cout << tickTime << endl;
        }
    }
}