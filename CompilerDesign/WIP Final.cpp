// Amir Daud
// Compiler Design 4110
// Visual Studio 2015 C++
// Code Gen 2/28/2017
/*
The purpose of this code is to parse a text file. As it parses it will
generate MIPS assembly code.
*/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
using namespace std;

//global used for the size of the symbol table as well 
//as used to find the hash key of an identifier
//I may increase the size of the table as I iterate the code to ensure a better table
//I am using a small size here to help with debugging and output visibility
const int SymbolTableSize = 13;


//Global used for the size of the keyword hash
const int SIZE = 13;

int OFFSET = 0;				//offset for the stack
const int INCREMENTOS = 4;	//amount to increase offset by

struct token
{
	string lexeme;
	int tokenNumber;
	int lineNumber;
};

struct keywordNode
{
	string name;
	int tokenNum;
	keywordNode* next;
};

struct expRec
{
	string type;
	int offSet;
};


//The Nodes of the Symbol table are defined
//The Name of the Identifier is represented as idName and 
//the block of code that it is active in is stored as blockNum
//This is to determine it's scope and visibility.
struct idNode
{
	string   idName;
	string dataType;
	int blockNum;
	int offset;
	int	tokenNum;
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
	void insert(string id, int tokenNum, string dataType);
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
void SymbolTable::insert(string id, int tokenNum, string dataType)
{
	int key = calcHashKey(id);

	idNode* temp = new idNode;
	temp->blockNum = activeBlockStack->number;
	temp->idName = id;
	temp->tokenNum = tokenNum;
	temp->dataType = dataType;
	temp->offset = OFFSET;
	OFFSET -= INCREMENTOS;

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

			cout << ", " << curr->blockNum << ", " << curr->offset << ", " << curr->tokenNum <<  "] ";
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
		result.offset = symbolTable[key]->offset;
		result.tokenNum = symbolTable[key]->tokenNum;
		result.dataType = symbolTable[key]->dataType;
	}
	else
	{
		found = false;
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
			result.offset = curr->offset;
			result.tokenNum = curr->tokenNum;
			result.dataType = curr->dataType;
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
					result.offset = curr->offset;
					result.tokenNum = curr->tokenNum;
					result.dataType = curr->dataType;
					found = true;
				}
				else
				{
					found = false;
				}
				temp = temp->next;
			}
			curr = curr->next;
		}
		else //if the id is not visible, then it will enter this else and return a false
		{
			found = false;
			curr = curr->next;
		}
	}
	return result;
}

int SymbolTable::getCurrentBlock()
{
	return activeBlockStack->number;
}


/*
The purpose of this class is to maintain a hash table of keywords
And allow for a quick search to find the approriate token number for a keyword
*/
class KeywordHash
{
public:
	KeywordHash(); //This will construct the hash table upon declaration of a class variable
	void checkKeyword(token&, bool&); //When given a token, this finds the appropriate keyword and gives the token number
	void outputHash(); //merely for testing to see if the table is being created prorperly
private:
	int generateKey(string);//generates a key for the string to allow for proper indexing
	void addWord(string, int);//adds a keyword and it's token number to the hash table
	keywordNode* keywords[SIZE];//the hash table's array of head pointers
};

