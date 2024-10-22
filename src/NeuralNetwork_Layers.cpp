#include "NeuralNetwork.h"
#include "LCG.h"
#include "MathExt.h"
#include <time.h>
#include "StringTools.h"

//TODO: ADD SIMD FUNCTIONS
namespace smpl
{
    Matrix NeuralLayer::feedForward(const Matrix& input)
    {
        if(nextLayer != nullptr)
            return nextLayer->feedForward(solve(input));
        else
            return solve(input);
    }

    SigmoidActivationLayer::SigmoidActivationLayer()
    {

    }
    SigmoidActivationLayer::~SigmoidActivationLayer()
    {
        
    }
    
    Matrix SigmoidActivationLayer::solve(const Matrix& input)
    {
        Matrix output = Matrix(input.getRows(), input.getCols());
        size_t totalSize = input.getRows() * input.getCols();
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
                SIMD_FP32 negativeSIMD = negative1*inputAsSIMD;
                SIMD_FP32 expValue = MathExt::exp(negativeSIMD.values);
                SIMD_FP32 finalValue = MathExt::reciprocal((ones+expValue).values);
                finalValue.store(&outputData[i]);
            }
            for(size_t i=simdBound; i<totalSize; i++)
            {
                outputData[i] = 1.0 / (1.0 + exp(-inputData[i]));
            }
        #endif

        return output;
    }

    Matrix SigmoidActivationLayer::derivative(const Matrix& input)
    {
        Matrix matrixOf1s = Matrix(input.getRows(), input.getCols());
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
    
    Matrix TanhActivationLayer::solve(const Matrix& input)
    {
        Matrix output = Matrix(input.getRows(), input.getCols());
        size_t totalSize = input.getRows() * input.getCols();
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
            SIMD_FP32 negative1 = -1;
		    int simdBound = SIMD_FP32::getSIMDBound(totalSize);
            #pragma omp parallel for
            for(size_t i=0; i<simdBound; i+=SIMD_FP32::SIZE)
            {
                SIMD_FP32 inputAsSIMD = SIMD_FP32::load(&inputData[i]);
                SIMD_FP32 negativeSIMD = negative1*inputAsSIMD;

                SIMD_FP32 expValue = MathExt::exp(inputAsSIMD.values);
                SIMD_FP32 negExpValue = MathExt::exp(negativeSIMD.values);
                
                SIMD_FP32 finalValue = (expValue-negExpValue)/(expValue+negExpValue);
                finalValue.store(&outputData[i]);
            }
            for(size_t i=simdBound; i<totalSize; i++)
            {
                float eX = exp(inputData[i]);
                float eNegX = exp(-inputData[i]);
                outputData[i] = (eX - eNegX) / (eX + eNegX);
            }
        #endif

        return output;
    }

    Matrix TanhActivationLayer::derivative(const Matrix& input)
    {
        Matrix matrixOf1s = Matrix(input.getRows(), input.getCols());
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
    
    Matrix LinearActivationLayer::solve(const Matrix& input)
    {
        return input;
    }

    Matrix LinearActivationLayer::derivative(const Matrix& input)
    {
        Matrix matrixOf1s = Matrix(input.getRows(), input.getCols());
        matrixOf1s.setAllValues(1);
        return matrixOf1s;
    }

    ReluActivationLayer::ReluActivationLayer()
    {

    }
    ReluActivationLayer::~ReluActivationLayer()
    {
        
    }
    
    Matrix ReluActivationLayer::solve(const Matrix& input)
    {
        Matrix output = Matrix(input.getRows(), input.getCols());
        size_t totalSize = input.getRows() * input.getCols();
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

    Matrix ReluActivationLayer::derivative(const Matrix& input)
    {
        Matrix output = Matrix(input.getRows(), input.getCols());
        size_t totalSize = input.getRows() * input.getCols();
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
    
    Matrix StepActivationLayer::solve(const Matrix& input)
    {
        Matrix output = Matrix(input.getRows(), input.getCols());
        size_t totalSize = input.getRows() * input.getCols();
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

    Matrix StepActivationLayer::derivative(const Matrix& input)
    {
        return Matrix(input.getRows(), input.getCols()); //initialized to all 0s
    }

    
    FullyConnectedLayer::FullyConnectedLayer(size_t inputNeurons, size_t outputNeurons)
    {
        weights = Matrix(inputNeurons, outputNeurons);
        bias = Matrix(1, outputNeurons);
    }
    FullyConnectedLayer::~FullyConnectedLayer()
    {

    }
    Matrix FullyConnectedLayer::solve(const Matrix& input)
    {
        //xW + b
        return Matrix::fusedMultiplyAdd(input, weights, bias);
    }
    Matrix FullyConnectedLayer::derivative(const Matrix& input)
    {
        //just W^t
        return weights.getTranspose();
    }

    
    InputLayer::InputLayer(const Matrix& trainingData)
    {
        //find mean and std dev per column
        
    }
    InputLayer::~InputLayer()
    {

    }
    
    Matrix InputLayer::solve(const Matrix& input)
    {
        //zscore on each column
        return Matrix();
    }
    Matrix InputLayer::derivative(const Matrix& input)
    {
        //throw exception lol
        return Matrix();
    }
} //NAMESPACE glib END