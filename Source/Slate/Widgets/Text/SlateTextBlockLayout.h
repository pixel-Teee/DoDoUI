#pragma once

#include "SlateCore/Styling/SlateTypes.h"//FTextBlockStyle depends on it

#include "SlateCore/Fonts/FontCache.h"//ETextShapingMethod

#include "Slate/Framework/Text/TextLayout.h"

namespace DoDo {
	class SWidget;
	class FSlateTextLayout;
	class FPaintArgs;
	struct FGeometry;
	class FSlateRect;
	class FSlateWindowElementList;
	class ITextLayoutMarshaller;
	/*class to handle the cached layout of STextBlock by proxying around a FTextLayout*/
	class FSlateTextBlockLayout
	{
	public:
		struct FWidgetDesiredSizeArgs
		{
			FWidgetDesiredSizeArgs(
				const DoDoUtf8String& in_text,
				const DoDoUtf8String& in_highlight_text,
				const float in_wrap_text_at,
				const bool in_auto_wrap_text,
				const ETextWrappingPolicy in_wrapping_policy,
				const ETextTransformPolicy in_transform_policy,
				const FMargin& in_margin,
				const float in_line_height_percentage,
				const ETextJustify::Type in_justification
			)
				: m_text(in_text)
				, m_highlight_text(in_highlight_text)
				, m_margin(in_margin)
				, m_wrap_text_at(in_wrap_text_at)
				, m_line_height_percentage(in_line_height_percentage)
				, m_wrapping_policy(in_wrapping_policy)
				, m_transform_policy(in_transform_policy)
				, m_justification(in_justification)
				, m_auto_wrap_text(in_auto_wrap_text)
			{

			}
				
			const DoDoUtf8String m_text = "";//empty
			const DoDoUtf8String m_highlight_text = "";
			const FMargin m_margin;
			const float m_wrap_text_at = 0.0f;
			const float m_line_height_percentage;
			const ETextWrappingPolicy m_wrapping_policy;
			const ETextTransformPolicy m_transform_policy;
			const ETextJustify::Type m_justification;
			const bool m_auto_wrap_text = false;
		};

		FSlateTextBlockLayout();

		/*
		* constructor
		*/
		FSlateTextBlockLayout(SWidget* in_owner, FTextBlockStyle in_default_text_style,
			const std::optional<ETextShapingMethod> in_text_shaping_method, const std::optional<ETextFlowDirection> in_text_flow_direction, std::shared_ptr<ITextLayoutMarshaller> in_marshaller);

		glm::vec2 get_desired_size() const;

		/*
		* paint this layout, updating the internal cache as required
		*/
		int32_t On_Paint(const FPaintArgs& in_paint_args, const FGeometry& in_allotted_geometry, const FSlateRect& in_clipping_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style,
			bool b_parent_enabled);

		/*
		* get the computed desired size for this layout, updating the internal cache as required
		*/
		glm::vec2 Compute_Desired_Size(const FWidgetDesiredSizeArgs& in_widget_args, const float in_scale, const FTextBlockStyle& in_text_style);
	private:


		/*updates the text layout to contain the given text*/
		void update_text_layout(const DoDoUtf8String& in_text);

		/*in control of the layout and wrapping of the text*/
		std::shared_ptr<FSlateTextLayout> m_text_layout;

		/*the marshaller used to get/set the text to/from the text layout*/
		std::shared_ptr<ITextLayoutMarshaller> m_marshaller;

		/*cache where to wrap text at?*/
		float m_cache_wrap_text_at;

		/*cache the auto wrap text value*/
		bool m_cached_auto_wrap_text;
	};
}