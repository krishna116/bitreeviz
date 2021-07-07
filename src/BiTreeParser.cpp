/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bitreeviz/BiTreeParser.h"
#include "bitreeviz/BiTreePropertyManager.h"

#include <iostream>
#include <stack>

using namespace bitree;

const std::size_t BiTreeParser::maxTokenSizeAllowed = 256;
std::size_t BiTreeParser::maxTokenSizeCurrent = 0;

bool BiTreeParser::BiTreePropertyParser::is_char_09(const char& ch)
{
	if('0' <= ch && ch <= '9')
	{
		return true;
	}
	return false;
}

bool BiTreeParser::BiTreePropertyParser::is_char_az_AZ(const char& ch)
{
	if('a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z')
	{
		return true;
	}
	return false;
}

bool BiTreeParser::BiTreePropertyParser::is_char_az_AZ_09(const char& ch)
{
	if(is_char_09(ch) || is_char_az_AZ(ch))
	{
		return true;
	}
	return false;
}

str_ptr_t BiTreeParser::BiTreePropertyParser::trimFrontSpace(str_ptr_t begin, str_ptr_t end)
{
	while(begin < end && *begin <= 0x20)
		++begin;
	return begin;
}

str_ptr_t BiTreeParser::BiTreePropertyParser::trimFrontSpaceAndComma(str_ptr_t begin, str_ptr_t end)
{
	begin = trimFrontSpace(begin, end);
	if(begin < end && (*begin == ',' || *begin == ';' || *begin==' '))
	{
		++begin;
	}
	return trimFrontSpace(begin, end);
}

str_ptr_t 
BiTreeParser::BiTreePropertyParser::trimFrontSpaceAndCommaAndRestComment(str_ptr_t begin, str_ptr_t end)
{
	begin=trimFrontSpaceAndComma(begin,end);
	if(begin<end && ( *begin=='/' || *begin=='#'))
	{
		begin=end;
	}
	return begin;
}

str_ptr_t
BiTreeParser::BiTreePropertyParser::getNodeId(str_ptr_t begin, str_ptr_t end, bool& status, std::string& nodeId)
{
	nodeId.clear();
	uint32_t i=0;

	begin = trimFrontSpace(begin, end);
	while(begin < end)
	{
		if(is_char_09(*begin))
		{
			nodeId.push_back(*begin);
			++begin;

			if(++i>maxTokenStringSizeAllowed)
			{
				PrintError("Find a token overflow, maxTokenStringSizeAllowed: %d\n",maxTokenStringSizeAllowed);
				nodeId.clear();
				break;
			}
		}
		else
		{
			break;
		}
	}

	nodeId.empty() ? (status = false) : (status = true);
	return begin;
}

str_ptr_t BiTreeParser::BiTreePropertyParser::getPropertyName(str_ptr_t begin,
												str_ptr_t end,
												bool& status,
												std::string& propertyName)
{
	propertyName.clear();
	uint32_t i=0;
	begin = trimFrontSpace(begin, end);
	while(begin < end)
	{
		if(is_char_az_AZ_09(*begin))
		{
			propertyName.push_back(*begin);
			++begin;

			if(++i>maxTokenStringSizeAllowed)
			{
				PrintError("Find a token overflow, maxTokenStringSizeAllowed: %d\n",maxTokenStringSizeAllowed);
				propertyName.clear();
				break;
			}
		}
		else
		{
			break;
		}
	}

	propertyName.empty() ? (status = false) : (status = true);
	return begin;
}

str_ptr_t BiTreeParser::BiTreePropertyParser::trimLpSquare(str_ptr_t begin, str_ptr_t end, bool& status)
{
	status = false;
	begin = trimFrontSpace(begin, end);

	if(begin < end && *begin == '[')
	{
		status = true;
		++begin;
	}

	return begin;
}


str_ptr_t
BiTreeParser::BiTreePropertyParser::trimRpSquare(str_ptr_t begin, str_ptr_t end, bool& status)
{
	status = false;
	begin = trimFrontSpace(begin, end);

	if(begin < end && *begin == ']')
	{
		status = true;
		++begin;
	}

	return begin;
}