/*
Token numbers:
1	identifier
2	any literal (integer such as 123,string "abc", FALSE, TRUE)
3   types (keywords 'INTEGER' 'STRING' 'LOGICAL')
4   addition operators (+  -  OR)
5   multiplication operators (* / DIV  REM AND)
6	relational operators = , !=, <, >
7	BEGIN		8   END		(not a token) COMMENT
9	IF			10  THEN	(*no ELSE in the language*)
11  WHILE		12  DO
3   INTEGER		3   STRING		3   LOGICAL
2   TRUE		2   FALSE		5	DIV				5  REM
4   OR			5   AND
13   READ		13  WRITE		13  WRITELN   (IO_operations)
14  (			15  )			16 ;  (not part of a comment)
17  ! Boolean not				18  .  (only at the end of the program)
19	:=
*/
KeywordHash::KeywordHash()
{
	for (int i = 0; i < SIZE; i++)
		keywords[i] = NULL;

	addWord("TRUE", 2);
	addWord("FALSE", 2);
	addWord("INTEGER", 3);
	addWord("STRING", 3);
	addWord("LOGICAL", 3);
	addWord("DIV", 5);
	addWord("REM", 5);
	addWord("OR", 4);
	addWord("AND", 5);
	addWord("BEGIN", 7);
	addWord("END", 8);
	addWord("IF", 9);
	addWord("THEN", 10);
	addWord("WHILE", 11);
	addWord("DO", 12);
	addWord("READ", 13);
	addWord("WRITE", 13);
	addWord("WRITELN", 13);
	addWord("COMMENT", -5);		//COMMENT is not a token, however this allows me to ignore it easily
}

int KeywordHash::generateKey(string input)
{
	int key = 0;
	for (int i = 0; i < input.length(); i++)
		key += (int)input[i];

	key = key%SIZE;
	return key;
}

void KeywordHash::addWord(string lexeme, int tokenNum)
{
	int key = generateKey(lexeme);
	keywordNode *temp = new keywordNode;
	temp->name = lexeme;
	temp->tokenNum = tokenNum;

	if (keywords[key] == NULL)
	{
		keywords[key] = temp;
		temp->next = NULL;
	}
	else
	{
		temp->next = keywords[key];
		keywords[key] = temp;
	}
}

void KeywordHash::outputHash()
{
	keywordNode *temp;
	for (int i = 0; i < SIZE; i++)
	{
		temp = keywords[i];
		cout << "Element " << i << ": ";

		while (temp)
		{
			cout << "Lex: ";
			for (int j = 0; j < temp->name.size(); j++)
				cout << temp->name[j];
			cout << " TokNum = " << temp->tokenNum << " ";
			temp = temp->next;
		}
		cout << endl;
	}
}

void KeywordHash::checkKeyword(token& input, bool &found)
{
	int key = generateKey(input.lexeme);
	keywordNode *temp = keywords[key];
	found = false;

	while (temp && !found)
	{
		if (input.lexeme.compare(temp->name) == 0)
		{
			input.tokenNumber = temp->tokenNum;
			found = true;
		}
		else
			temp = temp->next;
	}

}


/*
All functions related to reading the file come through this class
it will open a file when given the string name.
*/
class BufferClass
{
public:
	BufferClass();
	void openFile(string);//Opens the file
	char getChar();	//returns the current character in the string, increments position
	char getCharForString(); //Specifically for strings, does not ignore white space
	char peek();//returns the next character of the string
	void outputLine();//for testing purposes to see if it is reading the line properly
	bool getNewLine();//eads a new line from the file, returns true if it was able to read a new line
	void closeFile();//closes the file at the end of it's use
	int getLineNumber();//returns the current line number
private:
	ifstream fin;
	string currentLine;
	int position;
	int lineNumber;
};

BufferClass::BufferClass()
{
	currentLine = " ";
	position = 0;
	int lineNumber = 0;
}

void BufferClass::openFile(string fileName)
{
	fin.open(fileName);
}

void BufferClass::closeFile()
{
	fin.close();
}

bool BufferClass::getNewLine()
{
	if (!fin.eof())
	{
		getline(fin, currentLine);
		position = 0;
		lineNumber++;
		return true;
	}
	else
		return false;
}

char BufferClass::getChar()
{
	if (position == currentLine.length())
	{
		getNewLine();
		return ' ';
	}
	else
	{
		char output = currentLine[position];
		while (output == ' ' && position != currentLine.length())
		{
			position++;
			output = currentLine[position];
		}
		if (output == '\0')
		{
			output = getChar();
			return output;
		}
		else
		{
			position++;
			return output;
		}
	}
}

