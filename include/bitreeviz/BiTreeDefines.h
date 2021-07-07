/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <any>
#include <cstdint>
#include <set>
#include <string>

namespace bitree
{
// NodeId's range;
constexpr uint32_t NodeIdMin = 0;
constexpr uint32_t NodeIdMax = 0xfffffff0;
constexpr uint32_t NodeIdInvalid = NodeIdMax + 1;

// Common values.
constexpr uint32_t DefaultWhiteColor = 0xffffffff;
constexpr uint32_t DefaultBlackColor = 0x000000ff;
constexpr uint32_t DefaultLineWidth = 1;

// It is all node's default radius.
constexpr uint32_t NodeRadiusDefault = 0; // Must be zero to use auto generated radius value.
constexpr uint32_t NodeRadiusMin = 0;
constexpr uint32_t NodeRadiusMax = 255;

// NodeLabel length's range decide the-automatically-generated-radius of renderer. 
constexpr uint32_t NodeLabelStrLenMin = 1;
constexpr uint32_t NodeLabelyStrLenMax = 64;

constexpr uint32_t BorderColorDefault = DefaultBlackColor;

constexpr uint32_t BorderWidthDefault = DefaultLineWidth;
constexpr uint32_t BorderWidthMin = 0;
constexpr uint32_t BorderWidthMax = 12;

//
//  The DashString format for BorderDash property and EdgeDash property.
//  DashString  ::= [DoubleQuote] [DashUnit] [SpaceChar DashUnit]+ [DoubleQuote]
//  DoubleQuote ::= ["]
//	SpaceChar   ::= [ox20]+
//	DashUnit    ::= [0-9]+
//
#define BorderDashDefault ""
constexpr uint32_t BorderDashStringLengthMin = 1;
constexpr uint32_t BorderDashStringLengthMax = 255;

constexpr uint32_t FillColorDefault = 0;

constexpr uint32_t EdgeColorDefault = DefaultBlackColor;

constexpr uint32_t EdgeWidthDefault = DefaultLineWidth;
constexpr uint32_t EdgeWidthMin = 0;
constexpr uint32_t EdgeWidthMax = 12;

#define EdgeDashDefault ""
constexpr uint32_t EdgeDashStringLengthMin = 1;
constexpr uint32_t EdgeDashStringLengthMax = 255;

constexpr uint32_t FontColorDefault = DefaultBlackColor;

//
// These properties may not work, if you want change any of them of your rendering result,
// for the best result, you'd better using these combined properties , such as:
// [FontFamily="your font name", FontWeight=0,FontSlant=0];
// [FontFamily="your font name", FontWeight=0,FontSlant=1];
// [FontFamily="your font name", FontWeight=1,FontSlant=0];
// [FontFamily="your font name", FontWeight=1,FontSlant=1];
// 
#define FontFamilyDefault ""
constexpr uint32_t FontFamilyStrLenMin = 1;
constexpr uint32_t FontFamilyStrLenMax = 64;
constexpr uint32_t FontWeightDefault = 0;
constexpr uint32_t FontWeightMin = 0;
constexpr uint32_t FontWeightMax = 1;
constexpr uint32_t FontSlantDefault = 0;
constexpr uint32_t FontSlantMin = 0;
constexpr uint32_t FontSlantMax = 1;

constexpr uint32_t FontSizeDefault = 10;
constexpr uint32_t FontSizeMin = 10;
constexpr uint32_t FontSizeMax = 36;

constexpr uint32_t RadiusIncrementDefault = 1;
constexpr uint32_t RadiusIncrementMin = 0;
constexpr uint32_t RadiusIncrementMax = 360;

constexpr uint32_t EdgeXIncrementDefault = 5;
constexpr uint32_t EdgeXIncrementMin = 0;
constexpr uint32_t EdgeXIncrementMax = 360;

constexpr uint32_t EdgeYIncrementDefault = 5;
constexpr uint32_t EdgeYIncrementMin = 0;
constexpr uint32_t EdgeYIncrementMax = 360;

constexpr uint32_t PageBackgroundColorDefault = 32;
constexpr uint32_t PageBackgroundColorMin = 0;
constexpr uint32_t PageBackgroundColorMax = UINT32_MAX;

constexpr uint32_t PageMarginXIncrementDefault = 0;
constexpr uint32_t PageMarginXIncrementMin = 0;
constexpr uint32_t PageMarginXIncrementMax = 255;

constexpr uint32_t PageMarginYIncrementDefault = 0;
constexpr uint32_t PageMarginYIncrementMin = 0;
constexpr uint32_t PageMarginYIncrementMax = 255;
// Property-define-end.

//
// Macro defines----------------------------
//
#ifndef USING_BITREE_PRINT
#	include <cstdio>
#	define USING_BITREE_PRINT
#endif

#ifdef USING_BITREE_PRINT
#define PrintInfo(fmt, ...)                                                                        \
	do                                                                                             \
	{																							   \
		fprintf(stdout, "[info]: " fmt, ##__VA_ARGS__);                                             \
	} while(0)
#endif

#ifdef USING_BITREE_PRINT
#define PrintError(fmt, ...)                                                                       \
	do                                                                                             \
	{                                                                                              \
		fprintf(stdout, "[error]: " fmt, ##__VA_ARGS__);                                             \
	} while(0)
#endif

#ifndef USING_BITREE_ASSERT_AND_DEBUG
#	include <cassert>
#   include <typeinfo>
#	define USING_BITREE_ASSERT_AND_DEBUG
#endif

#ifdef USING_BITREE_ASSERT_AND_DEBUG
#	define BiTreeAssert(x) assert(x)
#else
#	define BiTreeAssert(x)
#endif

using str_ptr_t = std::string::const_iterator;

/**
 * @brief Holding a BitreeNode's all property values.
 */
class NodeInfo
{
public:
	/**
	 * @brief One element store one unique property.
	 */
	struct element_t
	{
		uint32_t index; ///< Property-index of propertyTable of BiTreePropertyManager.
		mutable std::any value; ///< Hold any property-value.

		bool operator<(const element_t& other) const
		{
			return this->index < other.index;
		}

		element_t()
			: index(UINT32_MAX){};

		element_t(const uint32_t& index, const std::any& value)
			: index(index)
			, value(value)
		{ }
	}; //Element-define-end.

private:
	uint32_t mNodeId; // Unique nodeId of a BiTreeNode.
	mutable std::set<element_t> mElementSet; 	// All properties.
	mutable uint32_t mElementSetSizeCurrent;	// Just for speeding-up when getting mElementSet size.
	static const uint32_t mElementSetSizeMax = 32;

public:
	const std::set<element_t>& getElementSet()const
	{
		return mElementSet;
	}
    const uint32_t& getNodeId()const
	{
		return mNodeId;
	}

	void setNodeId(const uint32_t& nodeId)
	{
		mNodeId=nodeId;
	}

	void clear()
	{
		mNodeId=UINT32_MAX;
		mElementSet.clear();
		mElementSetSizeCurrent=0;
	}

	uint32_t& getElementSetSizeCurrentRef()
	{
		return mElementSetSizeCurrent;
	}

	bool empty() const
	{
		return mElementSet.empty();
	}

	bool insert(const element_t& element)
	{
		if(mElementSetSizeCurrent < mElementSetSizeMax)
		{
			++mElementSetSizeCurrent;
			mElementSet.insert(element);

			return true;
		}

		PrintError("ElementSet size overflow, [mElementSetSizeMax]=%d.\n",mElementSetSizeMax);
		return false;
	}
	bool operator<(const NodeInfo& other) const
	{
		return this->mNodeId < other.mNodeId;
	}

	NodeInfo(const uint32_t& nodeId=UINT32_MAX)
		: mNodeId(nodeId),mElementSetSizeCurrent(0){};
}; // NodeInfo-define-end

/**
 * @brief Holding all nodeinfo of a bitree.
 */
class NodeInfoArray
{
private:
	std::set<NodeInfo> mNodeInfoSet;
    mutable uint32_t mNodeInfoSetSizeCurrent; // Just for speeding-up when getting size of nodeInfoSet.
	static const uint32_t mNodeInfoSetSizeMax = NodeIdMax;

public:
	NodeInfoArray()
	:mNodeInfoSetSizeCurrent(0)
	{ }
	bool
	insertNodeInfo(const uint32_t& nodeId, const uint32_t& elementIndex, const std::any& elementValue)
	{
		NodeInfo nodeInfo(nodeId);
		NodeInfo::element_t element(elementIndex, elementValue);

		auto it1 = mNodeInfoSet.find(nodeInfo);
		if(it1 != mNodeInfoSet.end())
		{
			return const_cast<NodeInfo&>(*it1).insert(element);
		}
		else
		{
			// Not find this nodeInfo, insert new NodeInfo.
			nodeInfo.insert(element);
			return this->insertNodeInfo(nodeInfo);
		}
	}

	bool insertNodeInfo(const NodeInfo& nodeInfo)
	{
		if(mNodeInfoSetSizeCurrent<mNodeInfoSetSizeMax)
		{
			++mNodeInfoSetSizeCurrent;
			mNodeInfoSet.insert(nodeInfo);
			return true;
		}
		PrintError("mNodeInfoSet overflow, [mNodeInfoSetSizeMax]=%d\n",mNodeInfoSetSizeMax);

		return false;
	}

	const NodeInfo& findNodeInfo(const std::string& NodeIdStr)const
	{
		static NodeInfo tempNodeInfo;
		uint32_t inputNodeId;
		bool good=true;

		try
		{
			inputNodeId=(uint32_t)std::stoul(NodeIdStr,nullptr,0);
		}
		catch(...)
		{
			good=false;
		}
	
		if(good)
		{
			tempNodeInfo.setNodeId(inputNodeId);
			auto it=mNodeInfoSet.find(tempNodeInfo);
			if(it!=mNodeInfoSet.end())
			{
				return *it;
			}
		}
		
		tempNodeInfo.setNodeId(NodeIdInvalid);

		return tempNodeInfo;
	}

}; //NodeInfoArray-define-end

//
// Math constants---------------------------
//
constexpr double Sin60 = 0.86602540378443864676372317075294;
constexpr double Sin30 = 0.5;
constexpr double Angle90 = 0.5 * 3.1415926535897932384626433832795;
constexpr double Angle180 = 3.1415926535897932384626433832795;
constexpr double Angle360 = 2 * 3.1415926535897932384626433832795;

} // namespace bitree
