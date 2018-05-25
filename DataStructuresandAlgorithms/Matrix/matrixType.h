/*	Amir Daud
	Assignment 1
*/

#include <iostream>
using namespace std;

const int MAX_ROWS = 10;
const int MAX_COLS = 10;
class MatrixType
{
    public:
        MatrixType();
        void MakeEmpty();
        void SetSize(int rowsSize, int colSize);
        int GetCols();
        int GetRows();
        int GetItem(int rowNum, int colNum);       
        void StoreItem(int item, int row, int col);
        void Add(MatrixType otherOperand, MatrixType& result);
        void Sub(MatrixType otherOperand, MatrixType& result);
        void Mult(MatrixType otherOperand, MatrixType& result);
        void Print(ofstream& outfile);
        bool AddSubCompatible(MatrixType otherOperand);
        bool MultCompatible(MatrixType otherOperand);
    private:
        int values[MAX_ROWS][MAX_COLS];
        int numRows;
        int numCols;
};

