#pragma once

namespace DoDo
{
#define SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType) \
	{\
		using WidgetArgsType = SlotType::FSlotArguments;\
		using SlotParentType::FSlotArguments::FSlotArguments;

#define SLATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType) \
		public:\
		struct FSlotArguments : public SlotParentType::FSlotArguments \
		SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType)

#define SLATE_SLOT_BEGIN_ARGS_OneMixin(SlotType, SlotParentType, Mixin1) \
		public:\
		struct FSlotArguments : public SlotParentType::FSlotArguments, public Mixin1::FSlotArgumentsMixin \
		SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType)

#define SLATE_SLOT_BEGIN_ARGS_TwoMixins(SlotType, SlotParentType, Mixin1, Mixin2) \
		public:\
		struct FSlotArguments : public SlotParentType::FSlotArguments, public Mixin1::FSlotArgumentsMixin, public Mixin2::FSlotArgumentsMixin \
		SLATE_PRIVATE_SLOT_BEGIN_ARGS(SlotType, SlotParentType)

#define SLATE_SLOT_END_ARGS() \
	};
}
