// Amir Daud
// Compiler Design 4110
// Symbol Table 1/11/2017

#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <fstream>

using namespace std;

//global used for the size of the symbol table as well 
//as used to find the hash key of an identifier
//I may increase the size of the table as I iterate the code to ensure a better table
//I am using a small size here to help with debugging and output visibility
const int SymbolTableSize = 5;


//The Nodes of the Symbol table are defined
//The Name of the Identifier is represented as idName and 
//the block of code that it is active in is stored as blockNum
//This is to determine it's scope and visibility.
struct idNode
{
	string   idName;
	int      blockNum;
	idNode* next;
};


//This will be used to create the Active Block Stack. 
//It is used to keep track of the scope of identifiers.
struct activeStack
{
	int number;
	activeStack* next;
};

class SymbolTable
{
public:
	SymbolTable();
	void insert(string id);
	idNode searchLocal(string id, bool& found);
	idNode searchAll(string id, bool& found);
	void display();
	void incrementBlockStack();
	void decrementBlockStack();
	int getCurrentBlock();		//used to make output more visible and clear
private:
	idNode* symbolTable[SymbolTableSize];
	activeStack* activeBlockStack;
	int currentBlock;
	int calcHashKey(string id);
	int tableDepth[SymbolTableSize];  //Used for the display method

};

SymbolTable::SymbolTable()
{
	for (int i = 0; i < SymbolTableSize; i++)
	{
		tableDepth[i] = 0;
		symbolTable[i] = new idNode;
		symbolTable[i]->next = NULL;
	}
	
	currentBlock = -1;
	activeBlockStack = new activeStack;
	activeBlockStack->next = NULL;
	activeBlockStack->number = currentBlock;
}

//A very simplified hash key calculate function. 
//It takes the name of the ID and takes the sum of the numerical values
//of each letter in the ID and mods it with the size of the symbol table.
//It is a private function because it is not necessary for the user to have access to this function
int SymbolTable::calcHashKey(string id)
{
	int size = 0;
	for (int i = 0; i < id.length(); i++)
		size += (int)id[i];

	size = size%SymbolTableSize;

	return size;
}

//When an id is inserted into the hash table it first must get a hash key
//after that it is inserted into the hash chain
void SymbolTable::insert(string id)
{
	int key = calcHashKey(id);

	idNode* temp = new idNode;
	temp->blockNum = activeBlockStack->number;
	temp->idName = id;

	temp->next = symbolTable[key];
	symbolTable[key] = temp;
	
	tableDepth[key]++;
}

//When a '{', or whatever character is designated as a new block, is read the 
//block number is incremented, and pushed onto the stack
void SymbolTable::incrementBlockStack()
{
	currentBlock++;
	activeStack* temp = new activeStack;
	temp->number = currentBlock;
	temp->next = activeBlockStack;
	activeBlockStack = temp;
}

//when a '}',  or whatever character is designated as the end of a block, 
//is read the top of the block stack is popped off
//the currentBlock variable is NOT decremented. This is to allow easier track of how many 
//different scopes we have entered
void SymbolTable::decrementBlockStack()
{
	activeStack* temp = activeBlockStack;
	activeBlockStack = activeBlockStack->next;
	delete temp;
}

//using 3 for loops:
//the first iterates though the array portion of the hash table
//the second iterates through the list off of each array element
//the third outputs the idName
//The output is in the form [ID, Block Number]
//at the end of the function the active block stack is output
//so that it is clear which scopes are visible
void SymbolTable::display()
{
	idNode* curr;
	for (int i = 0; i < SymbolTableSize; i++)
	{
		curr = symbolTable[i];
		cout << "\n Hash table chain " << i << ": ";
		for (int j = 0; j < tableDepth[i]; j++)
		{
			cout << " [";

			for (int k = 0; k < curr->idName.length(); k++)
				cout << curr->idName[k];

			cout << ", " << curr->blockNum << "] ";
			curr = curr->next;
		}

	}

	activeStack* trace = activeBlockStack;
	cout << "\n\nActive Block Stack: ";
	while (trace && trace->number != -1)
	{
		cout << "[ " << trace->number << " ] ";
		trace = trace->next;
	}

	cout << endl;

}


//This method will first find the key for the id that is sent to it
//After finding the key it will check the approriate chain of the hash table to see if the 
//Id is in the local scope, if not it will return an empty node with a massive block number and a false boolean.
idNode SymbolTable::searchLocal(string id, bool& found)
{
	int key = calcHashKey(id);
	idNode result;

	if (symbolTable[key]->idName == id && symbolTable[key]->blockNum == activeBlockStack->number)
	{
		found = true;
		result.idName = symbolTable[key]->idName;
		result.blockNum = symbolTable[key]->blockNum;
	}
	else
	{
		found = false;
		result.idName = "";
		result.blockNum = 10000;	//set to some extreme scope number for debugging
	}

	return result;
}


//The search all function looks for the id globally
//It does this by going to the appropriate chain of the hash table and going down the chain
//It compares the chains block numbers with the block number of the id. If it finds a match it returns the result
//Else it returns an empty string
idNode SymbolTable::searchAll(string id, bool& found)
{
	int key = calcHashKey(id);
	idNode result;
	idNode* curr = symbolTable[key];
	found = false;


	while (curr->next && !found)
	{
		//Ideally this first if should not occur, but it is here as a failsafe incase
		//this method is called at the wrong time, it will locate the local version
		if (curr->blockNum == activeBlockStack->number && curr->idName == id)
		{
			result.idName = curr->idName;
			result.blockNum = curr->blockNum;
			found = true;
		}
		else if (curr->idName == id && curr->blockNum != activeBlockStack->number)
		{
			activeStack*temp = activeBlockStack;

			while (temp)
			{
				if (temp->number == curr->blockNum)
				{
					result.idName = curr->idName;
					result.blockNum = curr->blockNum;
					found = true;
				}
				else
				{
					found = false;
					result.idName = "";
					result.blockNum = 10000;
				}
				temp = temp->next;
			}
			curr = curr->next;
		}
		else //if the id is not visible, then it will enter this else and return a false
		{ 
			found = false;
			result.idName = "";
			result.blockNum = 10000;
			curr = curr->next;
		}
	}
	return result;
}

int SymbolTable::getCurrentBlock()
{
	return activeBlockStack->number;
}

int main()
{
	SymbolTable test;
	idNode entry;
	bool found = false;
	ifstream fin;
	fin.open("input.txt");
	char input = ' ';
	string fullInput = "";

	while (!fin.eof())
	{

		fin.get(input);
		
		if (input == '{')
		{
			test.incrementBlockStack();
		}
		else if (input == '}')
		{
			test.decrementBlockStack();
		}
		else if (input != ' ' && input != '\n' && input != '\t')
		{
			fullInput += input;
		}
		else if(fullInput != "")
		{
			cout << "\nAttempting to input [";
			for (int i = 0; i < fullInput.length(); i++)
				cout << fullInput[i];
			cout << " , " << test.getCurrentBlock() << "]";
			
			test.searchLocal(fullInput, found);

			if (!found)
			{
				test.searchAll(fullInput, found);
				if (!found)
				{
					test.insert(fullInput);
					test.display();
				}
				else
				{
					cout << "\n***Variable Already Declared***\n";
				}
			}
			else
			{
				cout << "\n***Variable Already Declared***\n";
			}


			fullInput = "";
		}		

	}

	fin.close();
	cout << "\n\n At the end of main: \n";
	test.display();

	system("PAUSE");
    return 0;
}

