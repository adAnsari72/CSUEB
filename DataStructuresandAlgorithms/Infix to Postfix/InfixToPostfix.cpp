/* InfixToPostfix.cpp : Defines the entry point for the console application.
Amir Daud
11/7/2016
*/

//#include "stdafx.h"
#include <iostream>
#include <fstream>
using namespace std;

typedef char ItemType;

struct NodeType
{
	ItemType value;
	NodeType* next;
};


template<class ItemType>
class StackType
{
public:
	StackType();
	~StackType();
	bool IsEmpty() const;
	void Push(ItemType item);
	void Pop();
	ItemType Top();
private:
	NodeType* topPtr;
};


/*
Constructor method
post: Sets the topPtr to NULL
*/

template<class ItemType>
StackType<ItemType>::StackType()
{
	topPtr = NULL;
}

/*
Destructor Method
Post: parses the stack and deletes the links
*/
template<class ItemType>
StackType<ItemType>::~StackType()
{
	NodeType *temp;
	
	while (topPtr != NULL)
	{
		temp = topPtr;
		topPtr = topPtr->next;
		delete temp;
	}
}

/*
IsEmpty Method
Post: Returns true if the stack is empty
*/
template<class ItemType>
bool StackType<ItemType>::IsEmpty() const
{
	return (topPtr == NULL);
}

/*
Push Method
post: A new node is created, and is inserted onto the top of the stack
*/
template<class ItemType>
void StackType<ItemType>::Push(ItemType Item)
{
	NodeType* newItem = new NodeType;
	newItem->value = Item;
	newItem->next = topPtr;
	topPtr = newItem;
}

/*
Pop Method
Pre: The stack is created, and should not be empty. Method checks as well.
Post: Removes the top of the stack
*/

template<class ItemType>
void StackType<ItemType>::Pop()
{
	if (IsEmpty())
		cerr << "\n\n*****Empty Stack*****\n\n";
	else
	{
		NodeType* temp;
		temp = topPtr;
		topPtr = topPtr->next;
		delete temp;
	}
}

/*
Top Method
pre: The stack has been created and is not empty
post: Returns the value that is on the top of the stack
*/
template<class ItemType>
ItemType StackType<ItemType>::Top()
{
	if (IsEmpty())
		cerr << "\n\n*****Empty Stack*****\n\n";
	else
		return topPtr->value;
}

bool opCompare(ItemType, ItemType);
bool isOperator(ItemType);

int main()
{
	ItemType symbol;
	StackType<ItemType> stack;
	string postFix = "";

	ifstream fin;
	ofstream fout;

	fin.open("Input.txt");
	fout.open("Output.txt");

	while(!fin.eof())
	{
		fin.get(symbol);
		while (symbol != '\n')
		{
			if (!isOperator(symbol))
			{
				postFix += symbol;
			}
			else if (symbol == '(')
			{
				stack.Push(symbol);
			}
			else if (symbol == ')')
			{
				if (!stack.IsEmpty())
				{
					while (stack.Top() != '(' && !stack.IsEmpty())
					{
						postFix += stack.Top();
						stack.Pop();
					}
					if(!stack.IsEmpty())
						stack.Pop();
				}
				else
					cerr << "\n\n***Input Error: Starting with a ')' is invalid***\n\n";
			}
			else
			{
				while (!stack.IsEmpty() && opCompare(stack.Top(), symbol) && stack.Top() != '(')
				{
					postFix += stack.Top();
					stack.Pop();
				}
				
				stack.Push(symbol);
			}

			fin.get(symbol);
		}

		while (!stack.IsEmpty())
		{
			postFix += stack.Top();
			stack.Pop();
		}
		
		for (int i = 0; i < postFix.length(); i++)
			fout << postFix[i];
		fout << endl;
		postFix.clear();
	}


	fin.close();
	fout.close();
}

/*
opCompare Function
Pre: calling function sends 2 operators
Post: Using an Array determines the precedence of the sent operators
*/
bool opCompare(ItemType op1, ItemType op2)
{
	char operators[5] = {'*', '/', '%', '+', '-'};  //this array determines the value of the operators
	int op1Val = 0, op2Val = 0;

	for (int i = 0; i < 5; i++)
	{
		if (operators[i] == op1)
			op1Val = i;

		if (operators[i] == op2)
			op2Val = i;
	}

	//These two conditionals equalize those operators with equivalent precedence.
	if (op1Val <= 2)
		op1Val = 0;
	else
		op1Val = 1;

	if (op2Val <= 2)
		op2Val = 0;
	else
		op2Val = 1;

	if (op1Val <= op2Val)
		return true; //op1 is of equal or higher precedence than op2
	else
		return false; //op1 is of lesser precedence than op2
}

/*
isOperator Function
pre: Calling function sends an Item
post: The function uses an array to determine whether the sent item is an operator or not
*/

bool isOperator(ItemType item)
{
	const char operators[7] = { '(',')', '*', '/', '%', '+', '-' }; 

	for (int i = 0; i < 7; i++)
	{
		if (item == operators[i])
			return true;
	}

	return false;
}