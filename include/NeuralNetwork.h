#pragma once
#include <vector>
#include "SimpleXml.h"

class Neuron
{
public:
    Neuron();
    ~Neuron();

    void setActivation(double v);
    double getActivation();

    double getWeight(int index);
    void setWeight(int index, double v);

    void resetWeights(int size);

    double& operator[](int index);

    int size();

    XmlNode* exportTestInformation(int count);
private:
    double activation = 0.0;
    std::vector<double> weightsToConnections = std::vector<double>();
};

class NeuralLayer
{
public:
    NeuralLayer(int size, NeuralLayer* nextLayer, NeuralLayer* previousLayer);
    ~NeuralLayer();

    NeuralLayer* getNextLayer();
    NeuralLayer* getPreviousLayer();

    void setNextLayer(NeuralLayer* n);
    void setPreviousLayer(NeuralLayer* n);
    
    void setNeuronActivation(int index, float activation);
    double getNeuronActivation(int index);

    Neuron& getNeuron(int index);

    std::vector<Neuron> getListOfNeurons();

    double activationFunction(double value);
    double derivativeActivationFunction(double value, bool outputOfActivation = false);

    void run();
    int size();

    void resetWeightsBias();
    void setBiasValue(int index, double value);
    int getBiasSize();

    XmlNode* exportTestInformation(int count);

    Neuron& operator[](int index);
private:
    int length = 0;
    std::vector<Neuron> neurons = std::vector<Neuron>();
    NeuralLayer* nextLayer = nullptr;
    NeuralLayer* previousLayer = nullptr;
    
    std::vector<double> biasToConnections = std::vector<double>();
};

class NeuralNetwork
{
public:
    NeuralNetwork();
    ~NeuralNetwork();

    void addLayerToEnd(int sizeOfLayer);
    std::vector<double> run(std::vector<double> inputs);
    void train(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput);
    void resetNetwork();

    int size();
    double getLearningRate();
    void setLearningRate(double v);
    
    void exportTestInformation(std::string filename);

    NeuralLayer* getStartLayer();
    NeuralLayer* getEndLayer();
private:
    NeuralLayer* startLayer = nullptr;
    NeuralLayer* endLayer = nullptr;
    double learningRate = 0.5;
};