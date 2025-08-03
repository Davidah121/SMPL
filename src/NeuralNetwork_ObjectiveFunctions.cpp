#include "NeuralNetwork.h"
#include "MathExt.h"
namespace smpl
{
    #ifdef __SSE4_2__
    SIMD_SSE<float> simdLog(SIMD_SSE<float> v)
    {
        return SEML::log(v.values);
    }
    #endif
    
    #ifdef __AVX2__
    SIMD_AVX<float> simdLog(SIMD_AVX<float> v)
    {
        return SEML::log(v.values);
    }
    #endif

    SIMD_RAW<float> simdLog(SIMD_RAW<float> v)
    {
        return log(v.values);
    }
    
    MatrixF LeastSquares::evaluate(MatrixF expected, MatrixF actual)
    {
        //(y-yHat)^2 / size
        MatrixF diff = expected - actual;
        MatrixF output = (diff.getTranspose() * diff) / expected.getRows();
        return output;
    }
    MatrixF LeastSquares::derivative(MatrixF expected, MatrixF actual)
    {
        return -2 * (expected - actual);
    }

    MatrixF LogLoss::evaluate(MatrixF expected, MatrixF actual)
    {
        // TODO: FIX THIS
        MatrixF A, B;

        A = expected.getTranspose()*actual.broadcastAdd(EPSILON).broadcastFunction(log, simdLog);
        B = expected.broadcastInverseSubtract(1).getTranspose() * actual.broadcastInverseSubtract(1).broadcastAdd(EPSILON).broadcastFunction(log, simdLog);
        
        return -(A+B);
        return MatrixF();
    }
    MatrixF LogLoss::derivative(MatrixF expected, MatrixF actual)
    {
        MatrixF diff = expected - actual;
        MatrixF B = actual.hadamardProduct(actual.broadcastInverseSubtract(1)).broadcastAdd(EPSILON);
        return -diff.inverseHadamardProduct(B);
    }
}