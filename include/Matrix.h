#pragma once
#include "Object.h"
#include "GeneralVector.h"

class Matrix : public Object
{
public:
	Matrix();
	Matrix(int rows, int cols);
	Matrix(const Matrix& c);
	void operator=(const Matrix& o);

	~Matrix();

	void copy(const Matrix& o);

	//Object and Class Stuff
	const Class* getClass();
	static const Class* myClass;

	double* operator[](int row);

	Matrix operator*(double value);
	Matrix operator*(int value);
	Matrix operator*(float value);

	Matrix operator*(Matrix other);
	GeneralVector operator*(GeneralVector other);

	void operator*=(double value);
	void operator*=(int value);
	void operator*=(float value);

	Matrix operator+(Matrix other);
	void operator+=(Matrix other);

	Matrix operator-(Matrix other);
	void operator-=(Matrix other);

	bool operator==(Matrix other);
	bool operator!=(Matrix other);

	double** getData();

	int getRows();
	int getCols();
	bool getValid();

	double get(int col, int row);

protected:
	double** data = nullptr;
	int rows;
	int columns;

	bool valid = true;

};

