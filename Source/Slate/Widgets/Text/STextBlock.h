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
			, _Font()
			, _ColorAndOpacity()
			{}

			/*the text displayed in this text block*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Text)

			/*sets the font used to draw the text*/
			SLATE_ATTRIBUTE(FSlateFontInfo, Font)

			/*pointer to a style of the text block, which dictates the font, color, and shadow options*/
			SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)

			/*text color and opacity*/
			SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
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

		/*
		 * gets the text assigned to this text block
		 *
		 * @return this text block's text string
		 */
		const DoDoUtf8String& get_text() const
		{
			if(m_b_is_attribute_bound_text_bound)
			{
				STextBlock& mutable_self = const_cast<STextBlock&>(*this);
				mutable_self.m_bound_text.update_now(mutable_self);//todo:implement update now
			}
			return m_bound_text.Get();
		}

		/*gets the current foreground color*/
		FSlateColor get_color_and_opacity() const;
	public:
		//------SWidget interface------
		int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;
		//------SWidget interface------

	public:
		/*sets the text for this text block*/
		void set_text(TAttribute<DoDoUtf8String> in_text);

		/*sets the font used to draw the text*/
		void set_font(TAttribute<FSlateFontInfo> in_font);

		/*see color and opacity attribute*/
		void set_color_and_opacity(TAttribute<FSlateColor> in_color_and_opacity);
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
		TSlateAttribute<FSlateFontInfo> m_font;

		/*sets the brush used to strike through the text*/
		TSlateAttribute<const FSlateBrush*> m_strike_brush;

		/*text color and opacity*/
		TSlateAttribute<FSlateColor> m_color_and_opacity;

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
		union
		{
			struct
			{
				uint16_t m_b_is_attribute_bound_text_bound : 1;
				uint16_t m_b_is_attribute_font_set : 1;
				uint16_t m_b_is_attribute_color_and_opacity_set : 1;
			};
			uint16_t m_union_flags;
		};

		bool m_b_simple_text_mode;
	};
}