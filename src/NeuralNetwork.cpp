#include "NeuralNetwork.h"
#include "LCG.h"
#include "MathExt.h"
#include <time.h>
#include "StringTools.h"

namespace glib
{
    #pragma region NEURAL_LAYER
    NeuralLayer::NeuralLayer(int size, NeuralLayer* nextLayer, NeuralLayer* previousLayer)
    {
        length = size;
        neurons = Matrix(length, 1);

        setNextLayer(nextLayer);
        setPreviousLayer(previousLayer);
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
        if(nextLayer != nullptr)
        {
            n->weightsToConnections = Matrix(nextLayer->length, length);
            n->biasToConnections = Matrix(nextLayer->length, 1);

            n->previousLayer = this;
        }
    }

    void NeuralLayer::setPreviousLayer(NeuralLayer* n)
    {
        if(n != nullptr)
            n->setNextLayer(this);
    }

    int NeuralLayer::size()
    {
        return length;
    }

    void NeuralLayer::setNeuronActivation(int index, double activation)
    {
        neurons[index][0] = activation;
    }

    double NeuralLayer::getNeuronActivation(int index)
    {
        return neurons[index][0];
    }

    void NeuralLayer::run()
    {
        if(nextLayer != nullptr)
        {
            // int nSize = nextLayer->size();
            nextLayer->neurons = (nextLayer->weightsToConnections * neurons) + nextLayer->biasToConnections;

            for(int i=0; i<nextLayer->neurons.getRows(); i++)
            {
                for(int j=0; j<nextLayer->neurons.getCols(); j++)
                {
                    nextLayer->neurons[i][j] = nextLayer->activationFunction(nextLayer->neurons[i][j]);
                }
            }
        }
    }

    void NeuralLayer::resetWeightsBias()
    {
        if(nextLayer!=nullptr)
        {
            srand(time(nullptr));
            LCG random = LCG(rand());

            for(int i=0; i<weightsToConnections.getRows(); i++)
            {
                for(int i2=0; i2<weightsToConnections.getCols(); i2++)
                {
                    double weightValue = (double)(random.get() % 1000000) - 500000;
                    weightValue /= 500000;

                    weightsToConnections[i][i2] = weightValue;
                }
            }

            for(int i=0; i<biasToConnections.getRows(); i++)
            {
                biasToConnections[i][0] = 0;
            }
        }

    }

    double NeuralLayer::activationFunction(double value)
    {
        return MathExt::logisticsSigmoid(value);
    }

    double NeuralLayer::derivativeActivationFunction(double value)
    {
        return value*(1-value);
    }

    Matrix NeuralLayer::getDerivativeMatrix()
    {
        Matrix result = neurons;
        for(int i=0; i<result.getRows(); i++)
        {
            result[i][0] = derivativeActivationFunction( neurons[i][0] );
        }
        return result;
    }

    Matrix& NeuralLayer::getNeuronMatrix()
    {
        return neurons;
    }

    Matrix& NeuralLayer::getWeightMatrix()
    {
        return weightsToConnections;
    }

    Matrix& NeuralLayer::getBiasMatrix()
    {
        return biasToConnections;
    }

    void NeuralLayer::setBiasValue(int index, double value)
    {
        biasToConnections[index][0] = value;
    }

    double NeuralLayer::getBiasValue(int index)
    {
        return biasToConnections[index][0];
    }

