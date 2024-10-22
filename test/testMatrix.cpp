#include <catch2/catch_amalgamated.hpp>
#include "Matrix.h"
#include "MathExt.h"

TEST_CASE("Testing Matrix Class", "[Matrix]")
{
    SECTION("Test Square Multiplication")
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

    SECTION("Test Rectangular Multiplication")
    {
        smpl::Matrix A = smpl::Matrix(8, 4); //64 values
        smpl::Matrix B = smpl::Matrix(4, 7); //64 values

        //determine if C has the correct values
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<4; j++)
            {
                A[i][j] = j+i;
            }
        }
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<7; j++)
            {
                A[i][j] = j+1+i;
            }
        }
        
        smpl::Matrix C = A*B;
        smpl::Matrix Expected = smpl::Matrix(8, 7);
        Expected[0][0] = 20; Expected[0][1] = 26; Expected[0][2] = 32; Expected[0][3] = 38; Expected[0][4] = 44; Expected[0][5] = 50; Expected[0][6] = 56;
        Expected[1][0] = 30; Expected[1][1] = 40; Expected[1][2] = 50; Expected[1][3] = 60; Expected[1][4] = 70; Expected[1][5] = 80; Expected[1][6] = 90;
        Expected[2][0] = 70; Expected[2][1] = 96; Expected[2][2] = 122; Expected[2][3] = 148; Expected[2][4] = 174; Expected[2][5] = 200; Expected[2][6] = 226;
        Expected[3][0] = 110; Expected[3][1] = 152; Expected[3][2] = 194; Expected[3][3] = 236; Expected[3][4] = 278; Expected[3][5] = 320; Expected[3][6] = 362;
        Expected[4][0] = 150; Expected[4][1] = 208; Expected[4][2] = 266; Expected[4][3] = 324; Expected[4][4] = 382; Expected[4][5] = 440; Expected[4][6] = 498;
        Expected[5][0] = 190; Expected[5][1] = 264; Expected[5][2] = 338; Expected[5][3] = 412; Expected[5][4] = 486; Expected[5][5] = 560; Expected[5][6] = 634;
        Expected[6][0] = 230; Expected[6][1] = 320; Expected[6][2] = 410; Expected[6][3] = 500; Expected[6][4] = 590; Expected[6][5] = 680; Expected[6][6] = 770;
        Expected[7][0] = 270; Expected[7][1] = 376; Expected[7][2] = 482; Expected[7][3] = 588; Expected[7][4] = 694; Expected[7][5] = 800; Expected[7][6] = 906;
        
        REQUIRE(C.getRows() == 8);
        REQUIRE(C.getCols() == 7);
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<7; j++)
            {
                REQUIRE(C[i][j] == Expected[i][j]);
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