str_ptr_t 
BiTreeParser::BiTreePropertyParser::trimEq(str_ptr_t begin, str_ptr_t end, bool& status)
{
	status = false;
	begin = trimFrontSpace(begin, end);
	if(begin < end && *begin == '=')
	{
		++begin;
		status = true;
	}

	return begin;
}

str_ptr_t 
BiTreeParser::BiTreePropertyParser::getValueString(str_ptr_t begin,
											   str_ptr_t end,
											   bool& status,
											   std::string& value)
{
	value.clear();
	uint32_t i=0;
	status = false;
	begin = trimFrontSpaceAndComma(begin, end);

	if(begin < end && *begin == '\"') // Token start with ["]
	{
		++begin;
		if(begin < end)
		{
			while(begin < end && *begin != '\"' && *begin >= 0x20 && *begin <= 0x7e)
			{

				value.push_back(*begin);
				++begin;

				if(++i>maxTokenStringSizeAllowed)
				{
					PrintError("Find a token overflow, maxTokenStringSizeAllowed: %d\n",maxTokenStringSizeAllowed);
					status=false;
					break;
				}
			}

			if(begin < end && *begin == '\"')
			{
				status = true;
				++begin;
			}
		}
	}
	else if(begin<end) // Token start with a visible char except ["].
	{
		while(begin<end &&*begin > 0x20 && *begin!=',' && *begin!=']' && *begin!='"' )
		{
				value.push_back(*begin);
				++begin;

				status = true;
				if(++i>maxTokenStringSizeAllowed)
				{
					PrintError("Find a token overflow, maxTokenStringSizeAllowed: %d\n",maxTokenStringSizeAllowed);
					status=false;
					break;
				}
		}
	}

	return begin;
}

