#include "NeuralNetwork.h"
#include "LCG.h"
#include "MathExt.h"
#include <time.h>
#include "StringTools.h"

#pragma region NEURON
Neuron::Neuron()
{
}

Neuron::~Neuron()
{
}

void Neuron::setActivation(double v)
{
    activation = v;
}

double Neuron::getActivation()
{
    return activation;
}

double Neuron::getWeight(int index)
{
    return weightsToConnections[index];
}

void Neuron::setWeight(int index, double v)
{
    weightsToConnections[index] = v;
}

void Neuron::resetWeights(int size)
{
    weightsToConnections.clear();
    
    int modV = 100000;
    double divV = (double)modV / 20.0;
    double minusVal = (double)modV / 2.0;
    LCG r = LCG(rand(), 12345, 0, modV);
    
    for(int i=0; i<size; i++)
    {
        double w = ((double)r.get() - minusVal) / divV;
        weightsToConnections.push_back( w );
    }
}

double& Neuron::operator[](int index)
{
    return weightsToConnections[index];
}

int Neuron::size()
{
    return weightsToConnections.size();
}

XmlNode* Neuron::exportTestInformation(int count)
{
    //Neuron header information
    XmlNode* neuronHeader = new XmlNode();
    neuronHeader->title = "Neuron";

    XmlAttribute neuronAttrib = XmlAttribute();
    neuronAttrib.name = "NeuronNumber";
    neuronAttrib.value = std::to_string(count);
    neuronHeader->attributes.push_back( neuronAttrib );

    XmlNode* neuronWeights = new XmlNode();
    neuronWeights->title = "NeuronWeights";
    
    for(int i=0; i<weightsToConnections.size(); i++)
    {
        XmlNode* weightValue = new XmlNode();
        
        weightValue->title = "WeightValue";
        weightValue->value = std::to_string(weightsToConnections[i]);

        XmlAttribute weightAttrib = XmlAttribute();
        weightAttrib.name = "WeightNumber";
        weightAttrib.value = std::to_string(i);

        weightValue->attributes.push_back(weightAttrib);

        neuronWeights->childNodes.push_back(weightValue);
    }

    neuronHeader->childNodes.push_back(neuronWeights);
        
    return neuronHeader;
}

#pragma endregion

#pragma region NEURAL_LAYER
NeuralLayer::NeuralLayer(int size, NeuralLayer* nextLayer, NeuralLayer* previousLayer)
{
    length = size;
    this->nextLayer = nextLayer;
    this->previousLayer = previousLayer;

    for(int i=0; i<size; i++)
    {
        neurons.push_back( Neuron() );
    }
}

NeuralLayer::~NeuralLayer()
{
    
}

NeuralLayer* NeuralLayer::getNextLayer()
{
    return nextLayer;
}

NeuralLayer* NeuralLayer::getPreviousLayer()
{
    return previousLayer;
}

void NeuralLayer::setNextLayer(NeuralLayer* n)
{
    nextLayer = n;
}

void NeuralLayer::setPreviousLayer(NeuralLayer* n)
{
    previousLayer = n;
}

int NeuralLayer::size()
{
    return length;
}

void NeuralLayer::setNeuronActivation(int index, float activation)
{
    neurons[index].setActivation(activation);
}

double NeuralLayer::getNeuronActivation(int index)
{
    return neurons[index].getActivation();
}

Neuron& NeuralLayer::getNeuron(int index)
{
    return neurons[index];
}

std::vector<Neuron> NeuralLayer::getListOfNeurons()
{
    return neurons;
}

void NeuralLayer::run()
{
    if(nextLayer != nullptr)
    {
        int nSize = nextLayer->size();
        std::vector<double> results = std::vector<double>(nSize);
        for(int i=0; i<nSize; i++)
        {
            results[i] = 0;
        }

        for(int i=0; i<length; i++)
        {
            for(int i2=0; i2<nSize; i2++)
            {
                results[i2] += neurons[i].getActivation() * neurons[i].getWeight(i2);
            }
        }

        for(int i2=0; i2<nSize; i2++)
        {
            nextLayer->setNeuronActivation(i2, activationFunction( results[i2] + biasToConnections[i2] ) );
        }
    }
}

