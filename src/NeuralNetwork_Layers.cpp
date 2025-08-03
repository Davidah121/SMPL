#include "NeuralNetwork.h"
#include "LCG.h"
#include "MathExt.h"
#include <time.h>
#include "StringTools.h"

namespace smpl
{
    NeuralLayer::~NeuralLayer()
    {

    }
    MatrixF NeuralLayer::feedForward(const MatrixF& input)
    {
        if(nextLayer != nullptr)
            return nextLayer->feedForward(solve(input));
        else
            return solve(input);
    }
    
    void NeuralLayer::setPreviousLayer(NeuralLayer* layer)
    {
        previousLayer = layer;
    }
    void NeuralLayer::setNextLayer(NeuralLayer* layer)
    {
        nextLayer = layer;
    }

    SigmoidActivationLayer::SigmoidActivationLayer()
    {

    }
    SigmoidActivationLayer::~SigmoidActivationLayer()
    {
        
    }
    
    MatrixF SigmoidActivationLayer::solve(const MatrixF& input)
    {
        MatrixF output = MatrixF(input.getRows(), input.getColumns());
        size_t totalSize = input.getRows() * input.getColumns();
        float* inputData = input.getData();
        float* outputData = output.getData();

        #if(OPTI == 0)
            #pragma omp parallel for
            for(size_t i=0; i<totalSize; i++)
            {
                outputData[i] = 1.0 / (1.0 + exp(-inputData[i]));
            }
        #elif(OPTI > 0)
            SIMD_FP32 negative1 = -1;
            SIMD_FP32 ones = 1;
		    int simdBound = SIMD_FP32::getSIMDBound(totalSize);
            #pragma omp parallel for
            for(size_t i=0; i<simdBound; i+=SIMD_FP32::SIZE)
            {
                SIMD_FP32 inputAsSIMD = SIMD_FP32::load(&inputData[i]);
                SIMD_FP32 negativeSIMD = SEML::negate(inputAsSIMD.values);
                SIMD_FP32 expValue = SEML::exp(negativeSIMD.values);
                SIMD_FP32 finalValue = SEML::reciprocal((ones+expValue).values);
                finalValue.store(&outputData[i]);
            }
            for(size_t i=simdBound; i<totalSize; i++)
            {
                outputData[i] = 1.0 / (1.0 + exp(-inputData[i]));
            }
        #endif

        return output;
    }

    MatrixF SigmoidActivationLayer::derivative(const MatrixF& input)
    {
        MatrixF matrixOf1s = MatrixF(input.getRows(), input.getColumns());
        matrixOf1s.setAllValues(1);

        //Fully multithreaded.
        return input.hadamardProduct(matrixOf1s - input);
    }

    TanhActivationLayer::TanhActivationLayer()
    {

    }
    TanhActivationLayer::~TanhActivationLayer()
    {
        
    }
    
    MatrixF TanhActivationLayer::solve(const MatrixF& input)
    {
        MatrixF output = MatrixF(input.getRows(), input.getColumns());
        size_t totalSize = input.getRows() * input.getColumns();
        float* inputData = input.getData();
        float* outputData = output.getData();

        #if(OPTI == 0)
            #pragma omp parallel for
            for(size_t i=0; i<totalSize; i++)
            {
                float eX = exp(inputData[i]);
                float eNegX = exp(-inputData[i]);
                outputData[i] = (eX - eNegX) / (eX + eNegX);
            }
        #elif(OPTI > 0)
		    int simdBound = SIMD_FP32::getSIMDBound(totalSize);
            #pragma omp parallel for
            for(size_t i=0; i<simdBound; i+=SIMD_FP32::SIZE)
            {
                SIMD_FP32 inputAsSIMD = SIMD_FP32::load(&inputData[i]);
                SIMD_FP32 finalValue = SEML::tanh(inputAsSIMD.values);
                finalValue.store(&outputData[i]);
            }
            for(size_t i=simdBound; i<totalSize; i++)
            {
                float eX = exp(inputData[i]);
                float eNegX = 1.0/eX;
                outputData[i] = (eX - eNegX) / (eX + eNegX);
            }
        #endif

        return output;
    }

    MatrixF TanhActivationLayer::derivative(const MatrixF& input)
    {
        MatrixF matrixOf1s = MatrixF(input.getRows(), input.getColumns());
        matrixOf1s.setAllValues(1);

        //Fully multithreaded.
        return matrixOf1s - input.hadamardProduct(input);
    }

    LinearActivationLayer::LinearActivationLayer()
    {

    }
    LinearActivationLayer::~LinearActivationLayer()
    {
        
    }
    
    MatrixF LinearActivationLayer::solve(const MatrixF& input)
    {
        return input;
    }

    MatrixF LinearActivationLayer::derivative(const MatrixF& input)
    {
        MatrixF matrixOf1s = MatrixF(input.getRows(), input.getColumns());
        matrixOf1s.setAllValues(1);
        return matrixOf1s;
    }

    ReluActivationLayer::ReluActivationLayer()
    {

    }
    ReluActivationLayer::~ReluActivationLayer()
    {
        
    }
    
