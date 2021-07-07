/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bitreeviz/BiTreeLayouter.h"
#include "bitreeviz/BiTreeDefines.h"
#include "bitreeviz/BiTreePropertyManager.h"

#include <cassert>
#include <iostream>
#include <limits>
#include <stack>
#include <vector>

using namespace bitree;

BiTreeLayouter::BiTreeLayouter(const double& radius)
{
	BiTreeAssert(radius > 1.0f);
	BiTreeAssert(mLayoutInfo == nullptr);
	mLayoutInfo = new LayoutInfo();
	BiTreeAssert(mLayoutInfo != nullptr);

	mLayoutInfo->offset_h_ =
		Sin30 * 2.1 * radius +
		BiTreePropertyManager::getCurrentProperty().nodeExtendProperty.edgeXIncrement;

	mLayoutInfo->offset_v_ =
		Sin60 * 2.1 * radius +
		BiTreePropertyManager::getCurrentProperty().nodeExtendProperty.edgeYIncrement;

	auto baseValue = 5.0f + Sin30 * 2.5 * radius;
	mLayoutInfo->pageMarginX =
		baseValue +
		BiTreePropertyManager::getCurrentProperty().nodeExtendProperty.pageMarginXIncrement;
	mLayoutInfo->pageMarginY =
		baseValue +
		BiTreePropertyManager::getCurrentProperty().nodeExtendProperty.pageMarginYIncrement;
	
	// This radius is only used for drawing.
	mLayoutInfo->raduis =
		radius + BiTreePropertyManager::getCurrentProperty().nodeExtendProperty.radiusIncrement;	
}

BiTreeLayouter::~BiTreeLayouter()
{
	if(mLayoutInfo)
	{
		delete mLayoutInfo;
		mLayoutInfo = nullptr;
	}
}

size_t BiTreeLayouter::internalGetDepth(const BiTreeParser::BiTreeNode* node)
{
	if(node == nullptr)
	{
		return 0;
	}
	else
	{
		size_t leftDepth = internalGetDepth(node->leftChild) + 1;
		size_t rightDepth = internalGetDepth(node->rightChild) + 1;

		return std::max(leftDepth, rightDepth);
	}
}

size_t BiTreeLayouter::getDepth(const BiTreeParser::BiTreeNode* node)
{
	BiTreeAssert(node != nullptr);

	return internalGetDepth(node);
}

double BiTreeLayouter::internalGetLeftOrRightMostX(const BiTreeParser::BiTreeNode* node,
												   const size_t depth,
												   bool isUsingLeft)
{
	BiTreeAssert(node != nullptr);

	size_t i = 1;
	std::vector<const BiTreeParser::BiTreeNode*> vec{node};

	// Get the child to root-total-offset to calculate the absolute x-coordinate relative to its subTree.
	auto getSubTreeOffset = [](const BiTreeParser::BiTreeNode* root,
							   const BiTreeParser::BiTreeNode* child) {
		double subTreeOffset = 0.0f;

		while(child != root) //from the child to root.
		{
			subTreeOffset += child->subTreeOffsetX;
			child = child->parent;
			BiTreeAssert(child != nullptr);
		}
		subTreeOffset += root->subTreeOffsetX;

		return subTreeOffset;
	};

	// 001, find the level;
	while(!vec.empty())
	{
		if(i == depth)
		{
			break;
		}
		else
		{
			std::vector<const BiTreeParser::BiTreeNode*> tempVec;
			for(auto& n : vec)
			{
				if(n->leftChild)
					tempVec.push_back(n->leftChild);
				if(n->rightChild)
					tempVec.push_back(n->rightChild);
			}
			vec = std::move(tempVec);
			i++;
		}
	} //end-while

	// 002, find the left-most or right-most node at the level;
	BiTreeAssert(!vec.empty());
	auto n1 = vec.front();
	if(isUsingLeft)
	{
		for(auto& n2 : vec)
		{
			if(n2->x + getSubTreeOffset(node, n2) < n1->x + getSubTreeOffset(node, n1))
			{
				n1 = n2;
			}
		}
	}
	else
	{
		for(auto& n2 : vec)
		{
			if(n1->x + getSubTreeOffset(node, n1) < n2->x + getSubTreeOffset(node, n2))
			{
				n1 = n2;
			}
		}
	}

	return (n1->x + getSubTreeOffset(node, n1));
}

double BiTreeLayouter::getLeftMostX(const BiTreeParser::BiTreeNode* node, const size_t depth)
{
	return internalGetLeftOrRightMostX(node, depth, true);
}

double BiTreeLayouter::getRightMostX(const BiTreeParser::BiTreeNode* node, const size_t depth)
{
	return internalGetLeftOrRightMostX(node, depth, false);
}

double BiTreeLayouter::getMiddleX(BiTreeParser::BiTreeNode* leftChild,
								  BiTreeParser::BiTreeNode* rightChild)
{
	// If the two subTree do not have the same depth,
	// just check the minimal heigh of both subTree(the collision will be happened there).
	auto depths = std::min(getDepth(leftChild), getDepth(rightChild));

	// Check collision from layer(1) to layer(depthes);
	for(size_t i = 1; i <= depths; i++)
	{
		auto offset1 = getRightMostX(leftChild, i);
		auto offset2 = getLeftMostX(rightChild, i);

		if(offset1 + offsetH() * 2 > offset2) // Collision happened;
		{
			// Keep leftSubTree still, and shift rightSubTree to right position;
			rightChild->subTreeOffsetX += offset1 + offsetH() * 2 - offset2;
		}
	}

	// Parent-node's x-coordinate is locate at the middle of its leftChild and rightChild;
	return leftChild->subTreeOffsetX + leftChild->x +
		   ((rightChild->subTreeOffsetX + rightChild->x) -
			(leftChild->subTreeOffsetX + leftChild->x)) /
			   2;
}

