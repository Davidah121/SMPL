#pragma once
//TODO: REWRITE THIS
#include "BuildOptions.h"
#include <vector>
#include "Matrix.h"
#include "SimpleXml.h"

namespace smpl
{
    class NeuralLayer;
    class NerualNetwork;

    class NeuralLayer
    {
    public:
        Matrix feedForward(const Matrix& input);
        virtual Matrix derivative(const Matrix& input) = 0;

    protected:
        virtual Matrix solve(const Matrix& input) = 0;
        // virtual Matrix backpropagation(const Matrix& input) = 0;
        
        NeuralLayer* previousLayer = nullptr;
        NeuralLayer* nextLayer = nullptr;
    private:
    };

    class InputLayer : public NeuralLayer
    {
    public:
        InputLayer(const Matrix& trainingData);
        ~InputLayer();
        
        Matrix solve(const Matrix& input);
        Matrix derivative(const Matrix& input);
    private:
        //stuff needed to zscore probably
    };

    class FullyConnectedLayer : public NeuralLayer
    {
    public:
        FullyConnectedLayer(size_t inputNeurons, size_t outputNeurons);
        ~FullyConnectedLayer();
        Matrix solve(const Matrix& input);
        Matrix derivative(const Matrix& input);
    private:
        Matrix weights;
        Matrix bias;
    };

    class SigmoidActivationLayer : public NeuralLayer
    {
    public:
        SigmoidActivationLayer();
        ~SigmoidActivationLayer();
        Matrix solve(const Matrix& input);
        Matrix derivative(const Matrix& input);
    private:
    };

    class TanhActivationLayer : public NeuralLayer
    {
    public:
        TanhActivationLayer();
        ~TanhActivationLayer();
        Matrix solve(const Matrix& input);
        Matrix derivative(const Matrix& input);
    private:
    };

    class LinearActivationLayer : public NeuralLayer
    {
    public:
        LinearActivationLayer();
        ~LinearActivationLayer();
        Matrix solve(const Matrix& input);
        Matrix derivative(const Matrix& input);
    private:
    };

    class ReluActivationLayer : public NeuralLayer
    {
    public:
        ReluActivationLayer();
        ~ReluActivationLayer();
        Matrix solve(const Matrix& input);
        Matrix derivative(const Matrix& input);
    private:
    };

    class StepActivationLayer : public NeuralLayer
    {
    public:
        StepActivationLayer();
        ~StepActivationLayer();
        Matrix solve(const Matrix& input);
        Matrix derivative(const Matrix& input);
    private:
    };

    // class NeuralLayer
    // {
    // public:
    //     /**
    //      * @brief Construct a new Neural Layer object
    //      *      Consists of Neurons and Bias values.
    //      * 
    //      * @param size 
    //      *      The amount of neurons in the layer
    //      * @param nextLayer 
    //      *      The next layer it is connected to.
    //      * @param previousLayer 
    //      *      The previous layer connecting to this layer.
    //      */
    //     NeuralLayer(int size, NeuralLayer* nextLayer, NeuralLayer* previousLayer);

    //     /**
    //      * @brief Destroy the Neural Layer object
    //      * 
    //      */
    //     ~NeuralLayer();

    //     /**
    //      * @brief Gets the Next Layer
    //      *      If there is no next layer, it is assumed to be the last layer and returns nullptr.
    //      * 
    //      * @return NeuralLayer* 
    //      */
    //     NeuralLayer* getNextLayer();

    //     /**
    //      * @brief Gets the Previous Layer
    //      *      If there is no previous layer, it is assumed to be the first layer and returns nullptr.
    //      * 
    //      * @return NeuralLayer* 
    //      */
    //     NeuralLayer* getPreviousLayer();

    //     /**
    //      * @brief Sets the Next Layer
    //      * 
    //      * @param n 
    //      */
    //     void setNextLayer(NeuralLayer* n);

    //     /**
    //      * @brief Sets the Previous Layer
    //      * 
    //      * @param n 
    //      */
    //     void setPreviousLayer(NeuralLayer* n);
        
    //     /**
    //      * @brief Set the Neuron Activation at the specified index.
    //      * 
    //      * @param index 
    //      * @param activation 
    //      */
    //     void setNeuronActivation(int index, double activation);

    //     /**
    //      * @brief Get the Neuron Activation at the specified index.
    //      * 
    //      * @param index 
    //      * @return double 
    //      */
    //     double getNeuronActivation(int index);

    //     /**
    //      * @brief Solves the activation function.
    //      *      The activation function is the Logistic Sigmoid.
    //      * 
    //      * @param value 
    //      * @return double 
    //      */
    //     double activationFunction(double value);

