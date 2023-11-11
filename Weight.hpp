#ifndef WEIGHT_HPP
#define WEIGHT_HPP
#include <random>

class Neuron;

class Weight
{
    private:
        float weight;
        Neuron *connectionIn;
        Neuron *connectionOut;

    public:
        Weight()
        {
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_real_distribution<float> dist (-2.0, 2.0f);

            weight = dist(mt);
            connectionIn = nullptr;
            connectionOut =nullptr;
        }

        Weight(Neuron *in, Neuron *out)
        {
            std::random_device dev;
            std::mt19937 mt(dev());
            std::uniform_real_distribution<float> dist (-2.0, 2.0f);

            weight = dist(mt);
            connectionIn = in;
            connectionOut = out;
        }

        Weight(Neuron *in, Neuron *out, float inWeight)
        {
            weight = inWeight;
            connectionIn = in;
            connectionOut = out;
        }

        Weight(float inWeight)
        {
            weight = inWeight;
        }

        void setIn(Neuron *in)
        {
            connectionIn = in;
        }
        void setOut(Neuron *out)
        {
            connectionOut = out;
        }
        Neuron* getConnectionIn()
        {
            return connectionIn;
        }
        Neuron* getConnectionOut()
        {
            return connectionOut;
        }
        void setWeight(float inWeight)
        {
            weight = inWeight;
        }
        float getWeight()
        {
            return weight;
        }
};
#endif