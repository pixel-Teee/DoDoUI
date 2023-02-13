#pragma once

#include "SlateCore/Styling/CoreStyle.h"
#include "SlateCore/Styling/ISlateStyle.h"//get default brush
#include "SlateCore/Widgets/SLeafWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	/*
	* implements a widget that displays an image with a desired width and height
	*/
	class SImage : public SLeafWidget
	{
		SLATE_DECLARE_WIDGET(SImage, SLeafWidget)

	public:
		SLATE_BEGIN_ARGS(SImage)
			: _Image(FCoreStyle::get().get_default_brush())
			, _ColorAndOpacity(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			, _FlipForRightToLeftFlowDirection(false)
			{}
			/*Image resource*/
			SLATE_ATTRIBUTE(const FSlateBrush*, Image)

			/*color and opacity*/
			SLATE_ATTRIBUTE(glm::vec4, ColorAndOpacity)

			/*flips the image if the localization's flow direction is right to left*/
			SLATE_ARGUMENT(bool, FlipForRightToLeftFlowDirection)
		SLATE_END_ARGS()

		/*Constructor*/
		SImage();

		/*
		* construct this widget
		* 
		* @param InArgs the declaration data for this widget
		*/
		void Construct(const FArguments& in_args);
	protected:
		virtual glm::vec2 Compute_Desired_Size(float) const override;
	public:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect,
			FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
	private:
		/*the slate brush to draw for the ImageAttribute that we can invalidate*/
		TSlateAttribute<const FSlateBrush*> m_image_attribute;

		/*color and opacity scale for this image attribute*/
		TSlateAttribute<glm::vec4> m_color_and_opacity_attribute;

		/*when specified, ignore the content's desired size and report the height override as the box's desired height*/
		TSlateAttribute<std::optional<glm::vec2>> m_desired_size_override_attribute;

	protected:
		/*flips the image if the localization's flow direction is right to left*/
		bool m_b_flip_for_right_to_left_flow_direction;
	};
}