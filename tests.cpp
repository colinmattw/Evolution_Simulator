#include <iostream>
#include "NeuralNetwork.hpp"
#include <vector>
#include "raylib.h"
#include "Entity.hpp"
#include "Simulation.hpp"
#include "Wall.hpp"
using namespace std;

//Test Neuron construction and deletion
void BaseNeuronTest()
{   
    Neuron* n = new Neuron();
    delete n;
    n = nullptr;
    //cout << n->getValue() << endl;
}

void BaseWeightTest()
{
    Weight* w = new Weight();
    delete w;
    w = nullptr;
    //cout << w->getWeight() << endl;
}

void BaseNNTest()
{
    NeuralNetwork* net = new NeuralNetwork(10, 10, 10, 10);
    net->DeleteContents();
    delete net;
    net = nullptr;
}

//vector of entities test
void VecOfEntTest()
{
    std::vector<Entity*> v = {new Entity(true, 500, 500)};
    for(int i = 0; i < 5; ++i)
    {
        v.push_back(new Entity(true, 500, 500));
    }
    int i = 0;
    while(v.size() != 0)
    {
        v.back()->DeleteNets();
        delete v.back();
        v.back() = nullptr;
        v.erase(std::remove(v.begin(), v.end(), v.back()), v.end());
        ++i;
    }
}

//Neuralnet mutation test
void NetMutationTest()
{
    Entity* e1 = new Entity(true, 500, 500);
    e1->CreateNetToPassOn(0);
    Entity* e2 = e1->Reproduce(e1->GetPos(), 4);
    cout << "Neurons" << endl;
    for(int i = 0; i <  e1->GetNeuralNetwork()->GetNeurons().size(); ++i)
    {
        cout << e1->GetNeuralNetwork()->GetNeurons()[i]->GetBias() << ":" 
        << e2->GetNeuralNetwork()->GetNeurons()[i]->GetBias() << endl;
    }
    cout << "Weights" << endl;
    for(int i = 0; i <  e1->GetNeuralNetwork()->GetWeights().size(); ++i)
    {
        cout << e1->GetNeuralNetwork()->GetWeights()[i]->getWeight() << ":" 
        << e2->GetNeuralNetwork()->GetWeights()[i]->getWeight() << endl;
    }
}

void NNSizeTest()
{
    NeuralNetwork* smallNet = new NeuralNetwork(8, 4, 1, 6);
    NeuralNetwork* bigNet = new NeuralNetwork(8, 4, 1, 6);

    clock_t smallNetClock = clock();
    smallNet->ComputeOutputLayer(std::vector<float> {0.25, 0, 1, 0.12, -0.1, 0.25, 0, 1});
    smallNetClock = clock() - smallNetClock;

    //{0.25, 0, 1, 0.12, -0.1, 0.25, 0, 1, 0.25, 0, 1, 0.12, -0.1}
    clock_t bigNetClock = clock();
    bigNet->ComputeOutputLayer(std::vector<float> {0.25, 0, 1, 0.12, -0.1, 0.25, 0, 1});
    bigNetClock = clock() - bigNetClock;

    cout << "Small net time: " << smallNetClock << endl;
    cout << "Big net time: " << bigNetClock << endl;
}

void RaySpeedTest()
{
    std::vector<Entity*> ents;
    for(int i = 0; i < 40; ++i)
    {
        ents.push_back(new Entity(true, 500, 500));
    }
    clock_t time = clock();
    for(int i = 0; i < 40; ++i)
    {
        ents[i]->CollectRayData(ents, {500, 500});
    }
    time = clock() - time;
    cout << time << endl;
}

float MutationTest()
{
    Entity e1 = new Entity(true, 500, 500);
    Entity e2 = *e1.Reproduce({0,0}, 4);
    /*for(int i = 0; i < e1.GetNeuralNetwork()->GetWeights().size(); ++i)
    {
        cout << e1.GetNeuralNetwork()->GetWeights()[i]->getWeight() << " " << e2.GetNeuralNetwork()->GetWeights()[i]->getWeight() << endl;
    }*/
    return e1.GetNeuralNetwork()->NetsSimilarityPercentage(e2.GetNeuralNetwork());
}

void ReproduceTest()
{
    Entity* e1 = new Entity({550, 200}, false);
    Entity* e2 = e1->Reproduce(e1->GetPos(), 100);
}

void DrawWallTest(Wall* w)
{
    w->DrawWall();
}

int main()
{   
    Wall* walls[4];
    walls[0] = new Wall(false, 200, 800, 200, 800, 25);
    for(int i = 1; i < 4; ++i)
    {
        walls[i] = new Wall(false, 200, 800, 200, 800, 25, walls[i - 1]->GetC3());
    }
    Entity* e1 = new Entity({500, 500}, false);
    Entity* e2 = new Entity({550, 450}, true);
    std::vector<Entity*> ents;
    e1->SetIsTruman();
    e1->SetFOV(140);
    e1->SetViewDist(60);
    e1->SetAngularSpeed(0.0025f);
    ents.push_back(e1);
    ents.push_back(e2);
    InitWindow(1000, 1000, "Evolution Simulatior");
    SetTargetFPS(120);
    while(!WindowShouldClose())
    {   
        //e1->UpdateOrientation();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        e1->drawEntity();
        e2->drawEntity();
        e1->CollectRayData(ents, {1000, 1000});
        for(Wall* wall : walls)
        {
            DrawWallTest(wall);
        }
        

        DrawFPS(600,50);
        EndDrawing();
    }
    CloseWindow();
    /*BaseNeuronTest();
    BaseWeightTest();
    BaseNNTest();
    VecOfEntTest();
    NetMutationTest();
    NNSizeTest();
    RaySpeedTest();*/
    //cout << MutationTest() << endl;

}