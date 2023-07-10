#include <iostream>
#include "NeuralNetwork.hpp"
#include <vector>
#include "raylib.h"
#include "Entity.hpp"
#include "Simulation.hpp"
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
    std::vector<Entity*> v = {new Entity(true)};
    for(int i = 0; i < 5; ++i)
    {
        v.push_back(new Entity(true));
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
    Entity* e1 = new Entity(true);
    e1->CreateNetToPassOn(0);
    Entity* e2 = e1->Reproduce(e1->GetPos());
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
        ents.push_back(new Entity(true));
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
    Entity e1 = new Entity(true);
    Entity e2 = *e1.Reproduce({0,0});
    /*for(int i = 0; i < e1.GetNeuralNetwork()->GetWeights().size(); ++i)
    {
        cout << e1.GetNeuralNetwork()->GetWeights()[i]->getWeight() << " " << e2.GetNeuralNetwork()->GetWeights()[i]->getWeight() << endl;
    }*/
    return e1.GetNeuralNetwork()->NetsSimilarityPercentage(e2.GetNeuralNetwork());
}

void WrapSightTest()
{
    Entity e1 = new Entity({550, 200}, false);
    e1.SetIsTruman();
    Entity e2 = new Entity({30, 220}, true);
    std::vector<Entity*> ents;
    ents.push_back(&e1);
    ents.push_back(&e2);
    e1.drawEntity();
    e2.drawEntity();
    e1.CollectRayData(ents, {600, 600});



}

int main()
{
    Entity* e1 = new Entity({200, 200}, false);
    e1->SetIsTruman();
    Entity* e2 = new Entity({150, 200}, true);
    Entity* e3 = new Entity({250, 200}, true);
    std::vector<Entity*> ents;
    ents.push_back(e1);
    ents.push_back(e2);
    ents.push_back(e3);

    InitWindow(800, 800, "Evolution Simulatior");
    SetTargetFPS(30);

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        e1->drawEntity();
        e2->drawEntity();
        e3->drawEntity();
        e1->CollectRayData(ents, {600, 600});
        e1->UpdateOrientation();

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