#pragma once

#include <glm/glm.hpp>

#include "Core/Containers/EnumAsBytes.h"//FSlateBrush depends on it
#include "Core/Math/Box2D.h"
#include "SlateCore/Layout/Margin.h"//FMargin

#include "Core/String/DoDoString.h"

#include "glm/glm.hpp"

#include "Core/Object/Object.h"//texture object

#include "SlateCore/Textures/SlateShaderResource.h"

#include "SlateCore/Layout/Margin.h"

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
	* enumerates brush image types
	*/
	namespace ESlateBrushImageType
	{
		enum Type
		{
			//no image loaded, color only brushes
			NoImage,
			//the image to be loaded is in full color
			FullColor,
			//the image is a special texture in linear space
			Linear,
			//the image is vector graphics and will be rendered and cached
			Vector
		};
	}

	/*
	* enumerates rounding options
	*/
	namespace ESlateBrushRoundingType
	{
		enum Type
		{
			/*use the specified radius*/
			FixedRadius,

			/*the rounding radius should be half the height such that it always looks perfectly round*/
			HalfHeightRadius
		};
	}

	/*possible options for rounded box brush image*/
	struct FSlateBrushOutlineSettings
	{
		FSlateBrushOutlineSettings()
			: m_corner_radii(0.0f)
			, m_color(glm::vec4(1.0f))
			, m_width(0.0f)
			, m_rounding_type(ESlateBrushRoundingType::HalfHeightRadius)
			, m_b_use_brush_transparency(false)
		{}

		FSlateBrushOutlineSettings(float in_uniform_radius)
			: m_corner_radii(glm::vec4(in_uniform_radius))
			, m_color(glm::vec4(1.0f))
			, m_width(0.0f)
			, m_rounding_type(ESlateBrushRoundingType::FixedRadius)
			, m_b_use_brush_transparency(false)
		{}

		FSlateBrushOutlineSettings(const glm::vec4& in_color, float in_width)
			: m_corner_radii(0.0f)
			, m_color(in_color)
			, m_width(in_width)
			, m_rounding_type(ESlateBrushRoundingType::HalfHeightRadius)
			, m_b_use_brush_transparency(false)
		{}

		/*radius in slate unit applied to the outline at each corner, x = top left, y = top right, z = bottom right, w = bottom left*/
		glm::vec4 m_corner_radii;

		/*tinting applied to the border outline*/
		glm::vec4 m_color;

		/*line with in slate units applied to the border outline*/
		float m_width;

		/*the rounding type*/
		TEnumAsByte<enum ESlateBrushRoundingType::Type> m_rounding_type;

		/*true if we should use the owning brush's transparency as our own*/
		bool m_b_use_brush_transparency;
	};


	/*
	 * an brush which contains information about how to draw a slate element
	 */
	struct FSlateBrush
	{
		//todo:implement FSlateShaderResourceManager
		friend class FSlateShaderResourceManager;//access the resource handle
	public:
		/*size of the resource in slate units*/
		glm::vec2 m_image_size;

		/*the margin to use in box and border modes*/
		FMargin m_margin;

		/*tinting applied to the image*/
		glm::vec4 m_tint_color;

		/*how to draw the outline, currently only used for rounded box type brushes*/
		//todo:implement FSlateBrushOutlineSettings
		FSlateBrushOutlineSettings m_outline_settings;
	public:

		/*
		 * default constructor
		 */
		FSlateBrush();

		virtual ~FSlateBrush();

	public:
		glm::vec2 get_image_size() const { return m_image_size; }

		void set_image_size(glm::vec2 in_image_size) { m_image_size = in_image_size; }

		const FMargin& get_margin() const { return m_margin; }

		ESlateBrushTileType::Type get_tiling() const { return m_tiling; }

		ESlateBrushMirrorType::Type get_mirroring() const { return m_mirroring; }

		//todo:implement get image type
		ESlateBrushImageType::Type get_image_type() const { return m_image_type; }

		ESlateBrushDrawType::Type get_draw_type() const { return m_draw_as; }

		/*
		* gets the name of the resource object, if any
		* 
		* @return resource name, or NAME_None if the resource object is not set
		*/
		const DoDoUtf8String& get_resource_name() const
		{
			if (m_resource_name != "" || m_resource_object == nullptr)
			{
				return m_resource_name;
			}
			else
			{
				m_resource_object->get_name();
			}
		}

		/*
		 * gets the brush's tint color
		 *
		 * @param InWidgetStyle the widget style to get the tint for
		 * @return Tint color
		 */
		glm::vec4 get_tint() const//todo:add widget style
		{
			return m_tint_color;
		}

		const FSlateResourceHandle& get_rendering_resource(glm::vec2 local_size, float draw_scale) const
		{
			update_rendering_resource(get_image_size(), 1.0f);

			return m_resource_handle;
		}

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
		TEnumAsByte<enum ESlateBrushImageType::Type> m_image_type;

	private:
		void update_rendering_resource(glm::vec2 local_size, float draw_scale) const;
		/*
		 * the image to render for this brush, can be UTexture or UMaterialInterface or an object implementing
		 * the AtlasedTextureInteface
		 */
		//todo:implement this resource
		std::shared_ptr<Object> m_resource_object;

		/*rendering resource for this brush*/
		mutable FSlateResourceHandle m_resource_handle;

	protected:
		/*the name of the rendering resource to use*/
		DoDoUtf8String m_resource_name;
	protected:
		/*
		* this constructor is protected, use one of the deriving classes instead
		* 
		* @param InDrawType how to the texture
		* @param InResourceName the name of the resource
		* @param InMargin margin to use in border and box modes
		* @param InTiling tile horizontally/vertically or both?(only in image mode)
		* @param InImageType the type of image
		* @param InTint tint to apply to the element
		* @param InOutlineSettings optional outline border settings for rounded box mode
		*/
		FSlateBrush(ESlateBrushDrawType::Type in_draw_type, const DoDoUtf8String in_source_name, const FMargin& in_margin, ESlateBrushTileType::Type in_tiling, 
		ESlateBrushImageType::Type in_image_type,
		const glm::vec2& in_image_size, const glm::vec4& in_tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), void* in_object_resource = nullptr, bool b_in_dynamically_loaded = false);
	};
}