str_ptr_t BiTreeParser::BiTreePropertyParser::getValueUint32(str_ptr_t begin,
											   str_ptr_t end,
											   bool& status,
											   std::string& value)
{
	status = false;
	value.clear();
	bool isDecimal = true;
	uint32_t i=0;

	begin = trimFrontSpaceAndComma(begin, end);

	if(begin < end && *begin == '0')
	{
		if(begin + 1 < end)
		{
			if(*(begin + 1) == 'x' || *(begin + 1) == 'X')
			{
				value.push_back(*begin);
				value.push_back(*(begin + 1));

				begin += 2;
				i+=2;
				isDecimal = false;
			}
		}
	}

	while(begin < end)
	{
		if(isDecimal)
		{
			if(is_char_09(*begin))
			{
				status = true;
				value.push_back(*begin);
				++begin;
			}
			else
			{
				break;
			}
		}
		else
		{
			// it is hex number;
			if(is_char_09(*begin) || 'A' <= *begin && *begin <= 'F' ||
			   'a' <= *begin && *begin <= 'f')
			{
				status = true;
				value.push_back(*begin);
				++begin;
				if(++i>maxTokenStringSizeAllowed)
				{
					status=false;
					PrintError("Find a token overflow, maxTokenStringSizeAllowed: %d\n",maxTokenStringSizeAllowed);
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	return begin;
}

const NodeInfoArray* BiTreeParser::BiTreePropertyParser::getNodeInfoArray()const
{
	assert(mNodeInfoArray != nullptr);
	return mNodeInfoArray;
}


bool BiTreeParser::BiTreePropertyParser::parsing(str_ptr_t begin, str_ptr_t end)
{
	bool status = false;
	std::string currentNodeIdStr;
	uint32_t currentNodeId;
	bool isExtendPropertyCurrent=false;
	
	// 001, get node id.
	begin = getNodeId(begin, end, status, currentNodeIdStr);
	if(status==false)
	{
		if(begin < end && *begin == '[')
		{
			isExtendPropertyCurrent=true;
		}
		else
		{
			PrintError("Invalid node id string: [%s].\n", currentNodeIdStr.c_str());
			return false;
		}
	}
	else
	{
		if(!BiTreePropertyManager::isNodeIdValid(currentNodeIdStr,currentNodeId))
		{
			PrintError("Invalid node id: [%s].\n",currentNodeIdStr.c_str());
			return false;
		}
	}

	if(isExtendPropertyCurrent)
	{
		PrintInfo("Parsing property: [");
	}
	else
	{
		PrintInfo("Parsing property: %s[",currentNodeIdStr.c_str());
	}

	begin = trimLpSquare(begin, end, status);
	std::vector<std::string> tempLog;
	bool isFirstKeyAndValue=1;

	if(status==true)
	{
		while(begin < end)
		{
			bool ok1 = false;
			bool ok2 = false;

			BiTreePropertyManager::PropertyRecord propertyRecord;
			std::string propertyName;
			std::string propertyValue;

			// 002, Get propertyName;
			begin = getPropertyName(begin, end, ok1, propertyName);
			if(!ok1)
			{
				return false;
			}

			if(isFirstKeyAndValue)
			{
				printf("%s ", propertyName.c_str());
			}
			else
			{
				printf("; %s ", propertyName.c_str());
			}
			
			if(!BiTreePropertyManager::isPropertyNameValid(propertyName,propertyRecord))
			{
				PrintError("Invalid property name: [%s]\n",propertyName.c_str());
				return false;
			}
			if(propertyRecord.isExtendProperty)
			{
				if(isExtendPropertyCurrent)
				{
					// It is: [key = value,  ...], ok
				}
				else
				{
					std::string str{"[error]: Extend roperty: ["+propertyName+"] cannot be used as node["+currentNodeIdStr+"]'s private property, it will be ignored.\n"};
					tempLog.push_back(str);					
					//return false;
				}
			}
			else
			{
				// It is: nodeId[key = value, ...];
			}

			// Get propertyValue;
			begin = trimEq(begin, end, status);
			if(propertyRecord.valueType==BiTreePropertyManager::PropertyRecord::ElementValueType::Uint32)
			{
				begin = getValueUint32(begin, end, ok2, propertyValue);
			}
			else
			{
				begin = getValueString(begin, end, ok2, propertyValue);
			}
			
			if(!ok2)
			{
				PrintError("Get-property-value faild.\n");
				return false;
			}

			printf("= %s",propertyValue.c_str());
			std::any value;
			if(!BiTreePropertyManager::isPropertyValueValid(propertyRecord,propertyValue,value))
			{
				PrintError("Property-value is invalid.\n");
				return false;
			}

			// Save PropertyName(this is an elementIndex in PropertyManager.propertyTable);
			// Save propertyValue;
			if(isExtendPropertyCurrent)
			{
				BiTreePropertyManager::setCurrentProperty(propertyRecord,value,true);
			}
			else
			{
				// Finally insert [propertyKey = propertyValue] here.
				BiTreeAssert(mNodeInfoArray != nullptr);
				status=mNodeInfoArray->insertNodeInfo(currentNodeId, propertyRecord.elementIndexOrId,value);
				if(status==false)
				{
					PrintError("Insert to mNodeInfoArray failed.\n");
				}
			}

			isFirstKeyAndValue=0;

			// try to get next "key = value";
			begin = trimFrontSpaceAndComma(begin, end);
			begin = trimRpSquare(begin, end, status);
			if(status==true)
			{	
				// find "]" means parsing end;
				begin=trimFrontSpaceAndCommaAndRestComment(begin,end);
				if(begin == end)
				{
					break;
				}
				else
				{
					return false;
				}
			}
			else
			{
				// Do not find "]" means any more "key = value" exist, it's ok;
			}
		} //while-end;
	}
	else
	{
		begin = trimFrontSpace(begin, end);
		if(begin != end)
		{
			return false;
		}
	}

	printf("]\n");

	for(auto const& log:tempLog)
	{
		printf("%s",log.c_str());
	}

	return true;
}



//
// BiTreeParser-------------------------------------------------------
//
BiTreeParser::BiTreeParser()
	: mRoot(nullptr),mPropertyParser(nullptr)
{
	BiTreeAssert(mPropertyParser==nullptr);
	mPropertyParser=new BiTreePropertyParser();
	BiTreeAssert(mPropertyParser!=nullptr);
}

BiTreeParser::~BiTreeParser()
{
	if(mRoot)
	{
		freeBiTree(&mRoot);
		mRoot = nullptr;
	}
	if(mPropertyParser)
	{
		delete mPropertyParser;
		mPropertyParser=nullptr;
	}
}
const std::size_t& BiTreeParser::getMaxTokenSizeCurrent()
{
	return maxTokenSizeCurrent;
}

BiTreeParser::ActionType
BiTreeParser::action(const Token& token, const Token& token1, const Token& token2)
{
	if(token.name == TokenName::Node && token1.name == TokenName::Int &&
	   token2.name == TokenName::Lp)
	{
		return ActionType::PushProduction1;
	}
	else if(token.name == TokenName::Node && token1.name == TokenName::Int &&
			(token2.name == TokenName::Comma || token2.name == TokenName::Rp))
	{
		return ActionType::PushProduction2;
	}
	else if(token.name == TokenName::Node &&
			(token1.name == TokenName::Comma || token1.name == TokenName::Rp ||
			 token1.name == TokenName::Eof))
	{
		return ActionType::PopStack;
	}
	else
	{
		if(token == token1)
		{
			return ActionType::PopStackAndMoveIt;
		}
	}

	return ActionType::Error;
}

const NodeInfoArray* BiTreeParser::getNodeInfoArray()const
{
	BiTreeAssert(mPropertyParser!=nullptr);
	return mPropertyParser->getNodeInfoArray();
}

BiTreeParser::TokenArray BiTreeParser::getTokenArray(const std::string& text)
{
	TokenArray tokenArray;
	auto it = text.begin();

	// This data just like a C language name.
	auto isDataChar = [=](const char& c) {
		if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
		{
			return true;
		}
		return false;
	};

	while(it != text.end())
	{
		if(*it <= 0x20)
		{
			++it;
		}
		if(*it == '(')
		{
			++it;
			tokenArray.push_back(Token(TokenName::Lp));
		}
		else if(*it == ')')
		{
			++it;
			tokenArray.push_back(Token(TokenName::Rp));
		}
		else if(*it == ',')
		{
			++it;
			tokenArray.push_back(Token(TokenName::Comma));
		}
		else if(isDataChar(*it))
		{
			std::string subStr;
			std::size_t i = 0;
			while(it != text.end() && isDataChar(*it))
			{
				subStr.push_back(*it);
				++it;
				++i;
				if(i > maxTokenSizeAllowed)
				{
					PrintError("Found a token.size() overflow. maxTokenSizeAllowed=%d\n",
							   maxTokenSizeAllowed);
					PrintError("The token is:\n%s\n", subStr.c_str());
					tokenArray.clear();
					return tokenArray;
				}
			}
			tokenArray.push_back(Token(TokenName::Int, subStr));
			if(maxTokenSizeCurrent < i)
			{
				maxTokenSizeCurrent = i;
			}
				
		}
		else
		{
			// Dump some error information.
			int i = 0;
			std::string str;
			while(it < text.end() && ++i < 32)
			{
				str.push_back(*it);
				++it;
			}
			str += "......\n";
			PrintError("Meet error char here:\n%s\n", str.c_str());

			tokenArray.clear();
			break;
		}
	} //while-end;

	return tokenArray;
}

bool BiTreeParser::parsing(const std::string& text)
{
	auto tokenArray = getTokenArray(text);
	if(tokenArray.empty())
	{
		return false;
	}

	tokenArray.push_back(Token(TokenName::Eof));

	auto it = tokenArray.begin();
	std::stack<Token> tokenStack;
	tokenStack.push(Token(TokenName::Eof));
	tokenStack.push(Token(TokenName::Node));

	BiTreeNode* root = nullptr;
	std::stack<BiTreeNode**> biTreeNodeStack;
	biTreeNodeStack.push(&root);
	bool good = true;

	while((it != tokenArray.end()) && good)
	{
		//
		// It is using LL2-parsing technology;
		// First-forward-looking token is *it;
		// Second-forward-looking token is token2;
		//
		Token token2;
		if(it + 1 == tokenArray.end())
		{
			token2.name = TokenName::Epsilon; // No more token, so token2 is epsilon;
		}
		else
		{
			token2.name = (*(it + 1)).name; // else token2 is second token followed by *it;
		}

		auto act = action(tokenStack.top(), *it, token2);
		switch(act)
		{
		case ActionType::PushProduction1: {
			tokenStack.pop();
			tokenStack.push(Token(TokenName::Rp));
			tokenStack.push(Token(TokenName::Node));
			tokenStack.push(Token(TokenName::Comma));
			tokenStack.push(Token(TokenName::Node));
			tokenStack.push(Token(TokenName::Lp));
			tokenStack.push(Token(TokenName::Int));

			// build bitree 001;
			auto node = biTreeNodeStack.top();
			*node=new BiTreeNode(it->value);
			BiTreeAssert(*node!=nullptr);
			
			biTreeNodeStack.pop();
			biTreeNodeStack.push(&((*node)->rightChild));
			biTreeNodeStack.push(&((*node)->leftChild));
			biTreeNodeStack.push(node);
			break;
		}
		case ActionType::PushProduction2: {
			tokenStack.pop();
			tokenStack.push(Token(TokenName::Int));

			// build bitree 002;
			auto node = biTreeNodeStack.top();
			*node=new BiTreeNode(it->value);
			BiTreeAssert(*node!=nullptr);

			biTreeNodeStack.pop();
			biTreeNodeStack.push(node);
			break;
		}
		case ActionType::PopStack: {
			auto token = tokenStack.top();
			if(token.name == TokenName::Node)
			{
				biTreeNodeStack.pop();
			}

			tokenStack.pop();

			break;
		}
		case ActionType::PopStackAndMoveIt: {
			auto token = tokenStack.top();
			if(token.name == TokenName::Int)
			{
				biTreeNodeStack.pop();
			}

			tokenStack.pop();

			++it;
			break;
		}
		default:
			PrintError("StackTop: [%s], looking1: [%s], looking2: [%s].\n",
					   tokenStack.top().value.c_str(),
					   it->value.c_str(),
					   token2.value.c_str());
			good = false;
			break;
		}
	}

	if(!good)
	{
		maxTokenSizeCurrent = 0;
		if(root != nullptr)
		{
			freeBiTree(&root);
			root = nullptr;
		}
	}

	if(root)
	{
		freeBiTree(&mRoot);
		mRoot = root;
	}

	// This parser cannot handle one-node-tree(for example: "bitree = 1"),
	// but I will give out a simple solution.
	auto tryApplyPatchForOneNodeTree = [&]() {
		// tokenArray.size()==2 means:
		// first token is a valid token;
		// second token is "End-Of-Stream" token of this-Parser-define;
		if(tokenArray.size() == 2 && tokenArray.front().name == TokenName::Int)
		{
			// Renew maxTokenSizeCurrent for the layouter to calcluate the best radius for the node.
			maxTokenSizeCurrent = tokenArray.front().value.size();
			// Create one node;
			return new BiTreeNode(tokenArray.front().value);
		}
		return (BiTreeNode*)nullptr;
	};

	if(mRoot==nullptr)
	{
		mRoot=tryApplyPatchForOneNodeTree();
	}

	return (mRoot != nullptr ? true : false);
}

bool BiTreeParser::empty()
{
	return (mRoot != nullptr ? true : false);
}

BiTreeParser::BiTreeNode* BiTreeParser::getRoot()
{
	BiTreeAssert(mRoot!=nullptr);
	return mRoot;
}

BiTreeParser::BiTreePropertyParser* BiTreeParser::getPropertyParser()
{
	BiTreeAssert(mPropertyParser != nullptr);
	return mPropertyParser;
}

void BiTreeParser::freeBiTree(BiTreeNode** root)
{
	if(root == nullptr)
		return;
	auto node = *root;
	if(node == nullptr)
		return;

	std::stack<BiTreeNode*> stack1;
	std::stack<BiTreeNode*> stack2;

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
		delete n;
	}

	*root = nullptr;
}


//
// BiTreeParser::BiTreePropertyParser--------------------------------
//
BiTreeParser::BiTreePropertyParser::BiTreePropertyParser()
	:mNodeInfoArray(nullptr)
{
	BiTreeAssert(mNodeInfoArray==nullptr);
	mNodeInfoArray=new NodeInfoArray;
	BiTreeAssert(mNodeInfoArray!=nullptr);
}
BiTreeParser::BiTreePropertyParser::~BiTreePropertyParser()
{
	if(mNodeInfoArray)
	{
		delete mNodeInfoArray;
		mNodeInfoArray=nullptr;
	}
}