void NeuralLayer::resetWeightsBias()
{
    if(nextLayer!=nullptr)
    {
        for(Neuron& k : neurons)
        {
            k.resetWeights( nextLayer->size());
        }

        biasToConnections.clear();

        int modV = 100000;
        double divV = (double)modV / 10.0;
        double minusVal = (double)modV / 2.0;
        LCG r = LCG(rand(), 12345, 0, modV);

        for(int i=0; i<nextLayer->size(); i++)
        {
            double b = ((double)r.get() - minusVal) / divV;
            biasToConnections.push_back(b);
        }
    }

}

double NeuralLayer::activationFunction(double value)
{
    return MathExt::logisticsSigmoid(value);
}

double NeuralLayer::derivativeActivationFunction(double value, bool outputOfActivation)
{
    if(outputOfActivation)
        return value*(1-value);
    
    double v = MathExt::logisticsSigmoid(value);
    return v * (1-v);
}

Neuron& NeuralLayer::operator[](int index)
{
    return neurons[index];
}

XmlNode* NeuralLayer::exportTestInformation(int count)
{
    //layer header information
    XmlNode* layerHeader = new XmlNode();
    layerHeader->title = "Layer";

    XmlAttribute layerAttrib = XmlAttribute();
    layerAttrib.name = "LayerNumber";
    layerAttrib.value = std::to_string(count);
    layerHeader->attributes.push_back( layerAttrib );

    XmlNode* layerBias = new XmlNode();
    layerBias->title = "LayerBias";
    
    for(int i=0; i<biasToConnections.size(); i++)
    {
        XmlNode* biasValue = new XmlNode();

        XmlAttribute biasAttrib = XmlAttribute();
        biasAttrib.name = "BiasNumber";
        biasAttrib.value = std::to_string(i);

        biasValue->attributes.push_back(biasAttrib);

        biasValue->title = "BiasValue";
        biasValue->value = std::to_string( biasToConnections[i] );

        layerBias->childNodes.push_back(biasValue);
    }

    layerHeader->childNodes.push_back(layerBias);
    for( int i=0; i<neurons.size(); i++)
    {
        layerHeader->childNodes.push_back( neurons[i].exportTestInformation(i) );
    }

    return layerHeader;

}

void NeuralLayer::setBiasValue(int index, double value)
{
    biasToConnections[index] = value;
}

int NeuralLayer::getBiasSize()
{
    return biasToConnections.size();
}

#pragma endregion


#pragma region NEURAL_NETWORK
NeuralNetwork::NeuralNetwork()
{
    
}

NeuralNetwork::~NeuralNetwork()
{
}

void NeuralNetwork::addLayerToEnd(int sizeOfLayer)
{
    NeuralLayer* nLayer = new NeuralLayer(sizeOfLayer, nullptr, endLayer);

    if(startLayer==nullptr)
    {
        startLayer = nLayer;
    }
    else
    {
        if(startLayer->getNextLayer() == nullptr)
        {
            startLayer->setNextLayer(nLayer);
        }
    }
    
    if(endLayer != nullptr)
    {
        endLayer->setNextLayer(nLayer);
    }
    
    endLayer = nLayer;
}

std::vector<double> NeuralNetwork::run(std::vector<double> inputs)
{
    NeuralLayer* l = startLayer;

    if(inputs.size() == startLayer->size())
    {
        for(int i=0; i<inputs.size(); i++)
        {
            startLayer->setNeuronActivation(i, inputs[i]);
        }
    }
    else
    {
        return std::vector<double>();
    }
    
    while(l!=nullptr)
    {
        if(l->getNextLayer() == nullptr)
        {
            break;
        }
        l->run();
        l = l->getNextLayer();
    }

    std::vector<double> activations = std::vector<double>();

    for(int i=0; i<l->size(); i++)
    {
        activations.push_back(l->getNeuronActivation(i));
    }

    return activations;
}

