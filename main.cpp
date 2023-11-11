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

Simulation sim = Simulation(50, 50, 1000, 1000);
std::vector<Wall*> walls = sim.GetWalls();
std::vector<Food*> foods = sim.GetFoodList();

int main()
{     

    std::string pred_count;
    std::string prey_count;
    if(drawSim)
    {
        InitWindow(1750, 1000, "Evolution Simulatior");
        SetTargetFPS(240);

        while(!WindowShouldClose())
        {   
            sim.DoGameTick();

            BeginDrawing();
            ClearBackground(RAYWHITE);

            //Draw the walls
            for(Wall* wall : walls)
            {
                wall->DrawWall();
            }

            //Draw the food
            for(Food* food : foods)
            {
                food->DrawFood();
            }

            //Draw the little guys
            for(int i = 0; i < sim.GetPredList().size(); ++i)
            {   
                sim.GetPredList()[i]->drawEntity();
            }
            for(int i = 0; i < sim.GetPreyList().size(); ++i)
            {   
                sim.GetPreyList()[i]->drawEntity();
            }

            pred_count = std::to_string(sim.GetPredList().size());
            prey_count = std::to_string(sim.GetPreyList().size());
            DrawText(pred_count.c_str(), 1600, 100, 40, RED);
            DrawText(prey_count.c_str(), 1600, 150, 40, GREEN);
            

            //Draw the weights of the first hidden layer
            /*
            std::vector<Weight*>w = sim.GetTrumanWeights();
            for(int j = 0; j < 12; ++j)
            {
                Color c;
                for(int i = 0; i < 12; ++i)
                {
                    if(w[i * j]->getWeight() < 0)
                    {
                        c = RED;
                        c.a = w[i * j]->getWeight() * (-125.5);
                        DrawLine(1300, 400 + (i * 35), 1500, 400 + (j * 35), c);
                    }
                    else
                    {
                        c = GREEN;
                        c.a = w[i * j]->getWeight() * 125.5;
                        DrawLine(1300, 400 + (i * 35), 1500, 400 + (j * 35), c);
                    }
                }
            }
            for(int j = 0; j < 3; ++j)
            {
                Color c;
                for(int i = 0; i < 12; ++i)
                {
                    if(w[i * j]->getWeight() < 0)
                    {
                        c = RED;
                        c.a = w[i * j]->getWeight() * (-125.5);
                        DrawLine(5300, 400 + (i * 35), 1700, 300 + (j * 50), c);
                    }
                    else
                    {
                        c = GREEN;
                        c.a = w[i * j]->getWeight() * 125.5;
                        DrawLine(1500, 400 + (i * 35), 1700, 300 + (j * 50), c);
                    }
                }
            }

            for(int j = 0; j < 12; ++j)
            {
                Color c;
                for(int i = 0; i < 24; ++i)
                {
                    if(w[i * j]->getWeight() < 0)
                    {
                        c = RED;
                        c.a = w[i * j]->getWeight() * (-125.5);
                        DrawLine(1100, 170 + (i * 35), 1300, 400 + (j * 35), c);
                    }
                    else
                    {
                        c = GREEN;
                        c.a = w[i * j]->getWeight() * 125.5;
                        DrawLine(1100, 170 + (i * 35), 1300, 400 + (j * 35), c);
                    }
            
                }
            }*/
/*
            std::vector<Neuron*>t = sim.GetTrumanNeurons();
            for(int i = 0; i < 12; ++i)
            {
                if(t[i + 27]->getValue() < 0)
                {
                    Color c = GREEN;
                    c.a = t[i + 27]->getValue() * (-255);
                    DrawCircle(1100, 400 + (i * 30), 15, c);
                }
                else
                {
                    Color c = RED;
                    c.a = t[i + 27]->getValue() * 255;
                    DrawCircle(1100, 400 + (i * 30), 15, c);
                }
            }
            for(int i = 0; i < 12; ++i)
            {
                if(t[i + 39]->getValue() < 0)
                {
                    Color c = BROWN;
                    c.a = t[i + 39]->getValue() * (-255);
                    DrawCircle(1100, 760 + (i * 30), 15, c);
                }
                else
                {
                    Color c = BLACK;
                    c.a = t[i + 39]->getValue() * 255;
                    DrawCircle(1100, 760 + (i * 30), 15, c);
                }
            }

            //Hidden layers
            for(int i = 0; i < 12; ++i)
            {
                Color c = GRAY;
                c.a = t[i]->getValue() * 255;
                DrawCircle(1300, 400 + (i * 30), 15, c);
            }
            for(int i = 0; i < 12; ++i)
            {
                Color c = GRAY;
                c.a = t[i + 12]->getValue() * 255;
                DrawCircle(1500, 400 + (i * 30), 15, c);
            }
            for(int i = 0; i < 3; ++i)
            {
                Color c = GRAY;
                c.a = t[i + 24]->getValue() * 255;
                DrawCircle(1700, 450 + (i * 50), 15, c);
            */

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