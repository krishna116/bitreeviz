/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "BiTreeDefines.h"

#include <any>
#include <string>
#include <vector>

namespace bitree
{
/**
 * @brief Manage all the nodes's property type info.
 * It can be used to check if the property is valid and cache it.
 * It can be used to convert cached NodeInfo to currentNodeInfo for rendering.
 */
class BiTreePropertyManager
{
public:
	struct NodeBaseProperty
	{
		NodeBaseProperty()
			: nodeRadius(NodeRadiusDefault)
			, borderColor(BorderColorDefault)
			, borderWidth(BorderWidthDefault)
			, fillColor(FillColorDefault)
			, edgeColor(EdgeColorDefault)
			, edgeWidth(EdgeWidthDefault)
			, edgeDash(EdgeDashDefault)
			, fontColor(FontColorDefault)
		{ }
		std::string nodeLabel;
		uint32_t nodeRadius;
		uint32_t borderColor;
		uint32_t borderWidth;
		std::string borderDash;
		uint32_t fillColor;
		uint32_t edgeColor;
		uint32_t edgeWidth;
		std::string edgeDash;
		uint32_t fontColor;
	};

	struct NodeProperty
	{
		NodeProperty()
			: nodeBaseProperty(NodeBaseProperty())
			, nodeExtendProperty(NodeExtendProperty())
		{ }

		NodeBaseProperty nodeBaseProperty;
		struct NodeExtendProperty
		{
			NodeExtendProperty()
				: fontSize(FontSizeDefault)
				, fontSlant(FontSlantDefault)
				, fontWeight(FontWeightDefault)
				, radiusIncrement(RadiusIncrementDefault)
				, edgeXIncrement(EdgeXIncrementDefault)
				, edgeYIncrement(EdgeYIncrementDefault)
				, pageBackgroundColor(PageBackgroundColorDefault)
				, pageMarginXIncrement(PageMarginXIncrementDefault)
				, pageMarginYIncrement(PageMarginYIncrementDefault)
			{ }
			std::string fontFamily;
			uint32_t fontSize;
			uint32_t fontSlant;
			uint32_t fontWeight;
			uint32_t radiusIncrement;
			uint32_t edgeXIncrement;
			uint32_t edgeYIncrement;
			uint32_t pageBackgroundColor;
			uint32_t pageMarginXIncrement;
			uint32_t pageMarginYIncrement;
		} nodeExtendProperty;
	};

	typedef bool (*CheckValueFn)(const std::string& inputValue, std::any& outputValue);
	typedef void (*SendPropertyToNodeArrayFn)(const NodeInfo::element_t& element,
											  NodeProperty& nodeProperty);

	struct PropertyRecord
	{
		enum class ElementValueType
		{
			Uint32,
			String
		};

		uint32_t elementIndexOrId;
		std::string propertyName;
		ElementValueType valueType;
		bool isExtendProperty;
		mutable bool isPropertyGlobalOverrided;
		BiTreePropertyManager::CheckValueFn checkValue;
		BiTreePropertyManager::SendPropertyToNodeArrayFn sender;
	};

public:
	BiTreePropertyManager(){};

	/**
	 * @brief Mapping input value to mCurrentNodeProperty and return it.
	 * 
	 * @param propertyRecord 	The value's typeinfo.
	 * @param value 			The value to mapping to the NodeProperty.
	 * @return NodeProperty 	Mapped NodeProperty. 
	 */
	static bool setCurrentProperty(const PropertyRecord& propertyRecord,
								   const std::any& value,
								   bool globalOverride = false);

	/**
	 * @brief Mapping input nodeInfo to mCurrentNodeProperty and return it.
	 * 
	 * @param nodeInfo 			The NodeInfo to mapping to the NodeProperty.
	 * @return NodeProperty 	Mapped NodeProperty. 
	 */
	static const NodeProperty& getCurrentPropertyByUpdate(const NodeInfo& nodeInfo);

	/**
	 * @brief Get default NodeProperty
	 * 
	 * @return NodeProperty		Default NodeProperty
	 */
	static const NodeProperty& getCurrentProperty();

	/**
	 * @brief Check node id.
	 * 
	 * @param nodeId Input node id.
	 * @return true 	Valid.
	 * @return false 	Invalid.
	 */
	static bool isNodeIdValid(const uint32_t& nodeId);

	/**
	 * @brief Check node id.
	 * 
	 * @param nodeIdStr Input node id string.
	 * @param nodeId 	Return node id uint32 if It is valid.
	 * @return true 	Vaild
	 * @return false 	Invalid.
	 */
	static bool isNodeIdValid(const std::string& nodeIdStr, uint32_t& nodeId);

	/**
	 * @brief Check property name.
	 * 
	 * @param name 	Input property name string.
	 * @param outputPropertyRecord Out put the property typeinfo if it is valid.
	 * @return true 	Valid.
	 * @return false 	Invalid.
	 */
	static bool isPropertyNameValid(const std::string& name, PropertyRecord& outputPropertyRecord);

	/**
	 * @brief Check property value.
	 * 
	 * @param inputPropertyRecord Input the property typeinfo.
	 * @param valueString 	Input the property value.
	 * @param valueOutput 	Output value that can be cached if it is valid value.
	 * @return true 		Valid.
	 * @return false 		Invalid.
	 */
	static bool isPropertyValueValid(const PropertyRecord& inputPropertyRecord,
									 const std::string& valueString,
									 std::any& valueOutput);
	/**
	 * @brief Get the Element value type by elementIndex.
	 * Usually the elementIndex mapping to propertyTable-index.
	 * 
	 * @param elementIndex 	Input element index.
	 * @return PropertyRecord::ElementValueType 	It is also the property's value type.
	 */
	static PropertyRecord::ElementValueType getElementValueType(const uint32_t& elementIndex);

	/**
	 * @brief Input a property name string, then get it's infomation.
	 * 
	 * @param prpoertyNameStr Input property name string.
	 * @param propertyRecord  This Property's info or typeinfo if it is a valid property name.
	 * @return true 	Valid.
	 * @return false 	Invalid.
	 */
	static bool getPropertyRecordBy(const std::string& prpoertyNameStr,
									PropertyRecord& propertyRecord);

private:
	static const std::vector<PropertyRecord> propertyTable;
	static const std::size_t propertyTableSize;

	static NodeProperty mCurrentNodeProperty;
	static NodeProperty mCurrentNodePropertyOverride;

	static uint32_t getNextId()
	{
		static uint32_t i = 0;
		return i++;
	};
}; // BiTreePropertyManager-define-end
} // namespace bitree
