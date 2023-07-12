#pragma once

#include "SlateCore/Widgets/SPanel.h"//SPanel depends on it

#include "SlateCore/SlotBase.h"

#include "SlateCore/Layout/Children.h"//TPanelChildren depends on it

namespace DoDo {
	/*arrange 3 widgets, the top track, bottom track, and thumb*/
	class SScrollBarTrack : public SPanel
	{
	public:
		/*a list panel slot is very simple, it just stores a widget*/
		class FSlot : public TSlotBase<FSlot>
		{
		public:
			FSlot()
				: TSlotBase<FSlot>()
			{}
		};

		SScrollBarTrack()
			: m_children(this)
		{}

		SLATE_BEGIN_ARGS(SScrollBarTrack)
			: _TopSlot()
			, _ThumbSlot()
			, _BottomSlot()
			, _Orientation(Orient_Vertical)
		{

		}

			SLATE_NAMED_SLOT(FArguments, TopSlot)
				SLATE_NAMED_SLOT(FArguments, ThumbSlot)
				SLATE_NAMED_SLOT(FArguments, BottomSlot)
				SLATE_ARGUMENT(EOrientation, Orientation)
		SLATE_END_ARGS()

		/*
		* construct the widget from a declaration
		* 
		* @param InArgs declaration from which to construct the widget
		*/
		void Construct(const FArguments& in_args);

		virtual void On_ArrageChildren(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const;

		float distance_from_top() const;

	protected:
		static const int32_t TOP_SLOT_INDEX = 0;
		static const int32_t BOTTOM_SLOT_INDEX = 1;
		static const int32_t THUMB_SLOT_INDEX = 2;

		TPanelChildren<FSlot> m_children;

		float m_offset_fraction;
		float m_thumb_size_fraction;
		float m_min_thumb_size;
		bool m_b_is_always_visible = false;
		EOrientation m_orientation;
	};
}