char BufferClass::getCharForString()
{
	if (position == currentLine.length())
	{
		getNewLine();
		return ' ';
	}
	else
	{
		char output = currentLine[position];
		position++;
		return output;
	}
}

char BufferClass::peek()
{
	if (position != currentLine.length())
		return currentLine[position];
}

void BufferClass::outputLine()
{
	for (int i = 0; i < currentLine.length(); i++)
		cout << currentLine[i];
}

int BufferClass::getLineNumber()
{
	return lineNumber;
}


/*
This class will read a single character from the buffer,
using the buffer class' get char
after getting the character it will determine which token it
corresponds to and return a token of the appropriate type.
*/
class ScannerClass
{
public:
	ScannerClass(); //initializes and opens the file for buffer
					//gets the token and returns it, a boolean 
					//is tracked to see if it finds the token
	token getToken(bool&);
	void outputToScreen(token);//Outputs the token, lexeme token# and line# to screen
	void endProg() { buffer.closeFile(); }; //closes the file.
private:
	BufferClass buffer;
	KeywordHash keys;
};

ScannerClass::ScannerClass()
{
	buffer.openFile("input.txt");
	if (!buffer.getNewLine())
	{
		cout << "\n\n**End of File**\n\n";
	}
}

token ScannerClass::getToken(bool& found)
{
	token tok;
	found = false;
	char input = buffer.getChar(); //gets the first character from the buffer.

	tok.lineNumber = buffer.getLineNumber();

	/*
	the following code tests the single character according to the DFA
	first we check if it is a number, according to the DFA 0 cannot start a #
	if the starting number is not 0, then we proceed until we come to a non digit and create the token
	if it starts with a letter we keep getting characters until we hit a non digit or non letter
	we then check if it is a keyword, if it is not then it is an identifier.
	finally we check for the individual symbols, ! ; = < etc
	finally we return the created token
	*/
	if (input == '0')
	{
		tok.lexeme = input;
		tok.tokenNumber = 2;
		found = true;
	}
	else if (isdigit(input) && input != '0') //102+
	{
		tok.lexeme += input;

		while (isdigit(buffer.peek()) && isdigit(input))
		{
			input = buffer.getChar();
			tok.lexeme += input;
		}
		tok.tokenNumber = 2;
		found = true;
	}
	else if (isalpha(input))
	{
		tok.lexeme += input;

		while ((isalpha(buffer.peek()) || isdigit(buffer.peek())) && (isalpha(input) || isdigit(input)))
		{
			input = buffer.getChar();
			tok.lexeme += input;
		}

		keys.checkKeyword(tok, found);

		if (!found)
		{
			tok.tokenNumber = 1;
			found = true;
		}
		else if (tok.tokenNumber == -5)//if it is a comment
		{
			buffer.getNewLine();
			tok = getToken(found);
		}
	}
	else if (input == '+' || input == '-')
	{
		tok.lexeme = input;
		tok.tokenNumber = 4;
		found = true;
	}
	else if (input == '*' || input == '/')
	{
		tok.lexeme = input;
		tok.tokenNumber = 5;
		found = true;
	}
	else if (input == '!')
	{
		if (buffer.peek() == '=')
		{
			tok.lexeme = "!=";
			tok.tokenNumber = 6;
			buffer.getChar();
		}
		else
		{
			tok.lexeme = input;
			tok.tokenNumber = 17;
		}
		found = true;
	}
	else if (input == ':')
	{
		if (buffer.peek() == '=')
		{
			tok.lexeme = ":=";
			tok.tokenNumber = 19;
			buffer.getChar();
			found = true;
		}
		else
			found = false;
	}
	else if (input == '=' || input == '<' || input == '>')
	{
		tok.lexeme = input;
		tok.tokenNumber = 6;
	}
	else if (input == '(')
	{
		tok.lexeme = input;
		tok.tokenNumber = 14;
		found = true;
	}
	else if (input == ')')
	{
		tok.lexeme = input;
		tok.tokenNumber = 15;
		found = true;
	}
	else if (input == ';')
	{
		tok.lexeme = input;
		tok.tokenNumber = 16;
		found = true;
	}
	else if (input == '"')
	{
		tok.lexeme = input;
		input = buffer.getChar();
		while (input != '"')
		{
			tok.lexeme += input;
			input = buffer.getCharForString();
		}
		tok.lexeme += input;
		tok.tokenNumber = 2;
		found = true;
	}
	else if (input == '.')
	{
		tok.lexeme = input;
		tok.tokenNumber = 18;
		found = true;
	}
	else if (input == ' ')
	{
		tok = getToken(found);
	}

	return tok;
}