    int NeuralLayer::getBiasSize()
    {
        return biasToConnections.getRows();
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

        if((int)inputs.size() == startLayer->size())
        {
            for(size_t i=0; i<inputs.size(); i++)
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
        int numberOfLayers = size();

        std::vector< Matrix > weightAdjustments = std::vector< Matrix >(numberOfLayers);
        std::vector< Matrix > biasAdjustments = std::vector< Matrix >(numberOfLayers);
        std::vector< Matrix > layerDelta = std::vector< Matrix >(numberOfLayers);

        // NeuralLayer* currLayer = startLayer; //Not used

        if(inputs.size() != expectedOutput.size())
            return;

        size_t sLayerSize = startLayer->size();
        size_t eLayerSize = endLayer->size();
        
        double averageMult = 1.0 / expectedOutput.size();

        //set initial sizes
        NeuralLayer* layer = startLayer;
        for(int i=0; i<numberOfLayers; i++)
        {
            weightAdjustments[i] = Matrix(layer->getWeightMatrix().getRows(), layer->getWeightMatrix().getCols());
            biasAdjustments[i] = Matrix(layer->getBiasMatrix().getRows(), layer->getBiasMatrix().getCols());
            layerDelta[i] = Matrix(layer->getNeuronMatrix().getRows(), layer->getNeuronMatrix().getCols());
            layer = layer->getNextLayer();
        }

        for(size_t iterations=0; iterations<expectedOutput.size(); iterations++)
        {
            //apply x number of tests
            if(inputs[iterations].size() != sLayerSize)
                break;

            if(expectedOutput[iterations].size() != eLayerSize)
                break;

            //run inputs
            std::vector<double> runOutputs = run(inputs[iterations]);

            //calculate last layer error
            int currLayerNumber = numberOfLayers-1;
            layer = endLayer;

            Matrix currLayerDelta = layerDelta[currLayerNumber];

            //manual calculation for last layer delta
            for(int k=0; k<currLayerDelta.getRows(); k++)
            {
                layerDelta[currLayerNumber][k][0] = (runOutputs[k] - expectedOutput[iterations][k]) * layer->derivativeActivationFunction(runOutputs[k]);
            }
            
            NeuralLayer* lMinus = layer->getPreviousLayer();
            Matrix temp = layerDelta[currLayerNumber] * lMinus->getNeuronMatrix().getTranspose();
            weightAdjustments[currLayerNumber] += temp;
            biasAdjustments[currLayerNumber] += layerDelta[currLayerNumber];

            layer = layer->getPreviousLayer();
            currLayerNumber--;

            while(currLayerNumber > 0)
            {
                NeuralLayer* lPlus = layer->getNextLayer();
                NeuralLayer* lMinus = layer->getPreviousLayer();

                Matrix temp = (lPlus->getWeightMatrix().getTranspose() * layerDelta[currLayerNumber+1]);
                layerDelta[currLayerNumber] = temp.hadamardProduct( layer->getDerivativeMatrix() );
                
                temp = layerDelta[currLayerNumber] * lMinus->getNeuronMatrix().getTranspose();
                weightAdjustments[currLayerNumber] += temp;
                biasAdjustments[currLayerNumber] += layerDelta[currLayerNumber];

                layer = layer->getPreviousLayer();
                currLayerNumber--;
            }
        }

        layer = startLayer;

        for(size_t n=0; n<layerDelta.size(); n++)
        {
            //update weight values
            layer->getWeightMatrix() -= weightAdjustments[n]*learningRate*averageMult;

            //update bias values
            layer->getBiasMatrix() -= biasAdjustments[n]*learningRate*averageMult;

            layer = layer->getNextLayer();
        }
    }

    std::vector<double> NeuralNetwork::getCost(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput)
    {
        std::vector<double> results;

        if(inputs.size() != expectedOutput.size())
            return results;

        for(size_t iterations=0; iterations<expectedOutput.size(); iterations++)
        {
            //apply x number of tests

            //run inputs
            std::vector<double> runOutputs = run(inputs[iterations]);
            double cost = 0;
            for(size_t k=0; k<runOutputs.size(); k++)
            {
                cost += 0.5 * MathExt::sqr(expectedOutput[iterations][k] - runOutputs[k]);
            }

            results.push_back( cost );
        }

        return results;
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

    NeuralLayer* NeuralNetwork::getStartLayer()
    {
        return startLayer;
    }

    NeuralLayer* NeuralNetwork::getEndLayer()
    {
        return endLayer;
    }

    #pragma endregion

} //NAMESPACE glib END