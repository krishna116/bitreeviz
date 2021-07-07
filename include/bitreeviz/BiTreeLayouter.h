/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "BiTreeParser.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace bitree
{
/**
 * @brief  Used to layout a bitree.
 */
class BiTreeLayouter
{
public:
	struct LayoutInfo;

public:
	/**
    * @brief Constructer
    * 
    * @param radius Input node radius for layout, all node's radius are all the same size.
    */
	BiTreeLayouter(const double& radius);
	~BiTreeLayouter();

	/**
    * @brief Main layout function.
    * 
    * @param root The bitree will be layouted.
    * @return true Layout pass.
    * @return false Layout failed.
    */
	bool layout(BiTreeParser::BiTreeNode* root);
   
   /**
    * @brief Get the Internal LayoutInfo.
    * You should call this after layouting any bitree to get the latest result.
    * 
    * @return LayoutInfo   LayoutInfo
    */
	const LayoutInfo* getLayoutInfoConst();

private:
	LayoutInfo* mLayoutInfo = nullptr;
	LayoutInfo* getLayoutInfo();
	double offsetH();
	double offsetV();

	/**
   * @brief Internal calculate a tree's depth or height;
   *
   * @param node      A tree's root node;
   * @return size_t   Depth of this tree;
   */
	size_t internalGetDepth(const BiTreeParser::BiTreeNode* node);

	/**
   * @brief Get the Depth of this tree;
   *
   * @param node      A tree's root node, input cannot be nullptr;
   * @return size_t   Depth of this tree;
   */
	size_t getDepth(const BiTreeParser::BiTreeNode* node);

	/**
   * @brief Used to get a tree's left-most or right-most node's x-coordinate at
   * specified depth;
   *
   * @param node          A tree's root node, input cannot be nullptr;
   * @param depth         Input a depth, this depth must <= this tree's
   * max-depth;
   * @param isUsingLeft   Left or right;
   * @return float        Absolute x coordinate position of the node(relative to
   * this tree).
   */
	double internalGetLeftOrRightMostX(const BiTreeParser::BiTreeNode* node,
									   const size_t depth,
									   bool isUsingLeft);

	/**
   * @brief Get the left-most node's x-coordinate at specified depth;
   *
   * @param node          A tree's root node, input cannot be nullptr;
   * @param depth         Input a depth, this depth must <= this tree's
   * max-depth;
   * @return float        Absolute x coordinate position of the node(relative to
   * this tree).
   */
	double getLeftMostX(const BiTreeParser::BiTreeNode* node, const size_t depth);

	/**
   * @brief Get the right-most node's x-coordinate at specified depth;
   *
   * @param node          A tree's root node, input cannot be nullptr;
   * @param depth         Input a depth, this depth must <= this tree's
   * max-depth;
   * @return float        Absolute x coordinate position of the node(relative to
   * this tree).
   */
	double getRightMostX(const BiTreeParser::BiTreeNode* node, const size_t depth);

	/**
   * @brief Get the Middle-x-coordinate for the parent node;
   *
   * @param leftChild     Left child of a parent node;
   * @param rightChild    Right child of a parent node;
   * @return float        Middle x coordinate for the parent node using.
   */
	double getMiddleX(BiTreeParser::BiTreeNode* leftChild, BiTreeParser::BiTreeNode* rightChild);

	/**
   * @brief Layer-order of a tree, calculate the initial y-coordinate for every
   * node in the tree, and init parent node for each node.
   *
   * @param node          It should be the top root node of a tree.
   */
	void firstInitYCoordinate(BiTreeParser::BiTreeNode* node);

	/**
   * @brief Post-order of a tree, calculate the initial x-coordinate for every
   * node in the tree;
   *
   * @param node          It should be the top root node of a tree.
   */
	void SecondInitXCoordinate(BiTreeParser::BiTreeNode* node);

	/**
   * @brief Pre-order of a tree, calculate the final x-coordinate and
   * y-coordinate for every node in the tree;
   *
   * @param node          It should be the top root node of a tree.
   */
	void finalInitXYCoordinate(BiTreeParser::BiTreeNode* node);

public:
	struct LayoutInfo
	{
		LayoutInfo()
			: offset_h_(15.0f)
			, offset_v_(30.0f)
			, pageMarginX(0.0f)
			, pageMarginY(0.0f)
			, pageWidth(0.0f)
			, pageheight(0.0f)
			, raduis(0.0f)
         ,treeOffsetX(0.0f)
		{ }
		double offset_h_; ///< Horizontal offset between two nodes;
		double offset_v_; ///< Vertical offset between two nodes;
		double pageMarginX; ///< Page margin x;
		double pageMarginY; ///< Page margin y;
		double pageWidth; ///< Page width, it will be run-time generated;
		double pageheight; ///< Page height, it will be run-time generated;
		double raduis; ///< Node radius.
      double treeOffsetX;
		std::vector<BiTreeParser::BiTreeNode*> bitreeNodeArray;

      // Convert BitreeNode's x to absolute x for drawing.
      double toAbsoluteX(double bitreeNodeX)const
      {
         return bitreeNodeX + treeOffsetX + pageMarginX;
      }

      // Convert BitreeNode's y to absolute y for drawing.
      double toAbsoluteY(double bitreeNodeY)const
      {
         return bitreeNodeY + pageMarginY;
      }
	};
}; //BiTreeParser-define-end
} // namespace bitree
