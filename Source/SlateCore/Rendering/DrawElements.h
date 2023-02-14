#pragma once


#include "glm/vec4.hpp"
#include "glm/vec2.hpp"

#include "SlateRenderTransform.h"//FSlateDrawElement depends on it

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depend on it

#include "ElementBatcher.h"//FSlateWindowElementList depend on it

namespace DoDo
{
	struct FSlateFontInfo;
	struct FPaintGeometry;
	struct FSlateBrush;
	struct FSlateDataPayload;

	//todo:add bit field
	enum class EElementType
	{
		ET_Box,
		ET_DebugQuad,
		ET_Text,
		ET_ShapedText,
		ET_Spline,
		ET_Line,
		ET_Gradient,
		ET_Viewport,
		ET_Border,
		ET_Custom,
		ET_CustomVerts,
		ET_PostProcessPass,
		ET_RoundedBox,
		/* total number of draw commands */
		ET_Count
	};

	/*
	 * FSlateDrawElement is the building block for slate's rendering interface
	 * slate describes it's visual output as an ordered list of FSlateDrawElement s
	 */
	class FSlateDrawElement
	{
	public:
		friend class FSlateWindowElementList;

		FSlateDrawElement();

		~FSlateDrawElement();

		FSlateDrawElement(FSlateDrawElement&& rhs);

		FSlateDrawElement(const FSlateDrawElement& rhs) = delete;

		int32_t get_layer() const { return m_layer_id; }

		EElementType get_element_type() const { return m_element_type; }

		template<typename PayloadType>
		const PayloadType& get_data_pay_load() const { return *(PayloadType*)m_data_payload; }//cast to concrete type

		const FSlateRenderTransform& get_render_transform() const { return m_render_transform; }

		glm::vec2 get_local_size() const { return m_local_size; }

		float get_scale() const { return m_scale; }

		ESlateDrawEffect get_draw_effects() const { return m_draw_effect; }

		/*
		 * creates a box element based on the following diagram
		 * allows for this element to be resized while maintain the border of the image
		 * if there no margins the resulting box is simply a quad
		 *
		 *		__LeftMargin		__RightMargin
		 *	   /				   /
		 *	+--+------------------+--+
		 *	|  |c1				  |c2|__TopMargin
		 *	+--o------------------o--+
		 *	|  |				  |  |
		 *	|  |c3                |c4|
		 *	+--o------------------o--+
		 *	|  |                  |  |__BottomMargin
		 *	+--+------------------+--+
		 *
		 * @param ElementList The list in which to add elements
		 * @param InLayer The layer to draw the element on
		 * @param PaintGeometry DrawSpace position and dimensions, see FPaintGeometry
		 * @param InDrawEffects Optional draw effects to apply
		 * @param InTint Color to tint the element
		 */
		static void MakeBox(
			FSlateWindowElementList& element_list,
			uint32_t in_layer,
			const FPaintGeometry& paint_geometry,
			const FSlateBrush* in_brush,
			ESlateDrawEffect in_draw_effects,
			const glm::vec4& in_tint);

		/*
		 * creates a text element which displays a string of a rendered in a certain font on the screen
		 *
		 * @param ElementList the list in which to add elements
		 * @param InLayer the layer to draw the element on
		 * @param PaintGeometry draw space position and dimensions, see FPaintGeometry
		 * @param InText the string to draw
		 * @param StartIndex inclusive index to start rendering from on the specified text
		 * @param EndIndex exclusive index to stop rendering on the specified text
		 * @param InFontInfo the font to draw the string with
		 * @param InDrawEffects optional draw effects to apply
		 * @param InTint color to tint the element
		 */
		//todo:implement FSlateFontInfo
		static void make_text(FSlateWindowElementList& element_list, uint32_t in_layer, const FPaintGeometry& paint_geometry, const DoDoUtf8String& in_text,
		const FSlateFontInfo& in_font_info, ESlateDrawEffect in_draw_effects = ESlateDrawEffect::None, const glm::vec4& in_tint = glm::vec4(1.0f));

		glm::vec2 get_position() const { return m_position; }
	private:
		void init(FSlateWindowElementList& element_list, EElementType in_element_type, uint32_t in_layer, const FPaintGeometry& paint_geometry, ESlateDrawEffect in_draw_effects);

		static FSlateDrawElement& MakeBoxInternal(FSlateWindowElementList& element_list, uint32_t in_layer, const FPaintGeometry& paint_geometry, const FSlateBrush* in_brush, ESlateDrawEffect in_draw_effect,
			const glm::vec4& in_tint);

	private:
		FSlateDataPayload* m_data_payload;

		FSlateRenderTransform m_render_transform;//FTransform 2D

		glm::vec2 m_position;

		glm::vec2 m_local_size;

		int32_t m_layer_id;

		float m_scale;

		ESlateDrawEffect m_draw_effect;

		EElementType m_element_type;
	};

	class SWindow;
	//class FSlateBatchData;
	/*
	 * represents a top level window and it's draw elements
	 */
	class FSlateWindowElementList
	{
		friend class FSlateElementBatcher;
	public:
		/*
		* construct a new list of elements with which to paint a window
		* 
		* @param InPaintWindow the window that owns the widgets being painted, this is almost most always the same window that is beging rendered to
		* @param InRenderWindow the window that we will be rendering to
		*/
		explicit FSlateWindowElementList(const std::shared_ptr<SWindow>& in_paint_window);

		~FSlateWindowElementList();
		/*@return get the window that we will be painting*/
		/*FSlateWindowElementList don't hold the life time of window*/
		SWindow* get_paint_window() const
		{
			return !m_weak_paint_window.expired() ? m_raw_paint_window : nullptr;
		}

		/*@return get the window that we will be rendering to, unless you are a slate renderer you probably want to use get_paint_window()*/
		SWindow* get_render_window() const
		{
			//note: this assumes that the paint window is safe to paint and is not accessed by another thread
			return m_render_target_window != nullptr ? m_render_target_window : get_paint_window();
		}

		/*@return get the draw elements that we want to render into this window*/
		const FSlateDrawElementArray& get_uncached_draw_elements() const
		{
			return m_uncached_draw_elements;
		}

		template<typename PayloadType>
		PayloadType& create_pay_load(FSlateDrawElement& draw_element)
		{
			//elements are responsible for deleting their own payloads
			PayloadType* pay_load;

			pay_load = new PayloadType();

			draw_element.m_data_payload = pay_load;//owns the life time

			return *pay_load;
		}

		/*
		 * creates an uninitialized draw element
		 */
		FSlateDrawElement& add_uninitialized();

		/*
		 * remove all the elements from this draw list
		 */
		void reset_element_list();

		FSlateBatchData& get_batch_data();

	private:
		/*
		* window which owns the widgets that are being painted but not necessarily rendered to
		* widgets are always rendered to the RenderTargetWindow
		*/
		std::weak_ptr<SWindow> m_weak_paint_window;
		SWindow* m_raw_paint_window;

		/*
		 * the window to render to. this may be different from the paint window if we are displaying the contents of a window (or virtual window) onto another window
		 * if this is null, the paint window is used
		 */
		SWindow* m_render_target_window;

		/*batched data used for rendering*/
		FSlateBatchData m_batch_data;

		/*the uncached draw elements to be processed*/
		//store the FSlateDrawElement
		FSlateDrawElementArray m_uncached_draw_elements;

		/*store the size of the window beging used to paint*/
		glm::vec2 m_window_size;
	};
}
