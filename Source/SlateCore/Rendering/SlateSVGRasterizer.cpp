#include "PreCompileHeader.h"

#include "SlateSVGRasterizer.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_IMPLEMENTATION	// expands implementation
#include "src/nanosvg.h" //nanosvg depends on stdio/string/math
#define NANOSVGRAST_IMPLEMENTATION
#include "src/nanosvgrast.h"

namespace SVGConstants
{
	//the dpi is only used for unit conversion
	static const float DPI = 96.0f;

	//bytes per pixel (RGBA)
	static const int32_t BPP = 4;
}

namespace DoDo {
	std::vector<uint8_t> FSlateSVGRasterizer::rasterize_svg_from_file(const DoDoUtf8String& file_name, FIntPoint pixel_size)
	{
		std::vector<uint8_t> pixel_data;

		std::string svg_string;//todo:use our string

		//todo:load file to string
		NSVGimage* image = nsvgParseFromFile(file_name.c_str(), "px", SVGConstants::DPI);

		if (image)
		{
			pixel_data.resize(pixel_size.x * pixel_size.y * SVGConstants::BPP, 0);

			NSVGrasterizer* rasterizer = nsvgCreateRasterizer();

			const float svg_scale_x = (float)pixel_size.x / image->width;
			const float svg_scale_y = (float)pixel_size.y / image->height;

			const int32_t stride = pixel_size.x * SVGConstants::BPP;
			nsvgRasterize(rasterizer, image, 0, 0, svg_scale_x, pixel_data.data(), pixel_size.x, pixel_size.y, stride);//todo:implement nsvg rasterize full

			nsvgDeleteRasterizer(rasterizer);

			nsvgDelete(image);
		}

		return pixel_data;
	}
}