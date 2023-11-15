#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP
#include "Neuron.hpp"
#include <vector>

class NeuralNetwork
{
    private:
        int numOfinputDimensions;
        int numOfoutputDimensions;
        int numOfhiddenLayers;
        int neuronsPerHiddenLayer;
        std::vector<Neuron*> inputLayer;
        std::vector<Neuron*> outputLayer;
        std::vector<std::vector<Neuron*> > hiddenLayers;
        std::vector<Weight*> weights;
        std::vector<Neuron*> neurons;

        void buildNeuralNetwork()
        {
            //build hidden layers first
            for(int _ = 0; _ < numOfhiddenLayers; ++_)
            {   
                std::vector<Neuron*> particularLayer;
                for(int _ = 0; _ < neuronsPerHiddenLayer; ++_)
                {
                    Neuron* h = new Neuron(true);
                    particularLayer.push_back(h);
                    neurons.push_back(h);
                }
                hiddenLayers.push_back(particularLayer);
            }

            //connect layers if there is more than 1
            if(numOfhiddenLayers > 1)
            {
                for(int currentLayer = 0; currentLayer < numOfhiddenLayers - 1; ++currentLayer)
                {
                    for(int currentNeuron = 0; currentNeuron < neuronsPerHiddenLayer; ++currentNeuron)
                    {
                        for(int targetNeuron = 0; targetNeuron < neuronsPerHiddenLayer; ++targetNeuron)
                        {
                            Weight* wh = new Weight();
                            hiddenLayers[currentLayer][currentNeuron]->setConnection(hiddenLayers[currentLayer+1][targetNeuron], wh, 1);
                            weights.push_back(wh);
                        }
                    }
                }
            }
            
            //build output layer next
            for(int _ = 0; _ < numOfoutputDimensions; ++_)
            {
                Neuron* y = new Neuron(false);
                outputLayer.push_back(y);
                neurons.push_back(y);
            }

            //connect output layer to the last hidden layer
            for(int currentY = 0; currentY < numOfoutputDimensions; ++currentY)
            {
                for(int targetH = 0; targetH < neuronsPerHiddenLayer; ++targetH)
                {
                    Weight* wy = new Weight();
                    outputLayer[currentY]->setConnection(hiddenLayers[numOfhiddenLayers-1][targetH], wy, false);
                    weights.push_back(wy);
                }
            }

            //build the input layer
            for(int _ = 0; _ < numOfinputDimensions; ++_)
            {
                Neuron* x = new Neuron(false);
                inputLayer.push_back(x);
                neurons.push_back(x);
            }

            //connect input layer to the first hidden layer
            for(int currentX = 0; currentX < numOfinputDimensions; ++currentX)
            {
                for(int targetH = 0; targetH < neuronsPerHiddenLayer; ++targetH)
                {
                    Weight* wx = new Weight();
                    inputLayer[currentX]->setConnection(hiddenLayers[0][targetH], wx, 1);
                    weights.push_back(wx);
                }
            }
        }

    public:

        NeuralNetwork()
        {
            numOfinputDimensions = 1;
            numOfoutputDimensions = 1;
            numOfhiddenLayers = 1;
            neuronsPerHiddenLayer = 1;

            buildNeuralNetwork();
        }

        NeuralNetwork(int inD, int outD, int numHLs, int hidD)
        {
            numOfinputDimensions = inD;
            numOfoutputDimensions = outD;
            numOfhiddenLayers = numHLs;
            neuronsPerHiddenLayer = hidD;

            buildNeuralNetwork();
        }

        NeuralNetwork(int inD, int outD, int numHLs, int hidD, std::vector<Weight*> inWeights, std::vector<Neuron*> inNuerons)
        {
            numOfinputDimensions = inD;
            numOfoutputDimensions = outD;
            numOfhiddenLayers = numHLs;
            neuronsPerHiddenLayer = hidD;
        }

        ~NeuralNetwork(){}

        void DeleteContents()
        {
            for(int i = 0; i < weights.size(); ++i)
            {
                delete weights[i];
                weights[i] = nullptr;
            }
            for(int i = 0; i < inputLayer.size(); ++i)
            {
                delete inputLayer[i];
                inputLayer[i] = nullptr;
            }
            for(int i = 0; i < hiddenLayers.size(); ++i)
            {
                for(int j = 0; j < hiddenLayers[i].size(); ++j)
                {
                    delete hiddenLayers[i][j];
                    hiddenLayers[i][j] = nullptr;
                }
            }
            for(int i = 0; i < outputLayer.size(); ++i)
            {
                delete outputLayer[i];
                outputLayer[i] = nullptr;
            }
        }

        void printWeights()
        {
            for(int i = 0; i < weights.size(); ++i)
            {
                std::cout << weights[i]->getWeight();
            }
            std::cout << "# of weights: ";
            std::cout << weights.size() << std::endl;
        }

        std::vector<Neuron*> returnOutput()
        {   
            return outputLayer;
        }

        std::vector<Weight*> GetWeights()
        {
            return weights;
        }


        void ComputeOutputLayer(std::vector<float> input)
        {   
            //set input layer to correct inputs
            for(int n = 0; n < numOfinputDimensions; ++n)
            {
                inputLayer[n]->setValue(input[n]);
            }

            //begin calculations
            for(int currentLayer = 0; currentLayer < numOfhiddenLayers; ++currentLayer)
            {
                for(int currentNeuron = 0; currentNeuron < neuronsPerHiddenLayer; ++currentNeuron)
                {
                    hiddenLayers[currentLayer][currentNeuron]->computeValue();
                }
            }

            //calculate output layer
            for(int currentY = 0; currentY < numOfoutputDimensions; ++currentY)
            {
                outputLayer[currentY]->computeValue();
            }
        }

        std::vector<Neuron*> GetNeurons()
        {
            return neurons;
        }

        float NetsSimilarityPercentage(NeuralNetwork* net)
        {   
            if((weights.size() == net->GetWeights().size()) && (neurons.size() == net->GetNeurons().size()))
            {
                int similarWeights = 0;
                for(int wIndex = 0; wIndex < weights.size(); ++wIndex)
                {
                    if(weights[wIndex]->getWeight() == net->GetWeights()[wIndex]->getWeight())
                    {
                        ++similarWeights;
                    }
                }
                int similarBiases = 0;
                 for(int nIndex = 0; nIndex < neurons.size(); ++nIndex)
                {
                    if(neurons[nIndex]->GetBias() == net->GetNeurons()[nIndex]->GetBias())
                    {
                        ++similarBiases;
                    }
                }
                float wtf = (similarBiases + similarWeights) / ((float)neurons.size() + (float)weights.size());
                return wtf;
            }
            else
            {
                return 0.0f;
            }
        }

        NeuralNetwork* CopyNet()
        {
            NeuralNetwork* newNet = new NeuralNetwork(numOfinputDimensions, numOfoutputDimensions, numOfhiddenLayers, neuronsPerHiddenLayer);
            std::vector<Weight*> newWeights = newNet->GetWeights();
            std::vector<Neuron*> newNeurons = newNet->GetNeurons();

            //copy values into new net
            for(int i = 0; i < weights.size(); ++i)
            {
                newWeights[i]->setWeight(weights[i]->getWeight());
            }
            for(int i = 0; i < neurons.size(); ++i)
            {
                newNeurons[i]->SetBias(neurons[i]->GetBias());
            }

            return newNet;
        }
        
        std::vector<Neuron*> GetInputLayer()
        {
            return inputLayer;
        }

        std::vector<Neuron*> GetOutputLayer()
        {
            return outputLayer;
        }

};


#endif