    MatrixF ReluActivationLayer::solve(const MatrixF& input)
    {
        MatrixF output = MatrixF(input.getRows(), input.getColumns());
        size_t totalSize = input.getRows() * input.getColumns();
        float* inputData = input.getData();
        float* outputData = output.getData();

        #if(OPTI == 0)
            #pragma omp parallel for
            for(size_t i=0; i<totalSize; i++)
            {
                outputData[i] = (inputData[i] > 0) ? inputData[i] : 0;
            }
        #elif(OPTI > 0)
		    int simdBound = SIMD_FP32::getSIMDBound(totalSize);
		    SIMD_FP32 allZeros = 0;
            #pragma omp parallel for
            for(size_t i=0; i<simdBound; i+=SIMD_FP32::SIZE)
            {
                SIMD_FP32 inputAsSIMD = SIMD_FP32::load(&inputData[i]);
                SIMD_FP32 mask = inputAsSIMD > allZeros;
                SIMD_FP32 finalValue = SIMD_FP32_BLEND_MASK(inputAsSIMD.values, allZeros.values, mask.values);
                finalValue.store(&outputData[i]);
            }
            for(size_t i=simdBound; i<totalSize; i++)
            {
                outputData[i] = (inputData[i] > 0) ? inputData[i] : 0;
            }
        #endif

        return output;
    }

    MatrixF ReluActivationLayer::derivative(const MatrixF& input)
    {
        MatrixF output = MatrixF(input.getRows(), input.getColumns());
        size_t totalSize = input.getRows() * input.getColumns();
        float* inputData = input.getData();
        float* outputData = output.getData();

        #if(OPTI == 0)
            #pragma omp parallel for
            for(size_t i=0; i<totalSize; i++)
            {
                outputData[i] = (inputData[i] > 0) ? 1 : 0;
            }
        #elif(OPTI > 0)
		    int simdBound = SIMD_FP32::getSIMDBound(totalSize);
		    SIMD_FP32 allZeros = 0;
		    SIMD_FP32 allOnes = 1;
            #pragma omp parallel for
            for(size_t i=0; i<simdBound; i+=SIMD_FP32::SIZE)
            {
                SIMD_FP32 inputAsSIMD = SIMD_FP32::load(&inputData[i]);
                SIMD_FP32 mask = inputAsSIMD > allZeros;
                SIMD_FP32 finalValue = SIMD_FP32_BLEND_MASK(allOnes.values, allZeros.values, mask.values); //make mask function
                finalValue.store(&outputData[i]);
            }
            for(size_t i=simdBound; i<totalSize; i++)
            {
                outputData[i] = (inputData[i] > 0) ? 1 : 0;
            }
        #endif

        return output;
    }

    StepActivationLayer::StepActivationLayer()
    {

    }
    StepActivationLayer::~StepActivationLayer()
    {
        
    }
    
    MatrixF StepActivationLayer::solve(const MatrixF& input)
    {
        MatrixF output = MatrixF(input.getRows(), input.getColumns());
        size_t totalSize = input.getRows() * input.getColumns();
        float* inputData = input.getData();
        float* outputData = output.getData();

        #if(OPTI == 0)
            #pragma omp parallel for
            for(size_t i=0; i<totalSize; i++)
            {
                outputData[i] = (inputData[i] > 0) ? 1 : 0;
            }
        #elif(OPTI > 0)
		    int simdBound = SIMD_FP32::getSIMDBound(totalSize);
		    SIMD_FP32 allZeros = 0;
		    SIMD_FP32 allOnes = 1;
            #pragma omp parallel for
            for(size_t i=0; i<simdBound; i+=SIMD_FP32::SIZE)
            {
                SIMD_FP32 inputAsSIMD = SIMD_FP32::load(&inputData[i]);
                SIMD_FP32 mask = inputAsSIMD>allZeros;
                SIMD_FP32 finalValue = SIMD_FP32_BLEND_MASK(allOnes.values, allZeros.values, mask.values);
                finalValue.store(&outputData[i]);
            }
            for(size_t i=simdBound; i<totalSize; i++)
            {
                outputData[i] = (inputData[i] > 0) ? 1 : 0;
            }
        #endif

        return output;
    }

    MatrixF StepActivationLayer::derivative(const MatrixF& input)
    {
        return MatrixF(input.getRows(), input.getColumns()); //initialized to all 0s
    }

    
    FullyConnectedLayer::FullyConnectedLayer(size_t inputNeurons, size_t outputNeurons)
    {
        weights = MatrixF(inputNeurons, outputNeurons);
        bias = MatrixF(1, outputNeurons);
    }
    FullyConnectedLayer::~FullyConnectedLayer()
    {

    }
    MatrixF FullyConnectedLayer::solve(const MatrixF& input)
    {
        //xW + b
        return MatrixF::fusedMultiplyAdd(input, weights, bias);
    }
    MatrixF FullyConnectedLayer::derivative(const MatrixF& input)
    {
        //just W^t
        return weights.getTranspose();
    }

    
    InputLayer::InputLayer(const MatrixF& trainingData)
    {
        //find mean and std dev per column
        MatrixF variance;

        mean = trainingData.verticalSum() / trainingData.getRows();
        variance = trainingData.broadcastSubtract(mean).verticalSum() / trainingData.getRows();
        #if(OPTI >= 1)
        stddev = variance.broadcastFunction(MathExt::sqrt, nullptr);
        #else
        stddev = variance.broadcastFunction(MathExt::sqrt, nullptr);
        #endif

        //prevent 0 in stddev
        for(size_t i=0; i<stddev.getRows()*stddev.getColumns(); i++)
            if(stddev.getData()[i] == 0)
                stddev.getData()[i] = 1;
    }
    InputLayer::~InputLayer()
    {

    }
    
    MatrixF InputLayer::solve(const MatrixF& input)
    {
        //zscore on each column
        return input.broadcastSubtract(mean).broadcastInverseHadamardProduct(stddev);
    }
    MatrixF InputLayer::derivative(const MatrixF& input)
    {
        //throw exception lol
        return MatrixF();
    }
} //NAMESPACE glib END