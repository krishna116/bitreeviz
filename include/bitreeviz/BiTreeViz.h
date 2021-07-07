/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <string>

namespace bitree
{
class BiTreeFileParser;
class BiTreeRenderer;
/**
 * @brief BiTreeViz main class.
 */
class BiTreeViz
{
public:
	/**
	 * @brief Constructor with file parameters.
	 * The inputFile should not empty;
	 * The outFile is otpional, it will using inputFileName with suffix[.pdf] if it is empty.
	 * 
	 * @param inputFile 	Input file name.
	 * @param outputFile 	Output file name.
	 */
	BiTreeViz(const std::string& inputFile, const std::string& outputFile = "");
	~BiTreeViz();

	/**
	 * @brief Parsing and Layouting and rendering the bitree.
	 * 
	 * @return true 	Operations all pass.
	 * @return false 	Operations have error(s).
	 */
	bool run();
	static const std::string& getNameAndVersion();
	static void ShowHelp();
	static void ShowVersion();

private:
	std::string mInputFile;
	std::string mOutputfile;
	BiTreeRenderer* mRenderer = nullptr;
	BiTreeFileParser* mFileParser = nullptr;

	/**
	 * @brief Convert char "\" to "/" in the filename as cairomm doesn't recognize the former.
	 * 
	 * @param fileName 		Input file name string.
	 * @return std::string 	Converted file name string.
	 */
	std::string convertSlash(std::string fileName);
};
} // namespace bitree