void BiTreeLayouter::firstInitYCoordinate(BiTreeParser::BiTreeNode* node)
{
	BiTreeAssert(node != nullptr);

	node->parent = nullptr;
	size_t level = 0;
	std::vector<BiTreeParser::BiTreeNode*> vec{node};

	// Access node(s) by layer.
	while(!vec.empty())
	{
		std::vector<BiTreeParser::BiTreeNode*> tempVec;
		for(auto& n : vec)
		{
			n->y = this->offsetV() * (double)(level);
			if(n->leftChild)
			{
				n->leftChild->parent = n;
				tempVec.push_back(n->leftChild);
			}

			if(n->rightChild)
			{
				n->rightChild->parent = n;
				tempVec.push_back(n->rightChild);
			}
		}
		vec = std::move(tempVec);
		level++;
	}
}

void BiTreeLayouter::SecondInitXCoordinate(BiTreeParser::BiTreeNode* node)
{
	BiTreeAssert(node != nullptr);

	getLayoutInfo()->bitreeNodeArray.clear();
	std::stack<BiTreeParser::BiTreeNode*> stack1;
	std::stack<BiTreeParser::BiTreeNode*> stack2;

	stack1.push(node);

	while(!stack1.empty())
	{
		auto n = stack1.top();
		stack1.pop();
		stack2.push(n);

		if(n->leftChild)
			stack1.push(n->leftChild);
		if(n->rightChild)
			stack1.push(n->rightChild);
	}

	while(!stack2.empty())
	{
		auto n = stack2.top();

		stack2.pop();

		if(n->leftChild == nullptr && n->rightChild == nullptr)
		{
			n->x = 0.0;
		}
		else if(n->leftChild != nullptr && n->rightChild == nullptr)
		{
			n->x = n->leftChild->x + this->offsetH();
		}
		else if(n->leftChild == nullptr && n->rightChild != nullptr)
		{
			n->x = n->rightChild->x - this->offsetH();
		}
		else
		{
			n->x = getMiddleX(n->leftChild, n->rightChild);
		}
		getLayoutInfo()->bitreeNodeArray.push_back(n);
	}
}

void BiTreeLayouter::finalInitXYCoordinate(BiTreeParser::BiTreeNode* node)
{
	BiTreeAssert(node != nullptr);

	auto curr = node;
	std::stack<BiTreeParser::BiTreeNode*> stack;

	double minX=0.0f;
	double maxX=0.0f;
	double maxY=0.0f;

	while(!stack.empty() || curr)
	{
		if(curr)
		{
			if(curr->parent != nullptr)
			{
				curr->subTreeOffsetX += curr->parent->subTreeOffsetX;
			}

			curr->x += curr->subTreeOffsetX;

			// Collect infomation begin.
			if(curr->x < minX) minX=curr->x;
			if(maxX < curr->x) maxX=curr->x;
			if(maxY < curr->y) maxY=curr->y;
			// Collect infomation end.

			stack.push(curr);
			curr = curr->leftChild;
		}
		else
		{
			curr = stack.top();
			stack.pop();
			curr = curr->rightChild;
		}
	} //while-end;

	//
	//	[The drawing page style]
	//
	//                       pageMarginY
	//                           |
	//                           |
	// pageMarginX-------------BiTree-----------pageMarginX
	//                           |
	//                           |
	//                       pageMarginY
	//
	//
	getLayoutInfo()->pageWidth = 2 * (getLayoutInfo()->pageMarginX) + (maxX - minX);
	getLayoutInfo()->pageheight = 2 * (getLayoutInfo()->pageMarginY) + maxY;

	if(minX<0.0f)
	{
		// This value will move the tree to the right when drawing the tree-node.
		getLayoutInfo()->treeOffsetX = std::abs(minX);
	}
	else if(minX>0.0f)
	{
		// This value will move the tree to the left when drwwing the tree-node.
		getLayoutInfo()->treeOffsetX = -(minX);
	}
	else
	{
		getLayoutInfo()->treeOffsetX=0.0f;
	}
}

bool BiTreeLayouter::layout(BiTreeParser::BiTreeNode* root)
{
	if(root)
	{
		firstInitYCoordinate(root);
		SecondInitXCoordinate(root);
		finalInitXYCoordinate(root);

		return true;
	}

	return false;
}

double BiTreeLayouter::offsetH()
{
	BiTreeAssert(mLayoutInfo != nullptr);
	return mLayoutInfo->offset_h_;
}

double BiTreeLayouter::offsetV()
{
	BiTreeAssert(mLayoutInfo != nullptr);
	return mLayoutInfo->offset_v_;
}

BiTreeLayouter::LayoutInfo* BiTreeLayouter::getLayoutInfo()
{
	BiTreeAssert(mLayoutInfo != nullptr);
	return mLayoutInfo;
}

const BiTreeLayouter::LayoutInfo* BiTreeLayouter::getLayoutInfoConst()
{
	BiTreeAssert(mLayoutInfo != nullptr);
	return mLayoutInfo;
}
