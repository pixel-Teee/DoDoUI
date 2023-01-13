#pragma once

#include <glm/glm.hpp>

#include "Core/Containers/EnumAsBytes.h"//FSlateBrush depends on it
#include "Core/Math/Box2D.h"
#include "SlateCore/Layout/Margin.h"//FMargin

namespace DoDo
{
	/*
	 * enumerates ways in which an image can be drawn
	 */
	namespace ESlateBrushDrawType
	{
		enum Type
		{
			/*don't do anything*/
			NoDrawType,

			/*draw a 3x3 box, where the sides and the middle stretch based on the margin*/
			Box,

			/*draw a 3x3 border where the sides tile and the middle is empty*/
			Border,

			/*draw an image, margin is ignored*/
			Image,

			/*draw a solid rectangle with an outline and corner radius*/
			RoundedBox
		};
	}

	/*
	 * enumerates tiling options for image drawing
	 */
	namespace ESlateBrushTileType
	{
		enum Type
		{
			/*just stretch*/
			NoTile,

			/*tile the image horizontally*/
			Horizontal,

			/*tile the image vertically*/
			Vertical,

			/*tile in both directions*/
			Both
		};
	}

	/*
	 * enumerates options for mirroring the brush image
	 */
	namespace ESlateBrushMirrorType
	{
		enum Type
		{
			/*don't mirror anything, just draw the texture as it is*/
			NoMirror,

			/*mirror the image horizontally*/
			Horizontal,

			/*mirror the image vertically*/
			Vertical,

			/*mirror in both directions*/
			Both
		};
	}


	/*
	 * an brush which contains information about how to draw a slate element
	 */
	struct FSlateBrush
	{
		//todo:implement FSlateShaderResourceManager

	public:
		/*size of the resource in slate units*/
		glm::vec2 m_image_size;

		/*the margin to use in box and border modes*/
		FMargin m_margin;

		/*tinting applied to the image*/
		glm::vec4 m_tint_color;

		/*how to draw the outline, currently only used for rounded box type brushes*/
		//todo:implement FSlateBrushOutlineSettings

	public:

		/*
		 * default constructor
		 */
		FSlateBrush();

		virtual ~FSlateBrush();

	public:
		glm::vec2 get_image_size() const { return m_image_size; }

		void set_image_size(glm::vec2 in_image_size) { m_image_size = in_image_size; }

		const FMargin& get_margin() const { m_margin; }

		ESlateBrushTileType::Type get_tiling() const { return m_tiling; }

		ESlateBrushMirrorType::Type get_mirroring() const { return m_mirroring; }

		//todo:implement get image type

		ESlateBrushDrawType::Type get_draw_type() const { return m_draw_as; }


		/*
		 * get brush uv region, should check if region is valid before using it
		 *
		 * @return uv region
		 */
		FBox2f get_uv_region() const
		{
			return m_uv_region;//todo:return double
		}

	protected:

		/*
		 * optional uv region for an image
		 * when valid - overrides uv region specified in resource proxy
		 */
		FBox2f m_uv_region;

	public:
		/*how to draw the image*/
		TEnumAsByte<enum ESlateBrushDrawType::Type> m_draw_as;

		/*how to tile the image in image mode*/
		TEnumAsByte<enum ESlateBrushTileType::Type> m_tiling;

		/* how to mirror the image in image mode, this is normally only used for dynamic image brushes where the source texture
		 * comes from a hardware device such as a web camera*/
		TEnumAsByte<enum ESlateBrushMirrorType::Type> m_mirroring;

		/*the type of image*/
		//TEnumAsByte<enum ESlateBrushImageType::Type> m_image_type;
	};
}