void NeuralNetwork::train(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput)
{
    std::vector< std::vector< std::vector<double> > > weightAdjustments = std::vector< std::vector< std::vector<double> > >();
    std::vector< std::vector<double> > layerDelta = std::vector< std::vector<double> >();

    // weightAdjustments[layers][neuronsInLayer][weightsPerNeuron];

    NeuralLayer* currLayer = endLayer;
    NeuralLayer* preLayer = endLayer->getPreviousLayer();

    NeuralLayer* s = startLayer;

    //populate the arrays based on our layers
    int layerIndex = 0;
    while(s != nullptr)
    {
        int neuronSize = s->size();
        layerDelta.push_back( std::vector<double>(neuronSize) );
        weightAdjustments.push_back( std::vector< std::vector<double> >( neuronSize) );
        for(int i=0; i<neuronSize; i++)
        {
            int weightSize = s->getNeuron(i).size();
            weightAdjustments[ layerIndex ][i] = std::vector<double>(weightSize);
        }

        s = s->getNextLayer();
        layerIndex++;
    }

    int i = layerDelta.size()-1;

    double avgMultiplier = 1.0 / inputs.size();

    for(int x=0; x<inputs.size(); x++)
    {
        std::vector<double> actualOutputs = run(inputs[x]);

        //Gradient Descent using back propagation. Adjust weights but not bias
        while(preLayer != nullptr)
        {
            if(currLayer == endLayer)
            {
                for(int i2=0; i2<layerDelta[i].size(); i2++)
                {
                    double neuronActivation = currLayer->getNeuronActivation(i2);
                    double p1 = (neuronActivation - expectedOutput[x][i2]);
                    double p2 = preLayer->derivativeActivationFunction( neuronActivation, true);
                    layerDelta[i][i2] =  p1 * p2;
                }
            }
            else
            {
                for(int i2=0; i2<layerDelta[i].size(); i2++)
                {
                    double summation = 0;
                    for(int k=0; k<layerDelta[i+1].size(); k++)
                    {
                        summation += ( (*currLayer)[i2][k] * layerDelta[i+1][k] ); 
                    }

                    layerDelta[i][i2] = summation * preLayer->derivativeActivationFunction( currLayer->getNeuronActivation(i2), true);
                }
            }

            //can solve for previous Layer weight deltas
            //the last layer does not have weights so start at the layer before the last
            int neuronSize = preLayer->size();
            for(int k=0; k<neuronSize; k++)
            {
                int weightSize = preLayer->getNeuron(k).size();
                for(int h=0; h<weightSize; h++)
                {
                    //weight adjustments should be the average adjustment over all samples
                    weightAdjustments[i-1][k][h] += (layerDelta[i][h] * preLayer->getNeuronActivation(k)) * avgMultiplier;
                }
            }

            i--;
            currLayer = preLayer;
            preLayer = preLayer->getPreviousLayer();
        }
        //after back propagation
        currLayer = endLayer;
        preLayer = currLayer->getPreviousLayer();
        i=layerDelta.size()-1;
    }
    //after training on samples
    
    //set the weights for each neuron

    NeuralLayer* l = startLayer;
    i=0;
    while(l->getNextLayer()!=nullptr)
    {
        for(int k=0; k<l->size(); k++)
        {
            for(int h=0; h<(*l)[k].size(); h++)
            {
                (*l)[k][h] -= weightAdjustments[i][k][h] * learningRate;
            }
        }
        i++;
        l = l->getNextLayer();
    }
    
}

void NeuralNetwork::resetNetwork()
{
    NeuralLayer* l = startLayer;

    while(l!=nullptr)
    {
        l->resetWeightsBias();
        l = l->getNextLayer();
    }
}

int NeuralNetwork::size()
{
    int count = 0;
    NeuralLayer* l = startLayer;
    while(l != nullptr)
    {
        count++;
        l = l->getNextLayer();
    }

    return count;
}

double NeuralNetwork::getLearningRate()
{
    return learningRate;
}
void NeuralNetwork::setLearningRate(double v)
{
    learningRate = v;
}

void NeuralNetwork::exportTestInformation(std::string filename)
{
    XmlNode* header = new XmlNode();
    header->title = "NeuralNetwork";

    int count = 0;
    NeuralLayer* l = startLayer;
    while(l != nullptr)
    {
        header->childNodes.push_back( l->exportTestInformation(count) );

        count++;
        l = l->getNextLayer();
    }

   SimpleXml file = SimpleXml();
   file.nodes.push_back(header);
   file.save(filename);
}

NeuralLayer* NeuralNetwork::getStartLayer()
{
    return startLayer;
}

NeuralLayer* NeuralNetwork::getEndLayer()
{
    return endLayer;
}

#pragma endregion