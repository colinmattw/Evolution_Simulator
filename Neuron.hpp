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
        bool isHidden;

        
    public:

        Neuron(bool hidden)
        {
            value = 0.0f;

            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> dist (-5000, 5000);
            isHidden = hidden;
            if(isHidden)
            {
                bias = dist(mt) / 500.0f;
            }
            else
            {
                bias = 0.0f;
            }



        }
        Neuron(float inval, bool hidden)
        {
            value = inval;
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_int_distribution<int> dist (-1000, 1000);
            isHidden = hidden;
            if(isHidden)
            {
                bias = dist(mt) / 500.0f;
            }
            else
            {
                bias = 0.0f;
            }
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

        float Sigmoid(float n)
        {
            return 1 / (1 + exp(-n));
        }

        float ReLU(float n)
        {
            return fmaxf(0, n);
        }
        void computeValue()
        {   
            float sum = 0.0f;
            for (int i = 0; i < inputWeights.size(); ++i)
            {
               sum += (inputWeights[i]->getWeight()) * (inputWeights[i]->getConnectionIn()->getValue());
            }

            if(!isHidden)
            {
                value = Sigmoid(sum);
            }
            else
            {
                sum += bias;
                value = ReLU(sum);
            }
        }

        bool GetIsHidden()
        {
            return isHidden;
        }
};
#endif