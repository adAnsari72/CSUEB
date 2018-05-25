// boggle solver.cpp : Defines the entry point for the console application.
// Amir Daud
// Boggle Solver

#include "stdafx.h"
#include "Dictionary.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

//alternate constructor
//pre: a file name is sent in a string
//post: a tree is created using the file
Dictionary::Dictionary(string file)
{
	int count = 0;
	string word;
	ifstream fin;
	
	fin.open(file);
	numWords = 0;	
	root = new Node;

	for (int i = 0; i < 26; i++) 
	{
		root->letters[i] = NULL;
		root->wordFlag[i] = false;
	}
	

	while (getline(fin, word))
	{		
		//cout << word << endl;
		addWord(word);
	}

	fin.close();
}

//Addword
//pre: a word is sent to the function
//post: Adds a word to the dictionary, if the starting character has not been added yet creates a new node for starting
void Dictionary::addWord(string word)
{
	Node* curr = root;
	int index = 0;
	for (int i = 0; i < word.length(); i++)
	{
		index = (int)word[i] - (int)'a';
		//cout << word[i] << ' ' << index << endl;
		if (!curr->letters[index])
		{
			curr->letters[index] = new Node;
			for (int j = 0; j < 26; j++)
			{
				curr->letters[index]->letters[j] = NULL;
				curr->letters[index]->wordFlag[j] = false;
			}
		}
		curr = curr->letters[index];
	}
	if (!curr->wordFlag[index])
	{
		curr->wordFlag[index] = true;
		numWords++;
	}
}

//isWord
//pre: a string is sent to the function
//post: returns a boolean value based on whether the sent string is a word or not
bool Dictionary::isWord(string word)
{
	Node* curr = root;
	int index = 0;
	for (int i = 0; i < word.length(); i++)
	{
		index = (int)word[i] - (int)'a';
		if (!curr->letters[index])
			return false;
		else
			curr = curr->letters[index];
	}

	return curr->wordFlag[index];
}

//isPrefix
//pre: a string is sent to the function
//post: returns a boolean value based on whether the string is a valid beginning to a word
bool Dictionary::isPrefix(string word)
{
	Node* curr = root;
	int index = 0;
	for (int i = 0; i < word.length(); i++)
	{
		index = (int)word[i] - (int)'a';
		if (!curr->letters[index])
				return false;
		else
				curr = curr->letters[index];
	}

	return true;
}

//wordCount
//pre: dictionary has been declared, whether default or via the string
//post: returns the number of words this particular dictionary has found
int Dictionary::wordCount()
{
	return numWords;
}

void SolveBoard(string board[4][4], Dictionary& dict, Dictionary& wordsFound, bool printBoard);
void outputBoard(string word, string Board[4][4], int numBoard[4][4], int counter);
void searchForWord(string board[4][4], int row, int col, Dictionary& dict, Dictionary& wordsFound, string curr, bool visited[4][4], int path[4][4], int step, bool printBoard);

int main()
{
	string board[4][4];
	bool done = false, detailedOutput = true, userEntry = false;
	char cont = 'y', init = 'y', otflag = 'y';

	srand(time(NULL));

	Dictionary dict("dictionary.txt");
	cout << "Words Loaded: " << dict.wordCount() << endl;

	while (!done) //Loop for overall program, quits when the user wants to
	{
		//if yes the user enters own board, else a random board is generated
		cout << "\n\nWould you like to enter your own board?: (y/n) ";
		cin >> init;	

		if (init == 'y' || init == 'Y')
		{
			char item;

			for (int i = 0; i < 4; i++)
			{
				cout << "\nRow " << i << ": ";
				for (int j = 0; j < 4; j++)
				{
					cin >> item;
					board[i][j] = item;
				}
			}
			cout << endl;
		}
		else
		{
			cout << "\n\nGenerating board:\n";
			for (int i = 0; i < 4; i++)
			{
				cout << "\nRow: ";
				for (int j = 0; j < 4; j++)
				{
					int n = rand() % 26;
					board[i][j] = (char)(n + 97);
					cout << board[i][j] << " ";					
				}
				cout << endl;
			}
		}

		//prompts user whether they want a detailed print
		cout << "\n\nWould you like a detailed printout? (y/n)";
		cin >> otflag;

		if (otflag == 'y' || otflag == 'Y')
			detailedOutput = true;
		else
			detailedOutput = false;

		Dictionary wordsFound;
		SolveBoard(board, dict, wordsFound, detailedOutput);

		//prompts the user whether they want to continue
		cout << "\n\n\nDo you wish to continue? (y/n)";
		cin >> cont;
		if (cont == 'y' || cont == 'Y')
			done = false;
		else
			done = true;

	}

	system("PAUSE");
	return 0;
}

//SolverBoard
//pre: the board, dict and printboard variables have been created and input by the user/generated. And an empty dictionary has been created
//post: The board has been solved via the searchForWord recursion
void SolveBoard(string board[4][4], Dictionary& dict, Dictionary& wordsFound, bool printBoard) {

	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			bool visited[4][4];
			int numBoard[4][4];
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					visited[i][j] = false;
					numBoard[i][j] = 0;
				}
			}
			searchForWord(board, r, c, dict, wordsFound, "", visited, numBoard, 1, printBoard);
		}
	}
}

//searchForWord
//pre: board, row, col, dict, visited, numBoard, curr and step have all been initialized and sent to the function
//post: using the given parameters, either adds the word to the wordsFound object, OR calls itself eight times for each surrounding location on the board
void searchForWord(string board[4][4], int row, int col, Dictionary& dict, Dictionary& wordsFound, string curr, bool visited[4][4], int numBoard[4][4], int step, bool printBoard)
{
	if (row >= 0 && row < 4 && col >= 0 && col < 4 && dict.isPrefix(curr) && !visited[row][col])
	{
		curr += board[row][col];
		visited[row][col] = true;
		numBoard[row][col] = step;

		if (dict.isWord(curr) && !wordsFound.isWord(curr))
		{
			wordsFound.addWord(curr);
			if (printBoard)
			{
				outputBoard(curr, board, numBoard, wordsFound.wordCount());
			}
			else
			{
				cout << wordsFound.wordCount() << ":" << std::string(5, ' ') << curr << endl;
			}
		}

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				bool visitedCopy[4][4];
				int numBoardCopy[4][4];
				for (int k = 0; k < 4; k++)
				{
					for (int l = 0; l < 4; l++)
					{
						visitedCopy[k][l] = visited[k][l];
						numBoardCopy[k][l] = numBoard[k][l];
					}
				}
				searchForWord(board, row + i, col + j, dict, wordsFound, curr, visitedCopy, numBoardCopy, step + 1, printBoard);
			}
		}
	}
}


//outputBoard
//pre: word, board, numBoard, and counter have been sent to the function
//post: outputs the word, the two boards and places the quotations around the path
void outputBoard(string word, string board[4][4], int numBoard[4][4], int counter)
{
	cout << endl << "Word: " << word << endl << "Word Number: " << counter << endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (numBoard[i][j])
				cout << "'" << board[i][j] << "'";
			else
				cout << " " << board[i][j] << " ";
		}

		cout << std::string(10, ' ');

		for (int k = 0; k < 4; k++)
		{
			cout << " " << numBoard[i][k] << " ";
		}

		cout << endl;
	}
}