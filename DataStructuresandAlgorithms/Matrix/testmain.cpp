/*	Amir Daud
	Assignment 1 Matrix Type
*/

// testmain.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "matrixType.h"
using namespace std;

MatrixType::MatrixType()
{
	numRows = 0;
	numCols = 0;
	for (int i = 0; i < MAX_ROWS; i++)
		for (int j = 0;  j < MAX_COLS; j++)
			values[i][j] = 0;
}
/*
MakeEmpty fucntion:
Clears the Matrix of all Data
Pre: Matrix exists and is size numRows by numCols
Post: Matrix is zero'd out
*/
void MatrixType::MakeEmpty()
{

	for (int i = 0; i < numRows; i++)
		for (int j = 0; j < numCols; j++)
			values[i][j] = 0;

}

/*
SetSize Function:
Allows the user to set the variables, numRows and numCols, pertaining to the size of the matrix
*/
void MatrixType::SetSize(int rowsSize, int colSize)
{
	numRows = rowsSize;
	numCols = colSize;
}


/*
GetCols function:
Returns the number of columns of this class to the user.
*/
int MatrixType::GetCols()
{
	return numCols;
}

/*
GetRows function:
Returns the number of rows of this class to the user.
*/
int MatrixType::GetRows()
{
	return numRows;
}

int MatrixType::GetItem(int rowNum, int colNum)
{
	return values[rowNum][colNum];
}

/*
StoreItem function:
Allows the user to set the value of a particular matrix cell
Pre: Matrix has already been created
Post: Item has been placed into the Matrix at the proper coordinates
*/
void MatrixType::StoreItem(int item, int row, int col)
{
	values[row][col] = item;
}

/*
AddSubCompatible Function:
Pre: Matrix has already been created and populated with Data
Post: Returns a true or false depending on the compatibility of the given matrix and that
of the current classe's matrix
*/

bool MatrixType::AddSubCompatible(MatrixType otherOperand)
{
	if (numRows == otherOperand.GetRows())
	{
		if (numCols == otherOperand.GetCols())
			return true;
	}
	return false;
}

/*
MultCompatible Function:
Pre: Matrix has already been created and populated with Data
Post: Returns a true or false depending on the compatibility of the given matrix and that
of the current classes' matrix
*/
bool MatrixType::MultCompatible(MatrixType otherOperand)
{
	if (numCols == otherOperand.GetRows())
		return true;

	return false;
}

/*
Add Function:
pre: the Matrices have been checked for compatibility
post: the result is returned by reference
*/
void MatrixType::Add(MatrixType otherOperand, MatrixType& result)
{
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
		{
			result.StoreItem(values[i][j] + otherOperand.GetItem(i, j), i, j);
		}
	}
}

/*
Sub Function:
pre: the Matrices have been checked for compatibility
post: the result is returned by reference
*/
void MatrixType::Sub(MatrixType otherOperand, MatrixType& result)
{
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
			result.StoreItem(values[i][j] - otherOperand.GetItem(i, j), i, j);
	}
}

/*
Mult Function:
pre: The matrices have been checked to see if they are compatible, the result Matrix is the proper size
post: the result is returned by reference
*/
void MatrixType::Mult(MatrixType otherOperand, MatrixType& result)
{
	result.MakeEmpty();         //ensure that the result matrix is empty.
	int temp = 0;               //temporary integter used for storing data

	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < otherOperand.GetCols(); j++)
		{
			for (int k = 0; k < numCols; k++)
			{
				temp += values[i][k] * otherOperand.GetItem(k, j);
			}
			result.StoreItem(temp, i, j);
			temp = 0;
		}
	}
}

/*
Print Function:
pre:
post: Prints out the matrix to the file
*/

void MatrixType::Print(ofstream& outfile)
{
	outfile << "\nMatrix of size: " << numRows << ", " << numCols << ": " << endl;
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
		{
			outfile << values[i][j] << " ";
		}
		outfile << endl;
	}

}

void outputMenu(ofstream&);
void inputMatrix(MatrixType& Matrix);
void outputMatrix(MatrixType Matrix);


