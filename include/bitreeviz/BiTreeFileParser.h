/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "BiTreeLayouter.h"
#include "BiTreeParser.h"

#include <string>

namespace bitree
{
	/**
	 * @brief Parsing user input file.
	 * After parsing, the follow information can be used to render a bitree.
	 * 1, all node's position in mBiTreeParser;
	 * 2, page-info in mBiTreeLayouter.
	 */
class BiTreeFileParser
{
public:
	struct FileParserInfo
	{
		std::string mInputFile; 					///< User input file.
		std::string mStartToken; 					///< Input file start token.
		BiTreeParser* mBiTreeParser = nullptr; 		///< Used to parse bitree
		BiTreeLayouter* mBiTreeLayouter = nullptr; 	///< Used to layout bitree.
	};

public:
	BiTreeFileParser(const std::string& inputFile);
	~BiTreeFileParser();
	bool parsing();

	const NodeInfoArray* getNodeInfoArray();

	/**
	 * @brief Get the Layout Info, it should be used after parsing.
	 * 
	 * @return LayoutInfo Parsed Layout Info.
	 */
	const BiTreeLayouter::LayoutInfo& getLayoutInfo();

private:
	FileParserInfo mFileParserInfo;

	/**
	 * @brief Trim a file-line text.
	 * Trim the font spaces and the back spaces and notes.
	 * 
	 * @param begin Pointer to string buffer begin.
	 * @param end Pointer to string buffer end.
	 * @return trimed start pos.
	 */
	std::string::const_iterator trimLine(std::string::const_iterator& begin,
										 std::string::const_iterator& end);

	/**
	 * @brief Trim off the file's start token before the bitree's real begin.
	 * 
	 * @param begin Pointer to string buffer begin.
	 * @param end Pointer to string buffer end.
	 * @param status Store result, true if success.	
	 * @return trimed start pos.
	 */
	std::string::const_iterator trimStartToken(std::string::const_iterator begin,
											   std::string::const_iterator end,
											   bool& status);
}; // BiTreeFileParser-end
} // namespace bitree
