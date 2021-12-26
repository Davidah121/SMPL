#pragma once
#include <vector>
#include "SimpleXml.h"

namespace glib
{

    class Neuron
    {
    public:
        /**
         * @brief Construct a new Neuron object
         *      A neuron will hold its current activation, and its weights to all connections.
         *      Assumes that the neuron is connected to all neurons in the next layer.
         * 
         */
        Neuron();

        /**
         * @brief Destroy the Neuron object
         * 
         */
        ~Neuron();

        /**
         * @brief Set the Activation of the neuron.
         * 
         * @param v 
         */
        void setActivation(double v);

        /**
         * @brief Get the Activation of the neuron.
         * 
         * @return double 
         */
        double getActivation();

        /**
         * @brief Get the Weight to the specified connection.
         * 
         * @param index 
         * @return double 
         */
        double getWeight(int index);

        /**
         * @brief Sets the Weight to the specified connection.
         * 
         * @param index 
         * @param v 
         */
        void setWeight(int index, double v);

        /**
         * @brief Resets the weights for the neuron.
         * 
         * @param size 
         *      The amount of connections.
         */
        void resetWeights(int size);

        /**
         * @brief Returns the weight to a connection
         * 
         * @param index 
         * @return double& 
         */
        double& operator[](int index);

        /**
         * @brief Returns how many connections for the neuron.
         * 
         * @return int 
         */
        int size();

        /**
         * @brief An internal tool for exporting Network information.
         * 
         * @param count 
         *      The neuron number I guess?
         * @return XmlNode* 
         */
        XmlNode* exportTestInformation(int count);
        
        void importNeuron(XmlNode* node);
    private:
        double activation = 0.0;
        std::vector<double> weightsToConnections = std::vector<double>();
    };

    class NeuralLayer
    {
    public:
        /**
         * @brief Construct a new Neural Layer object
         *      Consists of Neurons and Bias values.
         * 
         * @param size 
         *      The amount of neurons in the layer
         * @param nextLayer 
         *      The next layer it is connected to.
         * @param previousLayer 
         *      The previous layer connecting to this layer.
         */
        NeuralLayer(int size, NeuralLayer* nextLayer, NeuralLayer* previousLayer);

        /**
         * @brief Destroy the Neural Layer object
         * 
         */
        ~NeuralLayer();

        /**
         * @brief Gets the Next Layer
         *      If there is no next layer, it is assumed to be the last layer and returns nullptr.
         * 
         * @return NeuralLayer* 
         */
        NeuralLayer* getNextLayer();

        /**
         * @brief Gets the Previous Layer
         *      If there is no previous layer, it is assumed to be the first layer and returns nullptr.
         * 
         * @return NeuralLayer* 
         */
        NeuralLayer* getPreviousLayer();

        /**
         * @brief Sets the Next Layer
         * 
         * @param n 
         */
        void setNextLayer(NeuralLayer* n);

        /**
         * @brief Sets the Previous Layer
         * 
         * @param n 
         */
        void setPreviousLayer(NeuralLayer* n);
        
        /**
         * @brief Set the Neuron Activation at the specified index.
         * 
         * @param index 
         * @param activation 
         */
        void setNeuronActivation(int index, float activation);

        /**
         * @brief Get the Neuron Activation at the specified index.
         * 
         * @param index 
         * @return double 
         */
        double getNeuronActivation(int index);

        /**
         * @brief Gets a Neuron
         * 
         * @param index 
         * @return Neuron& 
         */
        Neuron& getNeuron(int index);

        /**
         * @brief Gets the List Of Neurons
         * 
         * @return std::vector<Neuron>& 
         */
        std::vector<Neuron>& getListOfNeurons();

        /**
         * @brief Solves the activation function.
         *      The activation function is the Logistic Sigmoid.
         * 
         * @param value 
         * @return double 
         */
        double activationFunction(double value);

        /**
         * @brief Solves the derivative of the activation function.
         *      The activation function is the Logistic Sigmoid.
         * 
         * @param value 
         * @return double 
         */
        double derivativeActivationFunction(double value);

        /**
         * @brief Using the current activations, weights, and bias, solves for the activations for the next layer.
         * 
         */
        void run();

        /**
         * @brief Returns the amount of neurons in the layer.
         * 
         * @return int 
         */
        int size();

        /**
         * @brief Resets all weights and bias values to 0.
         * 
         */
        void resetWeightsBias();

        /**
         * @brief Sets the Bias Value at the specified index.
         * 
         * @param index 
         * @param value 
         */
        void setBiasValue(int index, double value);

        /**
         * @brief Gets the Bias Value at the specified index.
         * 
         * @param index 
         * @return double 
         */
        double getBiasValue(int index);

        /**
         * @brief Gets the amount of bias values.
         * 
         * @return int 
         */
        int getBiasSize();

        /**
         * @brief An internal tool for exporting Network information.
         * 
         * @param count 
         *      The neural layer number I guess?
         * @return XmlNode* 
         */
        XmlNode* exportTestInformation(int count);

        void importLayer(XmlNode* node);

        /**
         * @brief Returns a Neuron
         * 
         * @param index 
         * @return Neuron& 
         */
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
        /**
         * @brief Construct a new Neural Network object
         *      Stores the start neural layer and end neural layer.
         */
        NeuralNetwork();

        /**
         * @brief Destroy the Neural Network object
         * 
         */
        ~NeuralNetwork();

        /**
         * @brief Adds a new neural layer to the network.
         * 
         * @param sizeOfLayer 
         *      The amount of neurons in the layer.
         */
        void addLayerToEnd(int sizeOfLayer);

        /**
         * @brief Runs the neural layer using inputs as the initial activations.
         *      Inputs must be the same size as the start layer.
         * @param inputs 
         * @return std::vector<double> 
         */
        std::vector<double> run(std::vector<double> inputs);

        /**
         * @brief Trains the network using a series of inputs and their expected output.
         *      This should still be called multiple times.
         *      This takes the average of all the training adjustments over the expected outputs and then adjust the network.
         * @param inputs 
         * @param expectedOutput 
         */
        void train(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput);

        void testTrain(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput);
        std::vector<double> getCost(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput);
        
        /**
         * @brief Resets the weights and bias values for the network.
         * 
         */
        void resetNetwork();

        /**
         * @brief Returns how many layers are in the Neural Network.
         * 
         * @return int 
         */
        int size();

        /**
         * @brief Gets the Learning Rate.
         * 
         * @return double 
         */
        double getLearningRate();

        /**
         * @brief Sets the Learning Rate.
         *      Setting lower reduces the chance for over fitting but requires more training. Setting higher increases the chance
         *      for over fitting but requires less training.
         * 
         * @param v 
         */
        void setLearningRate(double v);
        
        /**
         * @brief Internal tool for analysing neural networks
         * 
         * @param filename 
         */
        void exportTestInformation(std::string filename);

        void importNetwork(File f);

        /**
         * @brief Gets the Start Layer
         * 
         * @return NeuralLayer* 
         */
        NeuralLayer* getStartLayer();

        /**
         * @brief Gets the End Layer
         * 
         * @return NeuralLayer* 
         */
        NeuralLayer* getEndLayer();
    private:
        NeuralLayer* startLayer = nullptr;
        NeuralLayer* endLayer = nullptr;
        double learningRate = 0.5;
    };

} //NAMESPACE glib END