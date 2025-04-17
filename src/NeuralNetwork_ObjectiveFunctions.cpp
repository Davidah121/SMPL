#include "NeuralNetwork.h"
#include "MathExt.h"
namespace smpl
{
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
        // MatrixF A, B;
        // #if (OPTI>=1)
        //     #define SIMD_LOG SEML::log
        // #else
        //     #define SIMD_LOG nullptr
        // #endif

        // A = expected.getTranspose()*actual.broadcastAdd(EPSILON).broadcastFunction(log, SIMD_LOG);
        // B = expected.broadcastInverseSubtract(1).getTranspose() * actual.broadcastInverseSubtract(1).broadcastAdd(EPSILON).broadcastFunction(log, SIMD_LOG);
        
        // #undef SIMD_LOG
        // return -(A+B);
        return MatrixF();
    }
    MatrixF LogLoss::derivative(MatrixF expected, MatrixF actual)
    {
        MatrixF diff = expected - actual;
        MatrixF B = actual.hadamardProduct(actual.broadcastInverseSubtract(1)).broadcastAdd(EPSILON);
        return -diff.inverseHadamardProduct(B);
    }
}