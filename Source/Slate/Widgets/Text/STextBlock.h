#pragma once

#include "SlateCore/Layout/Margin.h"//FMargin
#include "SlateCore/Widgets/SLeafWidget.h"

#include "Core/String/DoDoString.h"
#include "SlateCore/Fonts/SlateFontInfo.h"
#include "SlateCore/Styling/CoreStyle.h"
#include "SlateCore/Styling/ISlateStyle.h"
#include "SlateCore/Styling/SlateTypes.h"//FCoreStyle

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"//FTextBlockStyle depends on it

namespace DoDo
{
	struct FSlateBrush;
	/*
	 * a simple static text widget
	 */
	class STextBlock : public SLeafWidget
	{
		SLATE_DECLARE_WIDGET(STextBlock, SLeafWidget)
	public:
		SLATE_BEGIN_ARGS(STextBlock)
			: _Text()
			, _TextStyle(&FCoreStyle::get().get_widget_style<FTextBlockStyle>("NormalText"))
			{}

			/*the text displayed in this text block*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Text)

			/*pointer to a style of the text block, which dictates the font, color, and shadow options*/
			SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)
		SLATE_END_ARGS()

		/*Constructor*/
		STextBlock();

		/*Destructor*/
		~STextBlock();

		/*
		 * construct this widget
		 *
		 * @param InArgs the declaration data for this widget
		 */
		void Construct(const FArguments& in_args);
	public:
		//------SWidget interface------
		int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;
		//------SWidget interface------

	public:
		/*sets the text for this text block*/
		void set_text(TAttribute<DoDoUtf8String> in_text);
	private:
		/*gets the current font*/
		FSlateFontInfo get_font() const;
	private:
		/*the text displayed in this text block*/
		TSlateAttribute<DoDoUtf8String> m_bound_text;

		//todo:implement FSlateTextBlockLayout, wrapped layout for this text block

		//todo:implement FTextBlockStyle, default style used by the TextLayout
		/*default style used by the text layout*/
		FTextBlockStyle m_text_style;

		//todo:implement FSlateFontInfo, sets the font used to draw the text

		/*sets the brush used to strike through the text*/
		TSlateAttribute<const FSlateBrush*> m_strike_brush;

		/*text color and opacity*/
		TSlateAttribute<glm::vec4> m_color_and_opacity;

		/*drop shadow offset in pixels*/
		TSlateAttribute<glm::vec2> m_shadow_offset;

		/*shadow color and opacity*/
		TSlateAttribute<glm::vec4> m_shadow_color_and_opacity;

		/*the color used to highlight the specified text*/
		TSlateAttribute<glm::vec4> m_highlight_color;

		/*the brush used to highlight the specified text*/
		TSlateAttribute<const FSlateBrush*> m_high_light_shape;

		/*HighLight this text in the TextBlock*/
		TSlateAttribute<DoDoUtf8String> m_highlight_text;

		/*whether text wraps onto a new line when it's length exceeds this width, if this value is zero or negative, no wrapping occurs*/
		TSlateAttribute<float> m_wrap_text_at;

		/*true if we're wrapping text automatically based on the computed horizontal space for this widget*/
		TSlateAttribute<bool> m_auto_wrap_text;

		/*the wrapping policy we're using*/
		//todo:implement ETextWrappingPolicy

		/*the transform policy we're using*/
		//todo:implement ETextTransformPolicy

		/*the amount of blank space left around the edges of text area*/
		TSlateAttribute<FMargin> m_margin;

		/*the amount to scale each lines height by*/
		//todo:implement ETextJustify

		/*how the text should be aligned with the margin*/
		TSlateAttribute<float> m_line_height_percentage;

		/*prevents the text block from being smaller than desired in certain cases (e.g. when it is empty)*/
		TSlateAttribute<float> m_min_desired_width;

		/*if this is enabled, text shaping, wrapping, justification are disabled in favor or must faster text layout and measurement*/
		mutable  std::optional<glm::vec2> m_cached_simple_desired_size;

		//todo:add flags to check if the slate attribute is set


		bool m_b_simple_text_mode;
	};
}