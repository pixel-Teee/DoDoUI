#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"//SCompoundWidget depends on it

namespace DoDo {
	/*
	* struct for holding individual pointers to float values
	*/
	struct FColorChannels
	{
		FColorChannels()
		{
			m_red = m_green = m_blue = m_alpha = nullptr;
		}

		float* m_red;
		float* m_green;
		float* m_blue;
		float* m_alpha;
	};
	/*
	* class for placing a color picker, if all you need is a standalone color picker
	* use the functions OpenColorPicker and DestroyPicker, since they hold a static 
	* instance of the color picker
	*/
	class SColorPicker : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SColorPicker)
		: _TargetColorAttribute(FLinearColor(ForceInit))
		, _TargetFColors()
		, _TargetLinearColors()
		, _UseAlpha(true)
		, _ParentWindow()
		{}
			/*the color that is being targeted as a TAttribute*/
			SLATE_ATTRIBUTE(FLinearColor, TargetColorAttribute)

			/*an array of color pointers this color picker targets*/
			SLATE_ATTRIBUTE(std::vector<FColor*>, TargetFColors)

			/*an array of linear color pointers this color picker targets*/
			SLATE_ATTRIBUTE(std::vector<FLinearColor*>, TargetLinearColors)

			/*whether the ability to pick the alpha value is enabled*/
			SLATE_ATTRIBUTE(bool, UseAlpha)

			/*a pointer to the parent window*/
			SLATE_ATTRIBUTE(std::shared_ptr<SWindow>, ParentWindow)
		SLATE_END_ARGS()
		/*destructor*/
		~SColorPicker();

		/*a default window size for the color picker which looks nice*/
		static const glm::vec2 default_window_size;
	public:
		/*
		* construct the widget
		* 
		* @param InArgs declaration from which to construct the widget
		*/
		void Construct(const FArguments& in_args);

	private:
		/*the color that is being targeted as a TAttribute*/
		TAttribute<FLinearColor> m_target_color_attribute;

		/*the current color being picked in HSV*/
		FLinearColor m_current_color_hsv;

		/*the current color being picked in rgb*/
		FLinearColor m_current_color_rgb;

		/*the old color to be changed in hsv*/
		FLinearColor m_old_color;

		/*color end point to animate to*/
		FLinearColor m_color_end;

		/*color start point to animate from*/
		FLinearColor m_color_begin;

		/*time, used for color animation*/
		float m_current_time;
	};

	struct FColorPickerArgs
	{
		/*the parent for the new color picker window*/
		std::shared_ptr<SWindow> m_parent_widget;

		/*whether or not to enable the alpha slider*/
		bool b_user_alpha;

		/*an array of FColors to target*/
		const std::vector<FColor*>* m_color_array;

		/*an array of FLinearColors to target*/
		const std::vector<FLinearColor*>* m_linear_color_array;

		/*an array of FColorChannels to target.(deprecate now that wx is gone?)*/

		/*overrides the initial color set on the color picker*/
		FLinearColor m_initial_color_override;
	};

	/*open up the static color picker, destroying any previously existing one*/
	bool open_color_picker(const FColorPickerArgs& args);

	/*
	* destroy the current color picker, necessary if the values the color picker
	* currently targets become invalid
	*/
	void destroy_color_picker();
}