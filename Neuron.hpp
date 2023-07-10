#ifndef NEURON_HPP
#define NEURON_HPP
#include <cmath>
#include <iostream>
#include <vector>
#include "Weight.hpp"
#include <random>


class Neuron
{
    private:
        std::vector<Weight*> inputWeights;
        std::vector<Weight*> outputWeights;
        float bias;
        float value;
        
    public:

        Neuron()
        {
            value = 0.0f;

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> dist (-1000, 1000);
            bias = dist(mt) / 250.0f;

        }
        Neuron(float inval)
        {
            value = inval;
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> dist (-1000, 1000);
            bias = dist(mt) / 250.0f;
        }

        void setConnection(Neuron* target, Weight* weight, bool isOutboundConnection)
        {
            if(isOutboundConnection)
            {
                outputWeights.push_back(weight);
                target->inputWeights.push_back(weight);
                weight->setIn(this);
                weight->setOut(target);
            }
            else
            {
                inputWeights.push_back(weight);
                target->outputWeights.push_back(weight);
                weight->setIn(target);
                weight->setOut(this);
            }
        }

        float getValue()
        {
            return value;
        }
        void setValue(float inVal)
        {
            value = inVal;
        }

        void SetBias(float inBias)
        {
            bias = inBias;
        }

        float GetBias()
        {
            return bias;
        }

        void computeValue()
        {   
            float sum = 0.0f;
            for (int i = 0; i < inputWeights.size(); ++i)
            {
               sum += (inputWeights[i]->getWeight()) * (inputWeights[i]->getConnectionIn()->getValue());
            }
            sum += bias;
            //std::cout << "before squishification: ";
            //std::cout << sum << std::endl;
            value = 1 / (1 + exp(-sum));
        }
};
#endif