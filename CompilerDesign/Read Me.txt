Compiler Design Work by Amir Daud Ansari

The work in this folder is partially complete. The Parser, Scanner and Symbol table all work individually. The compiler reads a specific dumbed down version of Algol called Algol W.


File descriptions:

Algol W LexElts 2017.doc

	Has the detailed description of the language given by the Professor for Compiler Design.



symboltable 1.cpp

	This program reads in a text file and using a basic hash table it organizes the tokens. This program will also identify scope of the tokens.



Scanner.cpp

	This program will read a text file line by line. It will scan the line letter by letter until it finds a token. It will then output the token to screen and to a outfile.



parserv1.cpp

	Takes the output from the Scanner and determines whether it follows the syntax rules. The rules can be found at the top of the file or in more detail in Algol W LexElts 2017.doc



WIP Final.cpp

	This was supposed to take the summation of the other files and add Code Generation into MIPS Assembly code. Due to the time contraints of a quarter system class I was unable to complete this part. 