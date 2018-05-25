//Amir Daud
//Dictionary.h file

#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;


struct Node {
	// Your data structure goes here
	Node* letters[26];
	bool wordFlag[26];
};

class Dictionary
{
public:
	//Default constructor
	//pre: no text file is sent
	//post: creates a new empty node, in prep for creating a new dictionary
	Dictionary() {
		root = new Node;
		for (int i = 0; i < 26; i++){	
			root->letters[i] = NULL;
			root->wordFlag[i] = false;	}
		numWords = 0;}

	Dictionary(string file);
	void addWord(string word);
	bool isWord(string word);
	bool isPrefix(string word);
	int wordCount();

private:
	Node* root;
	int numWords;
	// Any private methods you need/want
};
