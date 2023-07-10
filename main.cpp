#include <iostream>
#include "NeuralNetwork.hpp"
#include <vector>
#include "raylib.h"
#include "Entity.hpp"
#include "Simulation.hpp"
#include <string>
using namespace std;

clock_t tickTime;
bool drawSim = true;

Simulation sim = Simulation(75, 75);

int main()
{     

    std::string pred_count;
    std::string prey_count;
    if(drawSim)
    {
        InitWindow(1000, 1000, "Evolution Simulatior");
        SetTargetFPS(240);

        while(!WindowShouldClose())
        {   
            /*cout << "best predator: " << sim.GetMostSuccessfulPred()->GetTicksSurvived() << ' '
            << "best prey: " << sim.GetMostSuccessfulPrey()->GetTicksSurvived()<< endl;*/
            sim.DoGameTick();

            BeginDrawing();
            ClearBackground(RAYWHITE);

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
            DrawText(pred_count.c_str(), 600, 100, 40, RED);
            DrawText(prey_count.c_str(), 600, 150, 40, GREEN);

            std::vector<Weight*>w = sim.GetTrumanWeights();
            for(int j = 0; j < 4; ++j)
            {
                Color c;
                for(int i = 0; i < 8; ++i)
                {
                    if(w[i * j]->getWeight() < 0)
                    {
                        c = RED;
                        c.a = w[i * j]->getWeight() * (-25.5);
                        DrawLine(300, 500 + (i * 35), 500, 550 + (j * 50), c);
                    }
                    else
                    {
                        c = GREEN;
                        c.a = w[i * j]->getWeight() * 25.5;
                        DrawLine(300, 500 + (i * 35), 500, 550 + (j * 50), c);
                    }
                }
            }

            for(int j = 0; j < 8; ++j)
            {
                Color c;
                for(int i = 0; i < 13; ++i)
                {
                    if(w[i * j]->getWeight() < 0)
                    {
                        c = RED;
                        c.a = w[i * j]->getWeight() * (-25.5);
                        DrawLine(100, 420 + (i * 30), 300, 500 + (j * 35), c);
                    }
                    else
                    {
                        c = GREEN;
                        c.a = w[i * j]->getWeight() * 25.5;
                        DrawLine(100, 420 + (i * 30), 300, 500 + (j * 35), c);
                    }
            
                }
            }




            std::vector<Neuron*>t = sim.GetTrumanNeurons();
            for(int i = 0; i < 12; ++i)
            {
                if(t[i + 12]->getValue() < 0)
                {
                    Color c = GREEN;
                    c.a = t[i + 12]->getValue() * (-255);
                    DrawCircle(100, 420 + (i * 30), 15, c);
                }
                else
                {
                    Color c = RED;
                    c.a = t[i + 12]->getValue() * 255;
                    DrawCircle(100, 420 + (i * 30), 15, c);
                }
            }
            Color c = BLUE;
            c.a = t[24]->getValue() * 255;
            DrawCircle(100, 780, 15, c);

            for(int i = 0; i < 8; ++i)
            {
                Color c = GRAY;
                c.a = t[i]->getValue() * 255;
                DrawCircle(300, 500 + (i * 35), 15, c);
            }
            for(int i = 0; i < 4; ++i)
            {
                Color c = GRAY;
                c.a = t[i + 8]->getValue() * 255;
                DrawCircle(500, 550 + (i * 50), 15, c);
            }

            DrawFPS(600,50);
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