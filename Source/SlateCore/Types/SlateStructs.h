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

	/*
	* structure for optional floating point sizes
	*/
	struct FOptionalSize
	{
		/*
		* creates an unspecified size
		*/
		FOptionalSize()
			: m_size(m_unspecified)
		{}

		/*
		* creates a size with the specified value
		* 
		* @param SpecifiedSize the size to set
		*/
		FOptionalSize(const float specified_size)
			: m_size(specified_size)
		{}

		/*
		* compare one optional size to another for equality
		*/
		bool operator==(const FOptionalSize& other) const
		{
			return (m_size == other.m_size);
		}

		/*
		* checks whether the size is set
		* 
		* @return true if the size is set, false if it is unspecified
		* 
		* @see get
		*/
		bool is_set() const
		{
			return m_size != m_unspecified;
		}

		/*
		* gets the value of the size
		* 
		* before calling this method, check with is set() whether the size is actually specified
		* unspecified sizes a value of -1.0f will be returned
		* 
		* @see is set
		*/
		float get() const
		{
			return m_size;
		}
	private:
		//constant for unspecified sizes
		static const float m_unspecified;

		//holds the size, if specified
		float m_size;
	};
}
