#pragma once

#include "Core/Math/Color.h"//FLinearColor depends on it
#include "Core/Misc/Attribute.h"//TAttribute depends on it

namespace DoDo
{
	struct FSlateColor;
	/*
	* contains info about those aspects of widget appearance that should be propagated hierarchically
	*/
	class FWidgetStyle
	{
	public:
		/*default constructor*/
		FWidgetStyle()
			: m_color_and_opacity_tint(FLinearColor::White)
			, m_foreground_color(FLinearColor::White)
			, m_subdued_foreground(FLinearColor::White * m_subdued_foreground)//note: * 0.6f
		{

		}

		/*
		 * blends the current tint color with the specified tint
		 *
		 * @param InTint the color to blend with
		 * @return this instance (for method chaining)
		 */
		FWidgetStyle& blend_color_and_opacity_tint(const FLinearColor& in_tint)
		{
			m_color_and_opacity_tint *= in_tint;

			return *this;
		}

		/*
		 * sets the current foreground color from the given slate color attribute
		 *
		 * if the attribute passed in is not set, the foreground will remain unchanged
		 *
		 * @param InForeground the foreground color to set
		 * @return this instance (for method chaining)
		 * @see get foreground color
		 */
		FWidgetStyle& set_foreground_color(const TAttribute<FSlateColor>& in_foreground);

		/*
		 * gets the style's color opacity and tint
		 *
		 * @return current opacity and tint
		 * @see get foreground color, get subdued foreground color
		 */
		const FLinearColor& get_color_and_opacity_tint() const
		{
			return m_color_and_opacity_tint;
		}

		/*
		 * blends the current tint color with the specified alpha
		 *
		 * @param InTint the color to blend with
		 * @return this instance (for method chaining)
		 */
		FWidgetStyle& blend_opacity(float in_opacity)
		{
			m_color_and_opacity_tint.A *= in_opacity;
			return *this;
		}

		/*
		* gets the style's foreground color
		* 
		* @return current foreground color
		* @see get color and opacity tint, get subdued foreground color, set foreground color
		*/
		const FLinearColor& get_foreground_color() const
		{
			return m_foreground_color;
		}

		/*
		* gets the style's subdued color
		* 
		* @return current subdued color
		* @see get color and opacity tint, get foreground color
		*/
		const FLinearColor& get_subdued_foreground_color() const
		{
			return m_subdued_foreground;
		}
	private:
		FLinearColor m_color_and_opacity_tint;
		FLinearColor m_foreground_color;
		FLinearColor m_subdued_foreground;

	private:
		static const float m_subdue_amount;
	};
}
