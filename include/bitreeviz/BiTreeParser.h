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
#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace bitree
{
/**
 * @brief Used to parsing bitree string to bitree structure.
 */
class BiTreeParser
{
public:
	struct BiTreeNode;
	class BiTreePropertyParser
	{
	public:
		BiTreePropertyParser();
		~BiTreePropertyParser();
	
	/**
	 * @brief Parsing a property line.
	 * 
	 * @param begin Begin of a line.
	 * @param end 	End of a line.
	 * @return true 	Parsing pass.
	 * @return false 	Parsing failed.
	 */
		bool parsing(str_ptr_t begin, str_ptr_t end);

	/**
	 * @brief Can be used to get all cached node property info after parsing.
	 * 
	 * @return NodeInfoArray Parsed NodeInfoArray. 
	 */
		const NodeInfoArray* getNodeInfoArray() const;

	private:
		NodeInfoArray* mNodeInfoArray = nullptr;
		static const std::size_t maxTokenStringSizeAllowed = 255;

		bool is_char_09(const char& ch);

		bool is_char_az_AZ(const char& ch);

		bool is_char_az_AZ_09(const char& ch);

		str_ptr_t trimFrontSpace(str_ptr_t begin, str_ptr_t end);

		str_ptr_t trimFrontSpaceAndComma(str_ptr_t begin, str_ptr_t end);
		str_ptr_t trimFrontSpaceAndCommaAndRestComment(str_ptr_t begin, str_ptr_t end);

		str_ptr_t getNodeId(str_ptr_t begin, str_ptr_t end, bool& status, std::string& nodeId);

		str_ptr_t
		getPropertyName(str_ptr_t begin, str_ptr_t end, bool& status, std::string& propertyName);

		str_ptr_t trimLpSquare(str_ptr_t begin, str_ptr_t end, bool& status);

		str_ptr_t trimRpSquare(str_ptr_t begin, str_ptr_t end, bool& status);

		str_ptr_t trimEq(str_ptr_t begin, str_ptr_t end, bool& status);

		str_ptr_t getValueString(str_ptr_t begin, str_ptr_t end, bool& status, std::string& value);

		str_ptr_t getValueUint32(str_ptr_t begin, str_ptr_t end, bool& status, std::string& value);
	}; //BiTreePropertyParser-define-end

public:
	BiTreeParser();
	~BiTreeParser();

	/**
	 * @brief Main parsing function.
	 * 
	 * @param text The raw text of bitree.
	 * @return true Parsing ok.
	 * @return false Parsing failed.
	 */
	bool parsing(const std::string& text);

	/**
	 * @brief Get parsed max_token_length of a node.
	 * 
	 * @return const std::size_t& 	Max token length.
	 */
	const std::size_t& getMaxTokenSizeCurrent();

	/**
	 * @brief check if this parser contain a parsed-bitree.
	 * 
	 * @return true 	Has a parsed a bitree.
	 * @return false 	Has empty bitree.
	 */
	bool empty();

	/**
	 * @brief Get the internal bitree's root.
	 * 
	 * @return BiTreeNode*	Bitree's root node.
	 */
	BiTreeNode* getRoot();
	
	/**
	 * @brief PropertyParser should be used after the BiTreeParser work pass.
	 * 
	 * @return BiTreePropertyParser*	This BiTreePropertyParser.
	 */
	BiTreePropertyParser* getPropertyParser();
	
	/**
	 * @brief It stores BiTreePropertyParser work's result.
	 * 
	 * @return const NodeInfoArray* 	Array of NodeInfo.
	 */
	const NodeInfoArray* getNodeInfoArray() const;

private:
	BiTreeNode* mRoot = nullptr;
	BiTreePropertyParser* mPropertyParser = nullptr;

	static std::size_t maxTokenSizeCurrent;
	static const std::size_t maxTokenSizeAllowed;

	enum class TokenName
	{
		Unknown = 0,
		Epsilon,
		Eof,
		Node,
		Lp,
		Rp,
		Comma,
		Int
	};

	using value_t = std::string;
	struct Token
	{
		TokenName name;
		value_t value;
		Token()
			: name(TokenName::Unknown)
		{ }
		Token(const TokenName& name)
		{
			this->name = name;
		}
		Token(const TokenName& name, const value_t& value)
		{
			this->name = name;
			this->value = value;
		}
		bool operator==(const Token& other) const
		{
			if(this->name == other.name)
			{
				return true;
			}
			return false;
		}
	}; // Token-define-end

	enum class ActionType
	{
		Error,
		PushProduction1,
		PushProduction2,
		PopStack,
		PopStackAndMoveIt
	};

	std::string getTokenNameStr(const Token& token)
	{
		static const std::vector<std::string> mTokenArrayBuffer = {
			"Unknown", "Epsilon", "$", "T", "(", ")", ",", "i"};
		static const uint32_t mTokenArrayBufferSize=mTokenArrayBuffer.size();
		
		BiTreeAssert((uint32_t)(token.name)<mTokenArrayBufferSize);

		return mTokenArrayBuffer[(uint32_t)(token.name)];
	}

	using TokenArray = std::vector<Token>;
	TokenArray getTokenArray(const std::string& text);
	ActionType action(const Token& token, const Token& token1, const Token& token2);
	void freeBiTree(BiTreeNode** root);

public:

	/**
	 * @brief BiTreeNode define.
	 */
	struct BiTreeNode
	{
		BiTreeNode* leftChild; ///< Left child of this node;
		BiTreeNode* rightChild; ///< Right child of this node;
		std::string data;


		// Follow properties will be runtime generated when do layout work.
		double x; ///< X-coordinate.
		double y; ///< Y-coordinate.
		double subTreeOffsetX; ///< Offset of this tree.
		BiTreeNode* parent; ///< Point to parent.

		BiTreeNode(const std::string& data)
		{
			this->data = data;
			this->parent = this->leftChild = this->rightChild = nullptr;
			x = y = subTreeOffsetX = 0.0f;
		}

		std::string dataToString()
		{
			return data;
		}

		bool isDataDecimalOrHexNumber()
		{
			bool isDigitNumber = false;
			auto begin = this->data.begin();
			auto end = this->data.end();

			if(begin<end && (begin+1)<end && *begin=='0' && (*(begin+1)=='x' ||*(begin+1)=='X'))
			{
				begin+=2;
			}

			while(begin<end && ('0'<=*begin && *begin<='9' || 'a'<=*begin && *begin<='f' || 'A'<=*begin && *begin<='F'))
			{
				++begin;
				isDigitNumber=true;
			}

			if(begin<end)
			{
				isDigitNumber=false;
			}

			return isDigitNumber;
		}
	}; //BiTreeNode-define-end.
}; // BiTreeParser=define-end
} // namespace bitree
