#pragma once

#include "SlateCore/Widgets/SLeafWidget.h"//SLeafWidget depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "SlateCore/Styling/AppStyle.h"//FAppStyle

#include "SlateCore/Styling/ISlateStyle.h"//ISlateStyle

namespace DoDo {
	enum class EColorBlockAlphaDisplayMode : uint8_t
	{
		//draw a single block that draws color and opacity as one, i.e. the entire block will be semi-transparent if opacity < 1
		Combined,

		//the color block is split into in half, the left half draws the color with opacity and the right half draws without any opacity
		Separate,

		//alpha is omitted from display
		Ignore
	};
	struct FSlateBrush;
	struct FSlateGradientStop;
	class SColorBlock : public SLeafWidget
	{
		SLATE_DECLARE_WIDGET(SColorBlock, SLeafWidget)

	public:

		SLATE_BEGIN_ARGS(SColorBlock)
			: _Color(FLinearColor::White)
			, _AlphaBackgroundBrush(FAppStyle::get().get_brush("ColorPicker.AlphaBackground"))
			, _CornerRadius(0.0f)
			, _ColorIsHSV(false)
			, _ShowBackgroundForAlpha(false)
			, _UseSRGB(true)
			, _AlphaDisplayMode(EColorBlockAlphaDisplayMode::Combined)
			, _Size(glm::vec2(16, 16))
			, _OnMouseButtonDown()
		{}

		/*the color to display for this color block*/
		SLATE_ATTRIBUTE(FLinearColor, Color)

		/*background to display for when there is a color with transparency, irrelevant if ignoring alpha*/
		SLATE_ATTRIBUTE(const FSlateBrush*, AlphaBackgroundBrush)

		/*rounding to apply to the corners of the block*/
		SLATE_ATTRIBUTE(glm::vec4, CornerRadius)

		/*whether the color displayed is hsv or not*/
		SLATE_ATTRIBUTE(bool, ColorIsHSV)

		/*whether to display a background for viewing opacity, irrelevant if ignoring alpha*/
		SLATE_ATTRIBUTE(bool, ShowBackgroundForAlpha)

		/*whether to display sRGB color*/
		SLATE_ATTRIBUTE(bool, UseSRGB)

		/*how the color block displays color and opacity*/
		SLATE_ATTRIBUTE(EColorBlockAlphaDisplayMode, AlphaDisplayMode)
		
		/*how big should this color block be?*/
		SLATE_ATTRIBUTE(glm::vec2, Size)

		/*a handler to activate when the mouse is pressed*/
		SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)

		SLATE_END_ARGS()

	public:
		SColorBlock();

		/*
		* construct this widget
		* 
		* @param InArgs the declaration data for this widget
		*/
		void Construct(const FArguments& in_args);

	private:
		//SWidget overrides
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;

		void make_section(std::vector<FSlateGradientStop>& out_gradient_stops, glm::vec2 start_pt, glm::vec2 end_pt, FLinearColor color, const FWidgetStyle& in_widget_style, bool b_ignore_alpha) const;

	private:
		/*the color to display for this color block*/
		TSlateAttribute<FLinearColor> m_color;

		TSlateAttribute<const FSlateBrush*> m_alpha_background_brush;

		TSlateAttribute<glm::vec4> m_gradient_corner_radius;

		TSlateAttribute<glm::vec2> m_color_block_size;

		/*a handler to activate when the mouse is pressed*/
		FPointerEventHandler m_mouse_button_down_handler;

		/*whether to ignore alpha entirely from the input color*/
		TSlateAttribute<EColorBlockAlphaDisplayMode> m_alpha_display_mode;

		/*whether the color displayed is hsv or not*/
		TSlateAttribute<bool> m_color_is_hsv;

		/*whether to display a background for viewing opacity, irrelevant if ignoring alpha*/
		TSlateAttribute<bool> m_show_background_for_alpha;

		/*whether to display sRGB color*/
		TSlateAttribute<bool> m_b_use_srgb;
	};
}