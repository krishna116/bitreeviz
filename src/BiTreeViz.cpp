/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bitreeviz/BiTreeViz.h"

#include "bitreeviz/BiTreeDefines.h"
#include "bitreeviz/BiTreeFileParser.h"
#include "bitreeviz/BiTreeRenderer.h"

#include "config.h"

using namespace bitree;

const std::string& BiTreeViz::getNameAndVersion()
{
    static std::string nameAndVersionStr;

    nameAndVersionStr += BITREEVIZ_APP_NAME;
    nameAndVersionStr += " version ";
    nameAndVersionStr += std::to_string(BITREEVIZ_VERSION_MAJOR) + ".";
    nameAndVersionStr += std::to_string(BITREEVIZ_VERSION_MINOR) + ".";
    nameAndVersionStr += std::to_string(BITREEVIZ_VERSION_PATCH);

    return nameAndVersionStr;
}

BiTreeViz::BiTreeViz(const std::string& inputFile, const std::string& outputFile)
{
    mInputFile = convertSlash(inputFile);

    if(mOutputfile.empty())
    {
        mOutputfile = mInputFile + ".pdf";
    }
    else
    {
        mOutputfile = convertSlash(outputFile);
    }
}

BiTreeViz::~BiTreeViz()
{
    if(mFileParser)
    {
        delete mFileParser;
        mFileParser = nullptr;
    }
    if(mRenderer)
    {
        delete mRenderer;
        mRenderer = nullptr;
    }
}

std::string BiTreeViz::convertSlash(std::string fileName)
{
    for(auto& ch : fileName)
    {
        if(ch == '\\')
            ch = '/';
    };

    return fileName;
}

bool BiTreeViz::run()
{
    bool status = false;

    BiTreeAssert(mFileParser == nullptr);
    mFileParser = new BiTreeFileParser(mInputFile);
    BiTreeAssert(mFileParser != nullptr);

    BiTreeAssert(mRenderer == nullptr);
    mRenderer = new BiTreeRenderer();
    BiTreeAssert(mRenderer != nullptr);

    if(mFileParser->parsing())
    {
        status = mRenderer->draw(
            mFileParser->getNodeInfoArray(), mFileParser->getLayoutInfo(), mOutputfile);
    }

    PrintInfo("Done.");

    return status;
}

void BiTreeViz::ShowHelp()
{
    static const std::string usageStr = 
    R"~(bitreeviz is a tool for binary tree visualization.
                                       --from Krsihna, sssky307@163.com 
Usage:
  bitreeviz.exe <file> [-o <file2>]    //Read <file>, output to [<file2>].
  bitreeviz.exe --help                 //Show this help.
  bitreeviz.exe --version              //Show version.
)~";

    printf("%s\n", usageStr.c_str());
}

void BiTreeViz::ShowVersion()
{
    printf("%s\n", getNameAndVersion().c_str());
}
