/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bitreeviz/BiTreeViz.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

bool ParsingUserInput(int argc, char **argv, std::string &inputFile, std::string &outputFile);

int main(int argc, char *argv[])
{
	std::string inputFile;
	std::string outputFile;

	if (ParsingUserInput(argc, argv, inputFile, outputFile))
	{
		bitree::BiTreeViz bitreeViz(inputFile, outputFile);
		return (int)bitreeViz.run();
	}

	return 0;
}

bool ParsingUserInput(int argc, char **argv, std::string &inputFile, std::string &outputFile)
{
	inputFile.clear();
	outputFile.clear();
	bool hasTask = false;

	if (argc == 1)
	{
		bitree::BiTreeViz::ShowHelp();
	}
	else if (argc == 2)
	{
		const std::string str = argv[1];
		if (str == "--help" || str == "-help" || str == "--h" || str == "-h")
		{
			bitree::BiTreeViz::ShowHelp();
		}
		else if (str == "--version" || str == "-v")
		{
			bitree::BiTreeViz::ShowVersion();
		}
		else
		{
			inputFile = argv[1];
			hasTask = true;
		}
	}
	else if (argc == 4 && (std::string(argv[2]) == "-o"))
	{
		inputFile = argv[1];
		outputFile = argv[3];
		hasTask = true;
	}
	else
	{
		printf("bitreeviz: invalid parameter input.\n");
	}

	return hasTask;
}