void ScannerClass::outputToScreen(token tok)
{
	cout << " [ ";

	for (int i = 0; i < tok.lexeme.length(); i++)
		cout << tok.lexeme[i];
	cout << ", " << tok.tokenNumber << ", " << tok.lineNumber << " ] \n";
}

/*
The purpose of this class is to create the MIPS code gen.
*/

class CodeGenClass
{
public:
	void writeProlog();
	void writePostlog();
	void codeGen(string, string, string);
	void codeGen(string, string, int);
private:
	ofstream fout;
};

/*
Writes the prolog to the MIPS code:
#Prolog: next 7 lines will start all programs
.text
.globl main
main:
move   $fp  $sp    		#frame pointer will be used to access variables
la $a0 ProgStart   		#next 3 will print a header
li $v0 4
syscall

I wrote each line as a seperate fout statement for clarity for myself
*/
void CodeGenClass::writeProlog()
{
	fout.open("CodeGenOutput.txt");

	fout << ".test\n";
	fout << ".global main\n";
	fout << "main:\n";
	fout << "move $fp $sp\n";
	fout << "la $a0 Progstart\n";
	fout << "li $v0 4\n";
	fout << "syscall\n";
}

/*
Writes the postlog to the MIPS code:
#PostLog: next 8 lines will end all programs
la $a0 ProgEnd 		#next 3 will print a goodbye
li $v0 4
syscall
li $v0 10			#10 is program exit
syscall			#exits the MIPS program
.data
ProgStart: .asciiz	"Program Start\n"
ProgEnd:   .asciiz	"Program End\n"

I wrote each line as a seperate fout statement for clarity for myself
*/
void CodeGenClass::writePostlog()
{
	string start = "Program Start", end = "Program End";
	//i had a lot of trouble finding the way to print a string 
	//to file WITH quotes around it
	char quote = '"';		

	fout << "la $a0 ProgEnd\n";
	fout << "li $v0 4\n";
	fout << "syscall\n";
	fout << "li $v0 10\n";
	fout << "syscall\n";
	fout << ".data\n";
	fout << "ProgStart: .asciiz " << quote << start << quote << "\n";
	fout << "ProgEnd: .asciiz " << quote << end << quote << "\n";

	fout.close();
}

void CodeGenClass::codeGen(string first, string second, string last)
{
	fout << first << " " << second << " " << last << "\n";
}

void CodeGenClass::codeGen(string first, string second, int last)
{
	fout << first << " " << second << " " << last << "\n";
}

/*
The Purpose of the parser class is to determine whether the input file is in the proper context.
when the scanner is done sorting the code into tokens, the parser takes it and determines whether
the tokens are in the proper gramatical order or not.
*/
class ParserClass
{
public:
	ParserClass();
	token program(); //starts the parser
	token returnToken();
	void output();
private:
	token currentToken;
	ScannerClass scanner;
	SymbolTable symTable;
	CodeGenClass generate;
	bool noError;	//Used when an error is detected, allows easy exit
	bool match(int);
	void blockst();		//block statement method
	void stmt();		//statement method
	void decl();		//declaration method
	void assstat();		//assignment method
	void ifstat();		//if statement method
	void loopst();		//loop method
	void iostat();		//input/output method
	void expression(expRec&);	//addition/subtraction method
	void term(expRec&);		//multiplication/division method
	void relfact(expRec&);		//relational method
	void factor(expRec&);		//an element of an expression, id, literal etc
	void idref(expRec&);		//an ID token method
};

