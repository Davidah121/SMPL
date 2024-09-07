#include <catch2/catch_amalgamated.hpp>
#include "Matrix.h"
#include "MathExt.h"

TEST_CASE("Testing Matrix Class", "[Matrix]")
{
    SECTION("Test Multiplication")
    {
        smpl::Matrix A = smpl::Matrix(8, 8); //64 values
        smpl::Matrix B = smpl::Matrix(8, 8); //64 values

        //determine if C has the correct values
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                A[i][j] = 1;
                B[i][j] = 1;
            }
        }
        
        smpl::Matrix C = A*B;
        REQUIRE(C.getRows() == 8);
        REQUIRE(C.getCols() == 8);
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                REQUIRE(C[i][j] == 8);
            }
        }
    }

    SECTION("Test Hadamard Product")
    {
        smpl::Matrix A = smpl::Matrix(8, 8); //64 values
        smpl::Matrix B = smpl::Matrix(8, 8); //64 values

        //determine if C has the correct values
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                A[i][j] = j;
                B[i][j] = j;
            }
        }
        
        smpl::Matrix C = A.hadamardProduct(B);
        REQUIRE(C.getRows() == 8);
        REQUIRE(C.getCols() == 8);
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                REQUIRE(C[i][j] == (j*j));
            }
        }
    }

    SECTION("Test Transpose Multiplication")
    {
        smpl::Matrix A = smpl::Matrix(8, 8); //64 values
        smpl::Matrix B = smpl::Matrix(8, 8); //64 values

        //determine if C has the correct values
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                A[i][j] = j;
                B[i][j] = i;
            }
        }
        
        smpl::Matrix C = A.multiplyTranspose(B);
        smpl::Matrix C2 = A*B.getTranspose();
        REQUIRE(C.getRows() == 8);
        REQUIRE(C.getCols() == 8);
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                REQUIRE(C[i][j] == C2[i][j]);
            }
        }
    }

    SECTION("Test Inverse")
    {
        smpl::Matrix A = smpl::Matrix(6, 6);
        A[0][0] = 0; A[0][1] = 0; A[0][2] = 1; A[0][3] = 5; A[0][4] = 2; A[0][5] = 5;
        A[1][0] = 4; A[1][1] = 1; A[1][2] = 5; A[1][3] = 2; A[1][4] = 6; A[1][5] = 2;
        A[2][0] = 1; A[2][1] = 4; A[2][2] = 5; A[2][3] = 8; A[2][4] = 9; A[2][5] = 8;
        A[3][0] = 8; A[3][1] = 2; A[3][2] = 7; A[3][3] = 6; A[3][4] = 9; A[3][5] = 2;
        A[4][0] = 1; A[4][1] = 3; A[4][2] = 2; A[4][3] = 9; A[4][4] = 3; A[4][5] = 4;
        A[5][0] = 5; A[5][1] = 9; A[5][2] = 3; A[5][3] = 5; A[5][4] = 3; A[5][5] = 2;

        //A*A^-1 = A^-1 * A = I
        smpl::Matrix aInverse = A.getInverse();
        REQUIRE(aInverse.getValid() == true);

        smpl::Matrix aInvMultA = aInverse * A;
        smpl::Matrix aMultAInv = A * aInverse;
        for(int i=0; i<6; i++)
        {
            for(int j=0; j<6; j++)
            {
                if(i==j)
                {
                    REQUIRE(aInvMultA[i][j] - 1 <= EPSILON);
                    REQUIRE(aMultAInv[i][j] - 1 <= EPSILON);
                }
                else
                {
                    REQUIRE(aInvMultA[i][j] <= EPSILON);
                    REQUIRE(aMultAInv[i][j] <= EPSILON);
                }
            }
        }
    }
}