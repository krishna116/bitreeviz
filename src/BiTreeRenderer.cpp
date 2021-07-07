/*
The MIT License

Copyright 2021 Krishna sssky307@163.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "bitreeviz/BiTreeRenderer.h"
#include "bitreeviz/BiTreeParser.h"
#include "bitreeviz/BiTreePropertyManager.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <stack>

#pragma warning(disable : 4275)
#pragma warning(disable : 4244)

#include <cairomm/cairomm.h>

using namespace bitree;

BiTreeRenderer::BiTreeRenderer() { }

BiTreeRenderer::~BiTreeRenderer() { }

double BiTreeRenderer::getBestRadius(const std::size_t& strLen)
{
	auto surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, 20, 20);
	auto cr = Cairo::Context::create(surface);
	Cairo::TextExtents extents;

	BiTreeAssert(strLen > 0);
	std::string text;
	text.assign(strLen, 'A');

	auto& currentProperty = BiTreePropertyManager::getCurrentProperty();
	if(!currentProperty.nodeExtendProperty.fontFamily.empty())
	{
		cr->select_font_face(
			currentProperty.nodeExtendProperty.fontFamily,
			(Cairo::ToyFontFace::Slant)currentProperty.nodeExtendProperty.fontSlant,
			(Cairo::ToyFontFace::Weight)currentProperty.nodeExtendProperty.fontWeight);
	}

	cr->set_font_size((double)currentProperty.nodeExtendProperty.fontSize);
	cr->get_text_extents(text, extents);

	return (extents.width / 2 + extents.x_bearing);
}

double BiTreeRenderer::getDistance(const BiTreeRenderer::pos_t& p1, const BiTreeRenderer::pos_t& p2)
{
	auto width = p1.x - p2.x;
	auto height = p1.y - p2.y;

	return std::sqrt(width * width + height * height);
}

BiTreeRenderer::vec2d_t BiTreeRenderer::getClippedEdge(const BiTreeRenderer::pos_t& childPos,
													   const BiTreeRenderer::pos_t& parentPos,
													   const double& radius,
													   bool& status)
{
	double widthOffset;
	double heightOffset;
	double width;
	double height;
	double distance;
	vec2d_t vec2d;

	status = false;

	if(childPos.x == parentPos.x || parentPos.y >= childPos.y || radius == 0.0f)
	{
		//PrintInfo("Edge is too short to drawing.\n");
		return vec2d;
	}
	if((distance = getDistance(childPos, parentPos)) <= 2 * radius)
	{
		//PrintInfo("Edge is too short to drawing, distance: %f, 2*radius: %f\n", distance, 2 * radius);
		return vec2d;
	}

	width = std::abs(childPos.x - parentPos.x);
	height = std::abs(childPos.y - parentPos.y);
	widthOffset = radius * std::cos(std::atan2(height, width));
	heightOffset = radius * std::sin(std::atan2(height, width));

	// Adjust x position;
	if(childPos.x < parentPos.x)
	{
		vec2d.p1.x = childPos.x + widthOffset;
		vec2d.p2.x = parentPos.x - widthOffset;
	}
	else
	{
		vec2d.p1.x = childPos.x - widthOffset;
		vec2d.p2.x = parentPos.x + widthOffset;
	}

	// Adjust y position;
	vec2d.p1.y = childPos.y - heightOffset;
	vec2d.p2.y = parentPos.y + heightOffset;

	status = true;

	return vec2d;
}

bool BiTreeRenderer::convertDashStringToCairoDash(const std::string& nodeDashStr,
												  std::vector<double>& dashes,
												  double& offset)
{
	bool status = false;
	dashes.clear();
	offset = 0;

	if(nodeDashStr.empty())
	{
		return status;
	}

	auto skipSpace = [](std::string::const_iterator begin, std::string::const_iterator end) {
		while(begin < end && (*begin == ' ' || *begin == '\t' || *begin == '"' || *begin == ';' ||
							  *begin == ',' || *begin == '|' || *begin == ':'))
		{
			++begin;
		}
		return begin;
	};

	auto getNextDigit = [&](std::string::const_iterator begin,
							std::string::const_iterator end,
							double& value,
							bool& status) {
		std::string str;
		status = false;
		value = 0.0f;

		begin = skipSpace(begin, end);
		if(begin < end && (begin + 1) < end && *begin == '0' &&
		   (*(begin + 1) == 'x' || *(begin + 1) == 'X'))
		{
			str.push_back(*begin);
			str.push_back(*(begin + 1));
			begin += 2;
		}

		while(begin < end && '0' <= (*begin) && (*begin) <= '9')
		{
			str.push_back(*begin);
			++begin;
		}

		if(!str.empty())
		{
			auto data = std::stoul(str, nullptr, 0);
			if(0 < data && data <= 0xff)
			{
				value = (double)data;
				status = true;

				return begin;
			}
		}

		return begin;
	};

	auto begin = nodeDashStr.begin();
	auto end = nodeDashStr.end();
	dashes.clear();
	double value = 0.0f;
	while(begin < end)
	{
		begin = skipSpace(begin, end);
		begin = getNextDigit(begin, end, value, status);
		if(status)
		{
			dashes.push_back(value);
		}
		else
		{
			break;
		}
	}

	if(dashes.size() >= 2)
	{
		offset = dashes.back();
		return true;
	}

	return false;
}

bool BiTreeRenderer::draw(const NodeInfoArray* nodeInfoArray,
						  const BiTreeLayouter::LayoutInfo& layoutInfo,
						  const std::string& outputFile)
{
	const double gRadius = layoutInfo.raduis;
	Cairo::TextExtents extents;
	std::shared_ptr<Cairo::PdfSurface> surface;
	std::vector<double> dashes;
	double dashOffset = 0.0f;

	try
	{
		surface =
			Cairo::PdfSurface::create(outputFile, layoutInfo.pageWidth, layoutInfo.pageheight);
	}
	catch(...)
	{
		PrintError("Dump parameter-----------------------------------------------------------\n");
		PrintError("  ofile: %s\n", outputFile.c_str());
		PrintError("  width: %f\n", layoutInfo.pageWidth);
		PrintError("  height: %f\n", layoutInfo.pageheight);
		PrintError("The output file may be locked or incorrect, Cairo PdfSurface create failed.\n");
		return false;
	}

	auto getThisNodeProperty = [&](BiTreeParser::BiTreeNode* node) {
		if(node->isDataDecimalOrHexNumber())// Only using digits-number as index to NodeInfo.
		{
			auto& nodeInfo = nodeInfoArray->findNodeInfo(node->data);
			if(!nodeInfo.empty()) // Find the NodeInfo pass
			{
				// Convert NodeInfo to its data-structure of properties.
				return BiTreePropertyManager::getCurrentPropertyByUpdate(nodeInfo);
			}
		}

		// Return default data-structure of properties.
		return BiTreePropertyManager::getCurrentProperty();
	};

	auto drawPageBackgroundColor = [&](const uint32_t& pageBackgroundColor) {
		auto cr = Cairo::Context::create(surface);
		cr->set_source_rgba(red(pageBackgroundColor),
							green(pageBackgroundColor),
							blue(pageBackgroundColor),
							alpha(pageBackgroundColor));
		cr->paint();
	};

	auto drawCircle = [&](const pos_t& pos,
						  const double& radius,
						  const uint32_t& borderColor,
						  uint32_t& borderWidth,
						  const uint32_t& fillColor,
						  const std::string& borderDashStr) {
		auto cr = Cairo::Context::create(surface);

		cr->arc(pos.x, pos.y, radius, 0.0, Angle360);
		cr->set_source_rgba(red(fillColor), green(fillColor), blue(fillColor), alpha(fillColor));
		cr->fill();

		if(!borderDashStr.empty())
		{
			if(convertDashStringToCairoDash(borderDashStr, dashes, dashOffset))
			{
				cr->set_dash(dashes, dashOffset);
			}
			else
			{
				PrintError("BorderDash value: [%s] may be invalid, it is not used.\n",
						   borderDashStr.c_str());
			}
		}

		cr->set_source_rgba(
			red(borderColor), green(borderColor), blue(borderColor), alpha(borderColor));
		cr->set_line_width(double(borderWidth));
		cr->arc(pos.x, pos.y, radius, 0.0, Angle360);

		cr->stroke();
	};

	auto drawText = [&](const pos_t& pos,
						const std::string& text,
						const std::string& fontFamily,
						uint32_t fontSlant,
						uint32_t fontWeight,
						uint32_t fontSize,
						uint32_t fontColor) {
		if(!text.empty())
		{
			auto cr = Cairo::Context::create(surface);

			cr->set_source_rgba(
				red(fontColor), green(fontColor), blue(fontColor), alpha(fontColor));
			cr->set_font_size((double)fontSize);

			auto font = Cairo::ToyFontFace::create(fontFamily,
												   (Cairo::ToyFontFace::Slant)fontSlant,
												   (Cairo::ToyFontFace::Weight)fontWeight);
			cr->set_font_face(font);

			// Get center drawing position.
			cr->get_text_extents(text, extents);
			auto newX = pos.x - (extents.width / 2 + extents.x_bearing);
			auto newY = pos.y - (extents.height / 2 + extents.y_bearing);
			cr->move_to(newX, newY);
			
			cr->show_text(text);
		}
	};

	auto drawEdge = [&](const pos_t& childPos,
						const pos_t& parentPos,
						const uint32_t radius,
						uint32_t& edgeWidth,
						const uint32_t& edgeColor,
						const std::string& edgeDashStr) {
		bool status = false;
		vec2d_t vec2d = getClippedEdge(childPos, parentPos, radius, status);

		if(status)
		{
			auto cr = Cairo::Context::create(surface);
			cr->move_to(vec2d.p1.x, vec2d.p1.y);
			cr->set_source_rgba(
				red(edgeColor), green(edgeColor), blue(edgeColor), alpha(edgeColor));
			cr->set_line_width((double)edgeWidth);

			if(!edgeDashStr.empty())
			{
				if(convertDashStringToCairoDash(edgeDashStr, dashes, dashOffset))
				{
					cr->set_dash(dashes, dashOffset);
				}
				else
				{
					PrintError("EdgeDash value: [%s] may be invalid, it is not used.\n",
							   edgeDashStr.c_str());
				}
			}

			cr->line_to(vec2d.p2.x, vec2d.p2.y);
			cr->stroke();
		}
	};

	auto showPage = [&]() {
		auto cr = Cairo::Context::create(surface);
		cr->show_page();
	};

	PrintInfo("Rendering bitree..........................\n");

	// 000, draw page background.
	auto const& pageBackgroundColor =
		BiTreePropertyManager::getCurrentProperty().nodeExtendProperty.pageBackgroundColor;
	drawPageBackgroundColor(pageBackgroundColor);

	// 001, draw the bitree's nodes.
	for(auto const& node : layoutInfo.bitreeNodeArray)
	{
		//auto& nodeProperty = getThisNodeProperty(node); cannot work with mingwXX compiler.
		auto nodeProperty = getThisNodeProperty(node);

		const pos_t childPos(layoutInfo.toAbsoluteX(node->x), layoutInfo.toAbsoluteY(node->y));

		auto currentRadius = nodeProperty.nodeBaseProperty.nodeRadius > 0
								 ? ((double)(nodeProperty.nodeBaseProperty.nodeRadius))
								 : (gRadius);

		// 002-1, draw edge.
		if(node->parent != nullptr)
		{
			const pos_t parentPos(layoutInfo.toAbsoluteX(node->parent->x), layoutInfo.toAbsoluteY(node->parent->y));
			bool status = false;
			drawEdge(childPos,
					 parentPos,
					 currentRadius,
					 nodeProperty.nodeBaseProperty.edgeWidth,
					 nodeProperty.nodeBaseProperty.edgeColor,
					 nodeProperty.nodeBaseProperty.edgeDash);
		}

		// 002-2, draw circle.
		drawCircle(childPos,
				   currentRadius,
				   nodeProperty.nodeBaseProperty.borderColor,
				   nodeProperty.nodeBaseProperty.borderWidth,
				   nodeProperty.nodeBaseProperty.fillColor,
				   nodeProperty.nodeBaseProperty.borderDash);

		// 002-3, draw text.
		std::string nodeLabel = (nodeProperty.nodeBaseProperty.nodeLabel.empty()
									 ? (*node).data
									 : nodeProperty.nodeBaseProperty.nodeLabel);
		drawText(childPos,
				 nodeLabel,
				 nodeProperty.nodeExtendProperty.fontFamily,
				 nodeProperty.nodeExtendProperty.fontSlant,
				 nodeProperty.nodeExtendProperty.fontWeight,
				 nodeProperty.nodeExtendProperty.fontSize,
				 nodeProperty.nodeBaseProperty.fontColor);
	} // Drawing things end.

	// 003, get result.
	showPage();
	PrintInfo("Rendering bitree..........................ok\n\n");
	PrintInfo("Output: [%s]\n",outputFile.c_str());

	return true;
}
