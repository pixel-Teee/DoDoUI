#include "PreCompileHeader.h"

#include "SListPanel.h"

namespace DoDo {
	SListPanel::FSlot::FSlotArguments SListPanel::Slot()
	{
		return FSlot::FSlotArguments(std::make_unique<FSlot>());
	}
	void SListPanel::Construct(const FArguments& in_args)
	{
	}
	void SListPanel::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
	}
}