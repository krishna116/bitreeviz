/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bitreeviz/BiTreeFileParser.h"
#include "bitreeviz/BiTreeDefines.h"
#include "bitreeviz/BiTreePropertyManager.h"
#include "bitreeviz/BiTreeRenderer.h"

#include <fstream>
#include <sstream>

using namespace bitree;

BiTreeFileParser::BiTreeFileParser(const std::string& inputFile)
{
	mFileParserInfo.mInputFile=inputFile;
	mFileParserInfo.mStartToken="bitree";
	mFileParserInfo.mBiTreeParser=nullptr;
	mFileParserInfo.mBiTreeLayouter=nullptr;
}

BiTreeFileParser::~BiTreeFileParser()
{
	if(mFileParserInfo.mBiTreeParser)
	{
		delete mFileParserInfo.mBiTreeParser;
		mFileParserInfo.mBiTreeParser=nullptr;
	}
	if(mFileParserInfo.mBiTreeLayouter)
	{
		delete mFileParserInfo.mBiTreeLayouter;
		mFileParserInfo.mBiTreeLayouter=nullptr;
	}
}

bool BiTreeFileParser::parsing()
{
	bool status = false;
	bool isBiTreeParsed = false;

	auto& biTreeParser = mFileParserInfo.mBiTreeParser;
	if(biTreeParser == nullptr)
	{
		biTreeParser=new BiTreeParser;
		BiTreeAssert(biTreeParser!=nullptr);
	}

	// Open file;
	if(mFileParserInfo.mInputFile.empty())
	{
		PrintError("Input file is empty.\n");
		return status;
	}

	PrintInfo("Open file: [%s]\n", mFileParserInfo.mInputFile.c_str());
	std::ifstream inFile(mFileParserInfo.mInputFile);
	if(!inFile.good())
	{
		PrintError("Cannot open file:[%s]\n", mFileParserInfo.mInputFile.c_str());
		return status;
	}

	auto propertyParser = biTreeParser->getPropertyParser();
	BiTreeAssert(propertyParser!=nullptr);

	// Read file line by line;
	for(std::string line; std::getline(inFile, line);)
	{
		str_ptr_t begin = line.begin();
		str_ptr_t end = line.end();

		begin = trimLine(begin, end);
		if(begin >= end)
			continue;

		// Parsing a line.
		if(!isBiTreeParsed)
		{
			PrintInfo("Check file begin with [ %s = ].........\n",
					  mFileParserInfo.mStartToken.c_str());
			begin = trimStartToken(begin, end, status);
			if(status == false)
			{
				PrintInfo("Check file begin with [ %s = ].........failed\n\n",
						  mFileParserInfo.mStartToken.c_str());
				break;
			}
			else
			{
				PrintInfo("Check file begin with [ %s = ].........ok\n\n",
						  mFileParserInfo.mStartToken.c_str());
			}

			PrintInfo("Parsing bitree.............................\n");
			status = biTreeParser->parsing(line.substr(begin - line.begin(), end - begin + 1));
			isBiTreeParsed = true;

			if(status == false)
			{
				PrintError("Parsing bitree.............................failed.\n");
				break;
			}

			PrintInfo("Parsing bitree.............................ok\n\n");
		}
		else
		{
			// Parsing properties----------------------------------
			static size_t i = 0;
			if(propertyParser->parsing(begin, end) != true)
			{
				status = false;
				break;
			}
		}
	}

	// Parsing properties done.
	printf("\n");

	if(status)
	{
		PrintInfo("Layouting bitree..........................\n");
		auto& layouter = mFileParserInfo.mBiTreeLayouter;

		BiTreeAssert(layouter == nullptr);
		BiTreeAssert(mFileParserInfo.mBiTreeParser!=nullptr);
		auto radius = BiTreeRenderer::getBestRadius(
		mFileParserInfo.mBiTreeParser->getMaxTokenSizeCurrent());
		layouter=new BiTreeLayouter(radius);
		BiTreeAssert(layouter != nullptr);

		auto root = mFileParserInfo.mBiTreeParser->getRoot();
		status = layouter->layout(root);
		if(status)
		{
			PrintInfo("Layouting bitree..........................ok\n");
			PrintInfo("LayoutInfo: [radius = %f, width = %f, height = %f]\n\n",
					  layouter->getLayoutInfoConst()->raduis,
					  layouter->getLayoutInfoConst()->pageWidth,
					  layouter->getLayoutInfoConst()->pageheight);
		}
		else
		{
			PrintInfo("Layouting bitree...........................failed\n");
		}
	}

	return status;
}

const NodeInfoArray* BiTreeFileParser::getNodeInfoArray()
{
	BiTreeAssert(mFileParserInfo.mBiTreeParser);
	return mFileParserInfo.mBiTreeParser->getPropertyParser()->getNodeInfoArray();
}

const BiTreeLayouter::LayoutInfo& BiTreeFileParser::getLayoutInfo()
{
	BiTreeAssert(mFileParserInfo.mBiTreeLayouter != nullptr);
	return *(mFileParserInfo.mBiTreeLayouter->getLayoutInfoConst());
}

std::string::const_iterator BiTreeFileParser::trimLine(std::string::const_iterator& begin,
													   std::string::const_iterator& end)
{
	// Trim front space.
	while(begin < end && *begin <= 0x20)
		++begin;

	// It is note line.
	if(begin<end && (*begin=='/'||*begin=='#'))
	{
		begin=end;
		return begin;
	}

	// Trim back spaces
	auto size = end - begin;
	if(size > 0)
	{
		for(auto j = size - 1; j > 0; j--)
		{
			if(*(begin + j) <= 0x20 || *(begin + j) == '/' || *(begin + j) == ';' ||
			   *(begin + j) == ',')
			{
				continue;
			}
			else
			{
				end = begin + j;
				break;
			}
		}
	}

	return begin;
}

std::string::const_iterator BiTreeFileParser::trimStartToken(std::string::const_iterator begin,
															 std::string::const_iterator end,
															 bool& status)
{
	status = false;
	const std::string startToken = mFileParserInfo.mStartToken;
	auto begin2 = startToken.begin();
	auto end2 = startToken.end();

	// Case-insensitive-compare;
	auto equal = [](char char1, char char2) -> bool {

		if(char1 >= 'a')
			char1 -= 32;
		if(char2 >= 'a')
			char2 -= 32;

		return char1 == char2;
	};

	// Trim startToken;
	while(begin < end && begin2 < end2 && equal(*begin, *begin2))
	{
		++begin;
		++begin2;
	}

	if(begin2 != end2)
	{
		status = false;
		return begin;
	}

	// Trim space.
	while(begin < end && *begin <= 0x20)
		++begin;

	// Trim "="
	if(*begin != '=')
	{
		status = false;
		return begin;
	}
	else
	{
		++begin;
		status = true;
	}

	return begin;
}
