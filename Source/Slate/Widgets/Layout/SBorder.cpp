#include <PreCompileHeader.h>

#include "SBorder.h"

namespace DoDo
{
	void SBorder::Private_Register_Attributes(FSlateAttributeInitializer&)
	{
		//todo:implement this function
	}

	SBorder::SBorder()
		: m_b_flip_for_right_to_left_flow_direction(false)
	{
	}

	void SBorder::Construct(const FArguments& in_args)
	{
		//set content scale
		Set_Content_Scale(in_args._ContentScale);

		m_child_slot
		.HAlign(in_args._HAlign)
		.VAlign(in_args._VAlign)
		[
			in_args._Content.m_widget
		];
	}
}
