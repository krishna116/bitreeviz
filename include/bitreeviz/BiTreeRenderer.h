/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "BiTreeLayouter.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace bitree
{
/**
 * @brief Bitree renderer.
 */
class BiTreeRenderer
{
public:
	BiTreeRenderer();
	~BiTreeRenderer();
	/**
	 * @brief Rendering a bitree.
	 * 
	 * @param nodeInfoArray 	Input node info array.
	 * @param layoutInfo 		Input layout info.
	 * @param outputFile 		Input output file name.
	 * @return true 			Rendering pass.
	 * @return false 			Rendering not pass.
	 */
	bool draw(const NodeInfoArray* nodeInfoArray,
			  const BiTreeLayouter::LayoutInfo& layoutInfo,
			  const std::string& outputFile);
	/**
	 * @brief Get the best radius automatically for node rendering.
	 * 
	 * @param strLen 	Input the node's label's max length.
	 * @return double 	Radius for node-rendering.
	 */
	static double getBestRadius(const std::size_t& strLen);

private:
	struct pos_t
	{
		double x;
		double y;
		pos_t(const double& x_ = 0.0f, const double& y_ = 0.0f)
			: x(x_)
			, y(y_)
		{ }
	};

	struct vec2d_t
	{
		vec2d_t()
			: p1(pos_t())
			, p2(pos_t())
		{ }
		pos_t p1;
		pos_t p2;
	};

	vec2d_t getClippedEdge(const pos_t& childPos,
						   const pos_t& parentPos,
						   const double& radius,
						   bool& status);

	double getDistance(const pos_t& p1, const pos_t& p2);

	bool convertDashStringToCairoDash(const std::string& nodeDashStr,
									  std::vector<double>& dashes,
									  double& offset);

	//
	// Color functions-----------------------------
	//
	double red(uint32_t color)
	{
		return ((double)((color & 0xFF000000) >> 24)) / 255.0f;
	}

	double green(uint32_t color)
	{
		return ((double)((color & 0x00FF0000) >> 16)) / 255.0f;
	}

	double blue(uint32_t color)
	{
		return ((double)((color & 0x0000FF00) >> 8)) / 255.0f;
	}

	double alpha(uint32_t color)
	{
		return ((double)(color & 0x000000FF)) / 255.0f;
	}
}; // BiTreeRender-define-end
} // namespace bitree
