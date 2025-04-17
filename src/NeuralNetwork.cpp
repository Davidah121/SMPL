#include "NeuralNetwork.h"
namespace smpl
{
    NeuralNetwork::NeuralNetwork()
    {

    }
    NeuralNetwork::~NeuralNetwork()
    {
        for(size_t i=0; i<layers.size(); i++)
        {
            if(layers[i] != nullptr)
                delete layers[i];
        }
        layers.clear();
    }

    void NeuralNetwork::addNextLayer(NeuralLayer* layer)
    {
        if(!layers.empty())
        {
            layer->setPreviousLayer(layers.back());
            layers.back()->setNextLayer(layer);
        }
        layers.push_back(layer);
    }
}