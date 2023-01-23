#pragma once

#include "glm/vec4.hpp"
#include "glm/vec2.hpp"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depend on it

namespace DoDo
{
	class FPaintGeometry;
	class FSlateBrush;

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
		friend class FSlateWindowElementList;

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

	private:
		static FSlateDrawElement& MakeBoxInternal(FSlateWindowElementList& element_list, uint32_t in_layer, const FPaintGeometry& paint_geometry, const FSlateBrush* in_brush, ESlateDrawEffect in_draw_effect,
			const glm::vec4& in_tint);
	};

	class SWindow;
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
		/*
		 * creates an uninitialized draw element
		 */
		FSlateDrawElement& add_uninitialized();

	private:
		/*
		* window which owns the widgets that are being painted but not necessarily rendered to
		* widgets are always rendered to the RenderTargetWindow
		*/
		std::weak_ptr<SWindow> m_weak_paint_window;
		SWindow* m_raw_paint_window;

		/*the uncached draw elements to be processed*/
		//store the FSlateDrawElement
		FSlateDrawElementArray m_uncached_draw_elements;

		/*store the size of the window beging used to paint*/
		glm::vec2 m_window_size;
	};
}