int main()
{
	MatrixType Matrices[10];
	int choices[4] = { 0 }, item = 0;					//User input storage
	bool  done = false;									//used for the main while loop. 
	ofstream fout;							
	fout.open("DataOut.txt");

	
	while (!done)
	{
		outputMenu(fout);
		cin >> item;
		choices[0] = item;

		if (item == 0 || item == 4)
		{
			cin >> item;
			choices[1] = item;
		}
		else if (item != 5 && item < 5)
		{
			for (int i = 1; i < 4; i++)
			{
				cin >> item;
				choices[i] = item;
			}
		}
		

		switch (choices[0])
		{
		case 0:
			cout << "Creating New Matrix in slot " << choices[1] << endl;
			fout << "Creating New Matrix in slot " << choices[1] << endl;
			inputMatrix(Matrices[choices[1]]);
			break;
		case 1:
			if (Matrices[choices[1]].AddSubCompatible(Matrices[choices[2]]))
			{
				cout << "Adding slots " << choices[1] << " and " << choices[2] << " solution in " << choices[3] << endl;
				fout << "Adding slots " << choices[1] << " and " << choices[2] << " solution in " << choices[3] << endl;
				Matrices[choices[3]].SetSize(Matrices[choices[1]].GetRows(), Matrices[choices[1]].GetCols());
				Matrices[choices[1]].Add(Matrices[choices[2]], Matrices[choices[3]]);
				cout << endl;
				fout << endl;
			}
			else
				cout << "\n\n***Matrices incompatible. Please select a different pair of Matrices.***\n\n";
				fout << "\n\n***Matrices incompatible. Please select a different pair of Matrices.***\n\n";
			break;
		case 2:
			if (Matrices[choices[1]].AddSubCompatible(Matrices[choices[2]]))
			{
				cout << "Subtracting slots " << choices[1] << " and " << choices[2] << " solution in " << choices[3] << endl;
				fout << "Subtracting slots " << choices[1] << " and " << choices[2] << " solution in " << choices[3] << endl;
				Matrices[choices[3]].SetSize(Matrices[choices[1]].GetRows(), Matrices[choices[1]].GetCols());
				Matrices[choices[1]].Sub(Matrices[choices[2]], Matrices[choices[3]]);
				cout << endl;
				fout << endl;
			}
			else
				cout << "\n\n***Matrices incompatible. Please select a different pair of Matrices.***\n\n";
				fout << "\n\n***Matrices incompatible. Please select a different pair of Matrices.***\n\n";
			break;
		case 3:
			if (Matrices[choices[1]].MultCompatible(Matrices[choices[2]]))
			{
				cout << "Multiplying slots " << choices[1] << " and " << choices[2] << " solution in " << choices[3] << endl;
				fout << "Multiplying slots " << choices[1] << " and " << choices[2] << " solution in " << choices[3] << endl;
				Matrices[choices[3]].SetSize(Matrices[choices[1]].GetRows(), Matrices[choices[2]].GetCols());
				Matrices[choices[1]].Mult(Matrices[choices[2]], Matrices[choices[3]]);
				cout << endl;
				fout << endl;
			}
			else
				cout << "\n\n***Matrices incompatible. Please select a different pair of Matrices.***\n\n";
				fout << "\n\n***Matrices incompatible. Please select a different pair of Matrices.***\n\n";
			break;
		case 4:
			outputMatrix(Matrices[choices[1]]);
			Matrices[choices[1]].Print(fout);
			break;
		case 5:
			cout << "\nExiting.\n";
			fout << "\nExiting.\n";
			done = true;
			break;
		default:
			cout << "\n\n***Input not Valid. Please input a valid option.***\n\n";
			fout << "\n\n***Input not Valid. Please input a valid option.***\n\n";
			break;
		};
		item = 0;
		for (int i = 0; i < 4; i++)
			choices[i] = 0;
	}

			
	fout.close();
	//system("PAUSE");
    return 0;
}


void outputMenu(ofstream& fout)
{
	cout << "\nChoice and Parameters(0-9):" << endl;
	cout << "0 New Matrix            Usage 0: <Matrix>\n";
	cout << "1 Add Matrices          Usage 1: <Matrix> <Matrix> <Matrix>\n";
	cout << "2 Subtract Matrices     Usage 2: <Matrix> <Matrix> <Matrix>\n";
	cout << "3 Multiply Matrices     Usage 3: <Matrix> <Matrix> <Matrix>\n";
	cout << "4 Print Matrices        Usage 4: <Matrix>\n";
	cout << "5 Quit                  Usage 5:\n";
	cout << "\n\nEnter choice and Parameters: ";
	
	fout << "\nChoice and Parameters(0-9):" << endl;
	fout << "0 New Matrix            Usage 0: <Matrix>\n";
	fout << "1 Add Matrices          Usage 1: <Matrix> <Matrix> <Matrix>\n";
	fout << "2 Subtract Matrices     Usage 2: <Matrix> <Matrix> <Matrix>\n";
	fout << "3 Multiply Matrices     Usage 3: <Matrix> <Matrix> <Matrix>\n";
	fout << "4 Print Matrices        Usage 4: <Matrix>\n";
	fout << "5 Quit                  Usage 5:\n";
	fout << "\n\nEnter choice and Parameters: ";
	
}

void inputMatrix(MatrixType& Matrix)
{
	int row = 0, col = 0, item = 0;

	cout << "Enter row and Column Size: ";
	cin >> row >> col;
	Matrix.SetSize(row, col);
	
	for (int i = 0; i < row; i++)
	{
		cout << "\nRow " << i << ": ";
		for (int j = 0; j < col; j++)
		{
			cin >> item;
			Matrix.StoreItem(item, i, j);
		}
	}
	cout << endl;
}

void outputMatrix(MatrixType Matrix)
{
	cout << "\nThe Matrix requested: \n";
	for (int i = 0; i < Matrix.GetRows(); i++)
	{
		cout << "Row " << i << ": ";
		for (int j = 0; j < Matrix.GetCols(); j++)
			cout << Matrix.GetItem(i, j) << " ";
		cout << endl;
	}
}