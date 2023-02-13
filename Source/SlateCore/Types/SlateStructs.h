#pragma once

#include "Core/Misc/Attribute.h"

namespace DoDo
{
	/*
	 * base structure for size parameters
	 *
	 * describes a way in which a parent widget allocates available space to it's child widgets
	 *
	 * when size rule is SizeRule_Auto, the widget's desired size will be used as the space required
	 * when size rule is SizeRule_Stretch, the available space will be distributed proportionately between
	 * peer widgets depending on the value property, available space is space remaining after all the
	 * peer's SizeRule_Auto requirements have been satisfied
	 *
	 * FSizeParam cannot be constructed directly - see FStretch, and FAspectRation
	 */
	struct FSizeParam
	{
		enum ESizeRule
		{
			SizeRule_Auto,
			SizeRule_Stretch
		};

		/*the sizing rule to use*/
		ESizeRule m_size_rule;

		/*
		 * the actual value this size parameter stores
		 *
		 * this value can be driven by a delegate, it is only used for the stretch mode
		 */
		TAttribute<float> m_value;

	protected:
		/*
		 * hidden constructor
		 *
		 * use FAspectRatio, FAuto, FStretch to instantiate size parameters
		 *
		 * @see FAspectRatio, FAuto, FStretch
		 */
		FSizeParam(ESizeRule in_type_of_size, const TAttribute<float>& in_value)
			: m_size_rule(in_type_of_size)
			, m_value(in_value)
		{}
	};

	/*
	 * structure for size parameters with SizeRule - SizeRule_Stretch
	 *
	 * @see FAspectRatio
	 * @see FAuto
	 */
	struct FStretch
		: public FSizeParam
	{
		FStretch(const TAttribute<float>& stretch_amout)
			: FSizeParam(SizeRule_Stretch, stretch_amout)
		{}

		FStretch()
			: FSizeParam(SizeRule_Stretch, 1.0f)
		{}
	};

	/*
	 * structure for size parameters with SizeRule - SizeRule_Auto
	 *
	 * @see FAspectRatio, FStretch
	 */
	struct FAuto
		: public FSizeParam
	{
		FAuto()
			: FSizeParam(SizeRule_Auto, 0.0f)
		{}
	};
}
