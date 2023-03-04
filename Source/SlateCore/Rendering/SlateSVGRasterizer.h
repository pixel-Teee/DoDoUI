#pragma once

#include "Core/Math/IntPoint.h"

namespace DoDo {
	class DoDoUtf8String;
	class FSlateSVGRasterizer//Rasterizer
	{
	public:
		static std::vector<uint8_t> rasterize_svg_from_file(const DoDoUtf8String& file_name, FIntPoint pixel_size);
	};
}