#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Types/SlateEnums.h"//EHorizontalAlignment

namespace DoDo
{
	struct FSlateBrush;
	/*
	 * a border is an widget that can be used to contain other widgets
	 * it has a border image property, which allows it to take an different appearances
	 * border also has a content() slot as well as some parameters controlling the arrangement of said content
	 */
	class SBorder : public SCompoundWidget
	{
		SLATE_DECLARE_WIDGET(SBorder, SCompoundWidget)
	public:

		//define a FArguments type, first to declare a constructor
		SLATE_BEGIN_ARGS(SBorder)
		: _Content()
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _Padding(FMargin(2.0f))
		, _ContentScale(glm::vec2(1.0f, 1.0f))
		, _DesiredSizeScale(glm::vec2(1.0f, 1.0f))
		, _ColorAndOpacity(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) //todo:need to modify as FLinearColor
		, _BorderBackgroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
		, _ForegroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
		, _ShowEffectWhenDisabled(true)
		, _FlipForRightToLeftFlowDirection(false)
		{}

		SLATE_DEFAULT_SLOT(FArguments, Content)

		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)
		SLATE_ATTRIBUTE(FMargin, Padding)

		//todo:implement FSlateBrush*, border image

		SLATE_ATTRIBUTE(glm::vec2, ContentScale)

		SLATE_ATTRIBUTE(glm::vec2, DesiredSizeScale)

		/*color and opacity is the color and opacity of content in the border*/
		SLATE_ATTRIBUTE(glm::vec4, ColorAndOpacity)

		/*border back ground color refers to the actual color and opacity of the supplied border image*/
		SLATE_ATTRIBUTE(glm::vec4, BorderBackgroundColor)

		/*the foreground color of text and some glyphs that appear as the border's content*/
		SLATE_ATTRIBUTE(glm::vec4, ForegroundColor)

		/*whether or not to show the disabled effect when this border is disabled*/
		SLATE_ATTRIBUTE(bool, ShowEffectWhenDisabled)

		/*flips the background image if the localization's flow direction is RightToLeft*/
		SLATE_ARGUMENT(bool, FlipForRightToLeftFlowDirection)

		SLATE_END_ARGS()

		/*
		 * default constructor
		 */
		SBorder();

		/*
		 * construct this widget
		 *
		 * @param InArgs The declaration data for this widget
		 */
		void Construct(const FArguments& in_args);

		/*sets the color and opacity of the background image for this border*/
		void set_border_back_ground_color(TAttribute<glm::vec4> in_color_and_opacity);

		glm::vec4 get_border_back_ground_color() const { return m_border_back_ground_color_attribute.Get(); }

		void set_padding(TAttribute<FMargin> in_padding);

		/*see BorderImage attribute*/
		void set_border_image(TAttribute<const FSlateBrush*> in_border_image);

		/*
		 * sets the content for border
		 *
		 * @param InContent the widget to use as content for the border
		 */
	public:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

	private:
		//todo:implement FSlateBrush
		TSlateAttribute<const FSlateBrush*> m_border_image_attribute;

		//todo:implement FSlateColor
		TSlateAttribute<glm::vec4> m_border_back_ground_color_attribute;//control back ground color

		TSlateAttribute<glm::vec2> m_desired_size_scale_attribute;

		/*whether or not to show the disabled effect when this border is disabled*/
		TSlateAttribute<bool> m_show_disabled_effect_attribute;

		/*flips the image if the localization's flow direction is right to left*/
		bool m_b_flip_for_right_to_left_flow_direction;
	};
}
