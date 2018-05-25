/*
Amir Daud
Parser Program
Takes the tokens from the scanner and determines if it follows the appropriate grammar rules.
The Recursive Descent Grammar Rules are as Follows:
(1) program    :  blockst '.'
(2) statmt     :  decl  | assstat | ifstat | blockst | loopst  | iostat | <empty>
(3) decl       :  BASICTYPTOK IDTOK
(4) assstat       		:  idref   ASTOK  expression
(5) ifstat            	:  IFTOK exp THENTOK  statmt
(6) loopst            	:  WHILETOK exp DOTOK statmt
(7) blockst           	:  BEGINTOK  {statmt ';'} ENDTOK
(8) iostat			:  READTOK ( idref ) | WRITETOK (expression)
(9) expression       	:  term { ADDOPTOK term }
(10) term             	:  relfactor {MULOPTOK relfactor}
(11) relfact          	:  factor  [ RELOPTOK factor ]
(12) factor           	:  idref  | LITOK  | NOTTOK factor | ‘(‘ exp ‘)‘
(13) idref          	:  IDTOK

*/


#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
using namespace std;

const int SIZE = 13;

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
private:
	token currentToken;
	ScannerClass scanner;
	bool noError;	//Used when an error is detected, allows easy exit
	bool match(int);
	void blockst();		//block statement method
	void stmt();		//statement method
	void decl();		//declaration method
	void assstat();		//assignment method
	void ifstat();		//if statement method
	void loopst();		//loop method
	void iostat();		//input/output method
	void expression();	//addition/subtraction method
	void term();		//multiplication/division method
	void relfact();		//relational method
	void factor();		//an element of an expression, id, literal etc
	void idref();		//an ID token method
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

	blockst();

	return currentToken;
}

//this is used so that the main can retrieve the final 
//token and we do not have to have a series of
//returns on the functions
token ParserClass::returnToken()
{
	return currentToken;
}


void ParserClass::blockst()
{
	if (noError)
	{
		cout << "7, ";

		if (currentToken.tokenNumber == 7)	//7 is the token# for BEGIN
		{
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
			if (!match(8))
			{
				cout << "\nError on line: " << currentToken.lineNumber << " END token missing";
				noError = false;
			}
		}
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
			if (match(3))
				idref();
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
						expression();
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
			expression();
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
			expression();
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
						idref();
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
						expression();
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

void ParserClass::expression()
{
	if (noError)
	{
		cout << "9, ";
		term();
		while (noError && currentToken.tokenNumber == 4) //4 is the addition/subtraction/or token
		{
			if (match(4))
			{
				term();
			}
			else
			{
				cout << "\nError Expected addition operator.";
				noError = false;
			}
		}
	}
}

void ParserClass::term()
{
	if (noError)
	{
		cout << "10, ";
		relfact();
		while (noError && currentToken.tokenNumber == 5) //5 is the token for multiplication
		{
			if (match(5))
			{
				relfact();
			}
			else
			{
				cout << "\nError Expected Multiplication Operator";
				noError = false;
			}
		}
	}
	
}

void ParserClass::relfact()
{
	if (noError)
	{
		cout << "11, ";
		factor();
		while (noError && currentToken.tokenNumber == 6) //6 is the relational operator token
		{
			if (match(6))
			{
				factor();
			}
			else
			{
				cout << "\nError Expected Relational Operator";
				noError = false;
			}
		}
	}
}

void ParserClass::factor()
{
	if (noError)
	{
		if (currentToken.tokenNumber == 1)
		{
			cout << "12a, ";
			idref();
		}
		else if (currentToken.tokenNumber == 2)
		{
			cout << "12b, ";
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
				factor();
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
				expression();
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

void ParserClass::idref()
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

	cout << endl;
	system("PAUSE");
	return 0;
}