ParserClass::ParserClass()
{
	bool found = false;
	noError = true;
	currentToken = scanner.getToken(found);

	if (found)
		cout << "\nSuccess\n";
	else
		cout << "\nFailed to retrieve token.\n";
}

/*
Checks the current token to the expected type. If it is
the appropriate type, gets the next token and returns a true.
Otherwise it reports the error and exits program.
*/
bool ParserClass::match(int expectedTokenNumber)
{
	bool found = false;
	if (currentToken.tokenNumber == expectedTokenNumber)
	{
		currentToken = scanner.getToken(found);

		if (found)
			return true;
		else
			return false;
	}
	else
	{
		cout << "\nERROR: Wrong token: Expected " << expectedTokenNumber << " Received: " << currentToken.tokenNumber;
		return false;
	}

}

token ParserClass::program()
{
	cout << "1, ";
	if (currentToken.tokenNumber == 7)
	{
		generate.writeProlog();
		blockst();
	}
	
	if (currentToken.tokenNumber == 18)
	{
		generate.writePostlog();
	}

	return currentToken;
}

//this is used so that the main can retrieve the final 
//token and we do not have to have a series of
//returns on the functions
token ParserClass::returnToken()
{
	return currentToken;
}

void ParserClass::output()
{
	symTable.display();
}


void ParserClass::blockst()
{
	if (noError && currentToken.tokenNumber != 18)
	{
		cout << "7, ";

		if (currentToken.tokenNumber == 7)	//7 is the token# for BEGIN
		{
			symTable.incrementBlockStack();
			if (match(7))
			{
				while (currentToken.tokenNumber != 8 && noError)
				{
					stmt();
					if (currentToken.tokenNumber == 16)
					{
						if (!match(16))
						{
							cout << "\nError Expeted ;";
							noError = false;
						}
					}
					else
					{
						cout << "\nError, Expected ;";
						noError = false;
					}
				}
			}
			else
			{
				cout << "\nError on line: " << currentToken.lineNumber << " BEGIN token missing";
				noError = false;
			}
		}
		else
		{
			cout << "\nError on line: " << currentToken.lineNumber << " BEGIN token missing";
			noError = false;
		}

		if (currentToken.tokenNumber == 8) //8 is the token# for END
		{
			symTable.decrementBlockStack();
			if (!match(8))
			{
				cout << "\nError on line: " << currentToken.lineNumber << " END token missing";
				noError = false;
			}
		}

		//if after the END token there is a token that is not a '.' recall block statement		
		if (currentToken.tokenNumber != 18) 
			blockst();
	}

}

void ParserClass::stmt()
{
	if (noError)
	{

		if (currentToken.tokenNumber == 3)	//3 is the token# for an type token
		{
			cout << "2a, ";
			decl();
		}
		else if (currentToken.tokenNumber == 1) //1 is the token# for an identifier
		{
			cout << "2b, ";
			assstat();
		}
		else if (currentToken.tokenNumber == 9) //9 is the token# for an if statement
		{
			cout << "2c, ";
			ifstat();
		}
		else if (currentToken.tokenNumber == 11) //11 is the token number for a while
		{
			cout << "2d, ";
			loopst();
		}
		else if (currentToken.tokenNumber == 13) //13 is the token for read, write or writeln
		{
			cout << "2e, ";
			iostat();
		}
		else if (currentToken.tokenNumber == 7) //7 is the token number for begin
		{
			cout << "2f, ";
			blockst();
		}
	}

}

