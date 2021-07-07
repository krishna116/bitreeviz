/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bitreeviz/BiTreePropertyManager.h"

using namespace bitree;

//
// Property value checke functions, each property has a check function.
//
static bool CheckDefaultUint32(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(v <= UINT32_MAX)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckNodeId(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(v <= NodeIdMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckNodeLabel(const std::string& inputValue, std::any& outputValue)
{
	auto size = inputValue.size();
	if(NodeLabelStrLenMin <= size && size <= NodeLabelyStrLenMax)
	{
		outputValue = inputValue;
		return true;
	}

	return false;
}

static bool CheckNodeRadius(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(NodeRadiusMin <= v && v <= NodeRadiusMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckBorderWidth(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(BorderWidthMin <= v && v <= BorderWidthMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckBorderDash(const std::string& inputValue, std::any& outputValue)
{
	auto size = inputValue.size();
	if(BorderDashStringLengthMin <= size && size <= BorderDashStringLengthMax)
	{
		outputValue = inputValue;
		return true;
	}

	return false;
}

static bool CheckEdgeWidth(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(EdgeWidthMin <= v && v <= EdgeWidthMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckEdgeDash(const std::string& inputValue, std::any& outputValue)
{
	auto size = inputValue.size();
	if(EdgeDashStringLengthMin <= size && size <= EdgeDashStringLengthMax)
	{
		outputValue = inputValue;
		return true;
	}

	return false;
}

static bool CheckFontSize(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(FontSizeMin <= v && v <= FontSizeMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckFontWeight(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(FontWeightMin <= v && v <= FontWeightMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckFontFamily(const std::string& inputValue, std::any& outputValue)
{
	auto size = inputValue.size();
	if(FontFamilyStrLenMin <= size && size <= FontFamilyStrLenMax)
	{
		outputValue = inputValue;
		return true;
	}

	return false;
}

static bool CheckFontSlant(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(FontSlantMin <= v && v <= FontSlantMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckRadiusIncrement(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(RadiusIncrementMin <= v && v <= RadiusIncrementMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckEdgeXIncrement(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(EdgeXIncrementMin <= v && v <= EdgeXIncrementMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckEdgeYIncrement(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(EdgeYIncrementMin <= v && v <= EdgeYIncrementMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckPageBackgroundColor(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(PageBackgroundColorMin <= v && v <= PageBackgroundColorMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckPageMarginXIncrement(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(PageMarginXIncrementMin <= v && v <= PageMarginXIncrementMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

static bool CheckPageMarginYIncrement(const std::string& inputValue, std::any& outputValue)
{
	auto v = std::stoul(inputValue, nullptr, 0);
	if(PageMarginYIncrementMin <= v && v <= PageMarginYIncrementMax)
	{
		outputValue = (uint32_t)v;
		return true;
	}

	return false;
}

//
// Sender functions. it can use cached nodeInfo or user-input to update current node-properties.
//
static void SendNodeLabel(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.nodeLabel = std::any_cast<std::string>(element.value);
}

static void SendNodeRadius(const NodeInfo::element_t& element,
							BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.nodeRadius = std::any_cast<uint32_t>(element.value);
}


static void SendBorderColor(const NodeInfo::element_t& element,
							BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.borderColor = std::any_cast<uint32_t>(element.value);
}

static void SendBorderWidth(const NodeInfo::element_t& element,
							BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.borderWidth = std::any_cast<uint32_t>(element.value);
}

static void SendBorderDash(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.borderDash = std::any_cast<std::string>(element.value);
}

static void SendFillColor(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.fillColor = std::any_cast<uint32_t>(element.value);
}
static void SendEdgeColor(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.edgeColor = std::any_cast<uint32_t>(element.value);
}
static void SendEdgeWidth(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.edgeWidth = std::any_cast<uint32_t>(element.value);
}

static void SendEdgeDash(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.edgeDash = std::any_cast<std::string>(element.value);
}

static void SendFontColor(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeBaseProperty.fontColor = std::any_cast<uint32_t>(element.value);
}

static void SendFontFamily(const NodeInfo::element_t& element,
						   BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.fontFamily = std::any_cast<std::string>(element.value);
}

static void SendFontSize(const NodeInfo::element_t& element,
						 BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.fontSize = std::any_cast<uint32_t>(element.value);
}

static void SendFontSlant(const NodeInfo::element_t& element,
						  BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.fontSlant = std::any_cast<uint32_t>(element.value);
}
static void SendFontWeight(const NodeInfo::element_t& element,
						   BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.fontWeight = std::any_cast<uint32_t>(element.value);
}
static void SendRadiusIncrement(const NodeInfo::element_t& element,
								BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.radiusIncrement = std::any_cast<uint32_t>(element.value);
}
static void SendEdgeXIncrement(const NodeInfo::element_t& element,
							   BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.edgeXIncrement = std::any_cast<uint32_t>(element.value);
}
static void SendEdgeYIncrement(const NodeInfo::element_t& element,
							   BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.edgeYIncrement = std::any_cast<uint32_t>(element.value);
}

static void SendPageBackgroundColor(const NodeInfo::element_t& element,
									BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.pageBackgroundColor = std::any_cast<uint32_t>(element.value);
}

static void SendPageMarginXIncrement(const NodeInfo::element_t& element,
									BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.pageMarginXIncrement = std::any_cast<uint32_t>(element.value);
}

static void SendPageMarginYIncrement(const NodeInfo::element_t& element,
									BiTreePropertyManager::NodeProperty& nodeProperty)
{
	nodeProperty.nodeExtendProperty.pageMarginYIncrement = std::any_cast<uint32_t>(element.value);
}

typedef BiTreePropertyManager::PropertyRecord::ElementValueType vt;
const std::vector<BiTreePropertyManager::PropertyRecord> BiTreePropertyManager::propertyTable = {
	// Node base property.
	{getNextId(), 	"NodeLabel", 			vt::String, 0, 0, CheckNodeLabel, 			SendNodeLabel},
	{getNextId(),	"NodeRadius",			vt::Uint32,	0, 0, CheckNodeRadius,			SendNodeRadius},
	{getNextId(), 	"BorderColor",			vt::Uint32, 0, 0, CheckDefaultUint32, 		SendBorderColor},
	{getNextId(), 	"BorderWidth", 			vt::Uint32, 0, 0, CheckBorderWidth, 		SendBorderWidth},
	{getNextId(), 	"BorderDash", 			vt::String, 0, 0, CheckBorderDash, 			SendBorderDash},
	{getNextId(), 	"FillColor", 			vt::Uint32, 0, 0, CheckDefaultUint32, 		SendFillColor},
	{getNextId(), 	"EdgeColor", 			vt::Uint32, 0, 0, CheckDefaultUint32, 		SendEdgeColor},
	{getNextId(), 	"EdgeWidth", 			vt::Uint32, 0, 0, CheckEdgeWidth, 			SendEdgeWidth},
	{getNextId(), 	"EdgeDash", 			vt::String, 0, 0, CheckEdgeDash, 			SendEdgeDash},
	{getNextId(), 	"FontColor", 			vt::Uint32, 0, 0, CheckDefaultUint32, 		SendFontColor},

	// Node extend property, public used only.
	{getNextId(), 	"FontFamily", 			vt::String, 1, 0, CheckFontFamily, 			SendFontFamily},
	{getNextId(), 	"FontSize", 			vt::Uint32, 1, 0, CheckFontSize, 			SendFontSize},
	{getNextId(), 	"FontSlant", 			vt::Uint32, 1, 0, CheckFontSlant, 			SendFontSlant},
	{getNextId(), 	"FontWeight", 			vt::Uint32, 1, 0, CheckFontWeight, 			SendFontWeight},
	{getNextId(), 	"RadiusIncrement", 		vt::Uint32, 1, 0, CheckRadiusIncrement, 	SendRadiusIncrement},
	{getNextId(), 	"EdgeXIncrement", 		vt::Uint32, 1, 0, CheckEdgeXIncrement, 		SendEdgeXIncrement},
	{getNextId(), 	"EdgeYIncrement", 		vt::Uint32, 1, 0, CheckEdgeYIncrement, 		SendEdgeYIncrement},
	{getNextId(), 	"PageBackgroundColor", 	vt::Uint32, 1, 0, CheckPageBackgroundColor, SendPageBackgroundColor},
	{getNextId(), 	"PageMarginXIncrement", vt::Uint32, 1, 0, CheckPageMarginXIncrement, SendPageMarginXIncrement},
	{getNextId(), 	"PageMarginYIncrement", vt::Uint32, 1, 0, CheckPageMarginYIncrement, SendPageMarginYIncrement}
	};

const std::size_t BiTreePropertyManager::propertyTableSize =
	BiTreePropertyManager::propertyTable.size();

BiTreePropertyManager::NodeProperty BiTreePropertyManager::mCurrentNodeProperty =
	BiTreePropertyManager::NodeProperty();
BiTreePropertyManager::NodeProperty BiTreePropertyManager::mCurrentNodePropertyOverride =
	BiTreePropertyManager::NodeProperty();

bool BiTreePropertyManager::isNodeIdValid(const uint32_t& nodeId)
{
	if(nodeId <= NodeIdMax)
	{
		return true;
	}

	return false;
}

bool BiTreePropertyManager::isNodeIdValid(const std::string& nodeIdStr, uint32_t& nodeId)
{
	auto v = std::stoul(nodeIdStr, nullptr, 0);
	if(v <= NodeIdMax)
	{
		nodeId = v;
		return true;
	}

	return false;
}

bool BiTreePropertyManager::isPropertyNameValid(const std::string& name,
												PropertyRecord& outputPropertyRecord)
{
	for(auto const& propertyRecord : propertyTable)
	{
		if(propertyRecord.propertyName == name)
		{
			outputPropertyRecord = propertyRecord;
			return true;
		}
	}

	return false;
}

bool BiTreePropertyManager::isPropertyValueValid(const PropertyRecord& inputPropertyRecord,
												 const std::string& valueString,
												 std::any& valueOutput)
{
	return inputPropertyRecord.checkValue(valueString, valueOutput);
}

BiTreePropertyManager::PropertyRecord::ElementValueType
BiTreePropertyManager::getElementValueType(const uint32_t& elementIndex)
{
	BiTreeAssert(elementIndex < propertyTableSize);

	return propertyTable[elementIndex].valueType;
}

bool BiTreePropertyManager::getPropertyRecordBy(
	const std::string& prpoertyNameStr, BiTreePropertyManager::PropertyRecord& propertyRecord)
{
	for(auto const& thisRecord : propertyTable)
	{
		if(thisRecord.propertyName == prpoertyNameStr)
		{
			propertyRecord = thisRecord;
			return true;
		}
	}

	return false;
}

const BiTreePropertyManager::NodeProperty&
BiTreePropertyManager::getCurrentPropertyByUpdate(const NodeInfo& nodeInfo)
{
	mCurrentNodeProperty = mCurrentNodePropertyOverride;

	for(auto const& element : nodeInfo.getElementSet())
	{
		BiTreeAssert(element.index < propertyTableSize);
		propertyTable[element.index].sender(element, mCurrentNodeProperty);
	}

	return mCurrentNodeProperty;
}

const BiTreePropertyManager::NodeProperty& BiTreePropertyManager::getCurrentProperty()
{
	return mCurrentNodePropertyOverride;
}

bool BiTreePropertyManager::setCurrentProperty(
	const BiTreePropertyManager::PropertyRecord& propertyRecord,
	const std::any& value,
	bool globalOverride)
{
	BiTreeAssert(propertyRecord.elementIndexOrId < propertyTableSize);

	NodeInfo::element_t element;
	element.index = propertyRecord.elementIndexOrId;
	element.value = value;

	propertyRecord.sender(element, mCurrentNodeProperty);

	if(globalOverride)
	{
		propertyTable[propertyRecord.elementIndexOrId].isPropertyGlobalOverrided = globalOverride;
		propertyRecord.sender(element, mCurrentNodePropertyOverride);
	}

	return true;
}