    //     /**
    //      * @brief Solves the derivative of the activation function.
    //      *      The activation function is the Logistic Sigmoid.
    //      * 
    //      * @param value 
    //      * @return double 
    //      */
    //     double derivativeActivationFunction(double value);

    //     Matrix getDerivativeMatrix();
    //     Matrix& getNeuronMatrix();
    //     Matrix& getWeightMatrix();
    //     Matrix& getBiasMatrix();


    //     /**
    //      * @brief Using the current activations, weights, and bias, solves for the activations for the next layer.
    //      * 
    //      */
    //     void run();

    //     /**
    //      * @brief Returns the amount of neurons in the layer.
    //      * 
    //      * @return int 
    //      */
    //     int size();

    //     /**
    //      * @brief Resets all weights and bias values to 0.
    //      * 
    //      */
    //     void resetWeightsBias();

    //     /**
    //      * @brief Sets the Bias Value at the specified index.
    //      * 
    //      * @param index 
    //      * @param value 
    //      */
    //     void setBiasValue(int index, double value);

    //     /**
    //      * @brief Gets the Bias Value at the specified index.
    //      * 
    //      * @param index 
    //      * @return double 
    //      */
    //     double getBiasValue(int index);

    //     /**
    //      * @brief Gets the amount of bias values.
    //      * 
    //      * @return int 
    //      */
    //     int getBiasSize();

    // private:
    //     int length = 0;
    //     NeuralLayer* nextLayer = nullptr;
    //     NeuralLayer* previousLayer = nullptr;
        
    //     Matrix neurons = Matrix();

    //     //weights and bias connected to this layer from the previous.
    //     Matrix weightsToConnections = Matrix();
    //     Matrix biasToConnections = Matrix();
    // };

    // class NeuralNetwork
    // {
    // public:
    //     /**
    //      * @brief Construct a new Neural Network object
    //      *      Stores the start neural layer and end neural layer.
    //      */
    //     NeuralNetwork();

    //     /**
    //      * @brief Destroy the Neural Network object
    //      * 
    //      */
    //     ~NeuralNetwork();

    //     /**
    //      * @brief Adds a new neural layer to the network.
    //      * 
    //      * @param sizeOfLayer 
    //      *      The amount of neurons in the layer.
    //      */
    //     void addLayerToEnd(int sizeOfLayer);

    //     /**
    //      * @brief Runs the neural layer using inputs as the initial activations.
    //      *      Inputs must be the same size as the start layer.
    //      * @param inputs 
    //      * @return std::vector<double> 
    //      */
    //     std::vector<double> run(std::vector<double> inputs);

    //     /**
    //      * @brief Trains the network using a series of inputs and their expected output.
    //      *      This should still be called multiple times.
    //      *      This takes the average of all the training adjustments over the expected outputs and then adjust the network.
    //      * @param inputs 
    //      * @param expectedOutput 
    //      */
    //     void train(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput);

    //     /**
    //      * @brief Gets the Cost of the network according to the given inputs.
    //      *      Returns the cost of each input.
    //      *      The total cost is the sum of the cost over all samples.
    //      * 
    //      * @param inputs 
    //      * @param expectedOutput 
    //      * @return std::vector<double> 
    //      */
    //     std::vector<double> getCost(std::vector<std::vector<double>> inputs, std::vector<std::vector<double>> expectedOutput);
        
    //     /**
    //      * @brief Resets the weights and bias values for the network.
    //      * 
    //      */
    //     void resetNetwork();

    //     /**
    //      * @brief Returns how many layers are in the Neural Network.
    //      * 
    //      * @return int 
    //      */
    //     int size();

    //     /**
    //      * @brief Gets the Learning Rate.
    //      * 
    //      * @return double 
    //      */
    //     double getLearningRate();

    //     /**
    //      * @brief Sets the Learning Rate.
    //      *      Setting lower reduces the chance for over fitting but requires more training. Setting higher increases the chance
    //      *      for over fitting but requires less training.
    //      * 
    //      * @param v 
    //      */
    //     void setLearningRate(double v);

    //     /**
    //      * @brief Gets the Start Layer
    //      * 
    //      * @return NeuralLayer* 
    //      */
    //     NeuralLayer* getStartLayer();

    //     /**
    //      * @brief Gets the End Layer
    //      * 
    //      * @return NeuralLayer* 
    //      */
    //     NeuralLayer* getEndLayer();
    // private:
    //     NeuralLayer* startLayer = nullptr;
    //     NeuralLayer* endLayer = nullptr;
    //     double learningRate = 0.03;
    // };

} //NAMESPACE smpl END