void ParserClass::decl()
{
	if (noError)
	{
		cout << "3, ";
		if (currentToken.tokenNumber == 3) //3 is the token for any keyword
		{
			string datatype = currentToken.lexeme;	//storing the datatype for the symbol table
			if (match(3))
			{
				bool found = false;
				idNode symbol = symTable.searchLocal(currentToken.lexeme, found);
				if (!found)
				{
					symTable.insert(currentToken.lexeme, currentToken.tokenNumber, datatype);
				}
				else
				{
					cout << "\n Non Fatal Error";
				}
				expRec er;
				er.offSet = OFFSET;
				er.type = datatype;
				idref(er);
			}
			else
			{
				noError = false;
				cout << "\nError expected Data Type.";
			}
		}
		else
		{
			noError = false;
			cout << "\nError expected Data Type.";
		}
	}
}

void ParserClass::assstat()
{
	if (noError)
	{
		cout << "4, ";
		if (currentToken.tokenNumber == 1)
		{
			if (match(1))
			{
				if (currentToken.tokenNumber == 19) //19 is the token for :=
				{
					if (match(19))
					{
						expRec er;
						expression(er);
					}
					else
					{
						noError = false;
						cout << "\nError expected Equal Sign.";
					}
				}
				else
				{
					noError = false;
					cout << "\nError expected Equal Sign.";
				}
			}
			else
			{
				noError = false;
				cout << "\nError expected Identifier.";
			}

		}
		else
		{
			noError = false;
			cout << "\nError expected Identifier.";
		}
	}
}

void ParserClass::ifstat()
{
	if (noError)
	{
		cout << "5, ";
		if (match(9))
		{
			expRec er;
			expression(er);
			if (currentToken.tokenNumber == 10) //10 is the token for then
			{
				if (match(10))
					stmt();
				else
				{
					cout << "\nError expected conditional statement";
					noError = false;
				}
			}
			else
			{
				cout << "\nError expected THEN statement";
				noError = false;
			}
		}
		else
		{
			cout << "\nError expected IF statement";
			noError = false;
		}
	}
}

void ParserClass::loopst()
{
	if (noError)
	{
		cout << "6, ";
		if (match(11))
		{
			expRec er;
			expression(er);
			if (currentToken.tokenNumber == 12) //12 is the token for do
			{
				if (match(12))
					stmt();
			}
			else
			{
				cout << "\nError expected THEN statement";
				noError = false;
			}
		}
		else
		{
			cout << "\nError expected WHILE statement";
			noError = false;
		}
	}
}

void ParserClass::iostat()
{
	if (noError)
	{
		if (currentToken.lexeme == "READ")
		{
			cout << "8a, ";
			if (match(13))
			{
				if (match(14))//this is the token for a (
				{
					if (currentToken.tokenNumber == 1)
					{
						expRec er;
						idref(er);
						if (currentToken.tokenNumber == 15) //this is the token for a )
						{
							if (!match(15))
							{
								cout << "\nError expected ')'";
								noError = false;
							}

						}
						else
						{
							cout << "\nError expected ')'";
							noError = false;
						}
					}
					else
					{
						cout << "\nError expected Identifier";
						noError = false;
					}
				}
				else
				{
					cout << "\nError expected '('";
					noError = false;
				}
			}
			else
			{
				cout << "\nError expected READ statement";
				noError = false;
			}
		}
		else if (currentToken.lexeme == "WRITE" || currentToken.lexeme == "WRITELN")
		{
			cout << "8b, ";
			if (match(13))
			{
				if (currentToken.tokenNumber == 14)
				{
					if (match(14))
					{
						expRec er;
						expression(er);
						if (currentToken.tokenNumber == 15)
						{
							if (!match(15))
							{
								cout << "\nError expected ')'";
								noError = false;
							}
						}
						else
						{
							cout << "\nError expected ')'";
							noError = false;
						}
					}
					else
					{
						cout << "\nError expected '('";
						noError = false;
					}
				}
				else
				{
					cout << "\nError expected '('";
					noError = false;
				}

			}
			else
			{
				cout << "\nError expected WRITE or WRITELN statement";
				noError = false;
			}
		}
		else
		{
			cout << "\nError expected IO statement";
			noError = false;
		}
	}
}

