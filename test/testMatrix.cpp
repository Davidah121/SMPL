#include <catch2/catch_amalgamated.hpp>
#include "Matrix.h"
#include "MathExt.h"

TEST_CASE("Testing Matrix Class", "[Matrix]")
{
    SECTION("Test Square Multiplication")
    {
        smpl::MatrixF A = smpl::MatrixF(8, 8); //64 values
        smpl::MatrixF B = smpl::MatrixF(8, 8); //64 values

        //determine if C has the correct values
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                A[i][j] = 1;
                B[i][j] = 1;
            }
        }
        
        smpl::MatrixF C = A*B;
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
        smpl::MatrixF A = smpl::MatrixF(8, 4); //64 values
        smpl::MatrixF B = smpl::MatrixF(4, 7); //64 values

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
                B[i][j] = j+1+i;
            }
        }
        
        smpl::MatrixF C = A*B;
        smpl::MatrixF Expected = smpl::MatrixF(8, 7);
        Expected[0][0] = 20; Expected[0][1] = 26;  Expected[0][2] = 32;  Expected[0][3] = 38;  Expected[0][4] = 44;  Expected[0][5] = 50;  Expected[0][6] = 56;
        Expected[1][0] = 30; Expected[1][1] = 40;  Expected[1][2] = 50;  Expected[1][3] = 60;  Expected[1][4] = 70;  Expected[1][5] = 80;  Expected[1][6] = 90;
        Expected[2][0] = 40; Expected[2][1] = 54;  Expected[2][2] = 68;  Expected[2][3] = 82;  Expected[2][4] = 96;  Expected[2][5] = 110; Expected[2][6] = 124;
        Expected[3][0] = 50; Expected[3][1] = 68;  Expected[3][2] = 86;  Expected[3][3] = 104; Expected[3][4] = 122; Expected[3][5] = 140; Expected[3][6] = 158;
        Expected[4][0] = 60; Expected[4][1] = 82;  Expected[4][2] = 104; Expected[4][3] = 126; Expected[4][4] = 148; Expected[4][5] = 170; Expected[4][6] = 192;
        Expected[5][0] = 70; Expected[5][1] = 96;  Expected[5][2] = 122; Expected[5][3] = 148; Expected[5][4] = 174; Expected[5][5] = 200; Expected[5][6] = 226;
        Expected[6][0] = 80; Expected[6][1] = 110; Expected[6][2] = 140; Expected[6][3] = 170; Expected[6][4] = 200; Expected[6][5] = 230; Expected[6][6] = 260;
        Expected[7][0] = 90; Expected[7][1] = 124; Expected[7][2] = 158; Expected[7][3] = 192; Expected[7][4] = 226; Expected[7][5] = 260; Expected[7][6] = 294;
        
        REQUIRE(C.getRows() == 8);
        REQUIRE(C.getCols() == 7);
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<7; j++)
            {
                CHECK(C[i][j] == Expected[i][j]);
            }
        }
    }

    SECTION("Test Hadamard Product")
    {
        smpl::MatrixF A = smpl::MatrixF(8, 8); //64 values
        smpl::MatrixF B = smpl::MatrixF(8, 8); //64 values

        //determine if C has the correct values
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                A[i][j] = j;
                B[i][j] = j;
            }
        }
        
        smpl::MatrixF C = A.hadamardProduct(B);
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
        smpl::MatrixF A = smpl::MatrixF(8, 8); //64 values
        smpl::MatrixF B = smpl::MatrixF(8, 8); //64 values

        //determine if C has the correct values
        for(int i=0; i<8; i++)
        {
            for(int j=0; j<8; j++)
            {
                A[i][j] = j;
                B[i][j] = i;
            }
        }
        
        smpl::MatrixF C = A.multiplyTranspose(B);
        smpl::MatrixF C2 = A*B.getTranspose();
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
        smpl::MatrixF A = smpl::MatrixF(6, 6);
        A[0][0] = 0; A[0][1] = 0; A[0][2] = 1; A[0][3] = 5; A[0][4] = 2; A[0][5] = 5;
        A[1][0] = 4; A[1][1] = 1; A[1][2] = 5; A[1][3] = 2; A[1][4] = 6; A[1][5] = 2;
        A[2][0] = 1; A[2][1] = 4; A[2][2] = 5; A[2][3] = 8; A[2][4] = 9; A[2][5] = 8;
        A[3][0] = 8; A[3][1] = 2; A[3][2] = 7; A[3][3] = 6; A[3][4] = 9; A[3][5] = 2;
        A[4][0] = 1; A[4][1] = 3; A[4][2] = 2; A[4][3] = 9; A[4][4] = 3; A[4][5] = 4;
        A[5][0] = 5; A[5][1] = 9; A[5][2] = 3; A[5][3] = 5; A[5][4] = 3; A[5][5] = 2;

        //A*A^-1 = A^-1 * A = I
        smpl::MatrixF aInverse = A.getInverse();
        REQUIRE(aInverse.getValid() == true);

        smpl::MatrixF aInvMultA = aInverse * A;
        smpl::MatrixF aMultAInv = A * aInverse;
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