void ParserClass::expression(expRec& er)
{
	if (noError)
	{
		cout << "9, ";
		term(er);
		while (noError && currentToken.tokenNumber == 4) //4 is the addition/subtraction/or token
		{
			if (match(4))
			{
				term(er);
			}
			else
			{
				cout << "\nError Expected addition operator.";
				noError = false;
			}
		}
	}
}

void ParserClass::term(expRec& er)
{
	if (noError)
	{
		cout << "10, ";
		relfact(er);
		while (noError && currentToken.tokenNumber == 5) //5 is the token for multiplication
		{
			if (match(5))
			{
				relfact(er);
			}
			else
			{
				cout << "\nError Expected Multiplication Operator";
				noError = false;
			}
		}
	}

}

void ParserClass::relfact(expRec& er)
{
	if (noError)
	{
		cout << "11, ";
		factor(er);
		while (noError && currentToken.tokenNumber == 6) //6 is the relational operator token
		{
			if (match(6))
			{
				factor(er);
			}
			else
			{
				cout << "\nError Expected Relational Operator";
				noError = false;
			}
		}
	}
}

void ParserClass::factor(expRec& er)
{
	bool found = false;
	if (noError)
	{
		if (currentToken.tokenNumber == 1)
		{
			cout << "12a, ";
			idNode curr = symTable.searchLocal(currentToken.lexeme, found);
			er.offSet = OFFSET;
			er.type = curr.dataType;
			idref(er);
		}
		else if (currentToken.tokenNumber == 2)
		{
			cout << "12b, ";
			if (isalpha(currentToken.lexeme[0]))
			{
				er.type = "LOGICAL";
			}
			else
			{
				er.type = "INTEGER";
			}
			er.offSet = OFFSET;
			if (!match(2))
			{
				cout << "\nError Expected Literal Token";
				noError = false;
			}
		}
		else if (currentToken.tokenNumber == 17) //17 is boolean ! token
		{
			cout << "12c, ";
			if (match(17))
			{
				factor(er);
			}
			else
			{
				cout << "\nError Expected Boolean Token";
				noError = false;
			}
		}
		else if (currentToken.tokenNumber == 14) //14 is the ( token
		{
			cout << "12d, ";
			if (match(14))
			{
				idNode curr = symTable.searchLocal(currentToken.lexeme, found);
				er.offSet = OFFSET;
				er.type = curr.dataType;
				expression(er);
				if (currentToken.tokenNumber == 15) //15 is the ) token
				{
					if (!match(15))
					{
						cout << "\nError Expected ')'";
						noError = false;
					}
				}
				else
				{
					cout << "\nError expected ')'";
					noError = false;
				}
			}
			else
			{
				cout << "\nError expected '('";
				noError = false;
			}
		}
	}
}

void ParserClass::idref(expRec& er)
{
	if (noError)
	{
		cout << "13, ";
		if (currentToken.tokenNumber == 1)
		{
			if (!match(1))
			{
				cout << "\nError expected identifier";
				noError = false;
			}
			else
			{
				if (er.type == "INTEGER")
				{
					generate.codeGen("li", "$v0", 1);
					generate.codeGen("lw", "$a0", er.offSet);
					generate.codeGen("syscall", "", "");
				}
			}
		}
	}
}




int main()
{
	ParserClass parse;
	cout << "RULE: ";

	parse.program();

	if ((parse.returnToken()).tokenNumber == 18)
	{
		cout << "\nSuccessful Run.\n";
	}
	else
	{
		cout << "\nError: unexpected eof.\n";
	}

	parse.output();

	cout << endl;
	system("PAUSE");
	return 0;
}


