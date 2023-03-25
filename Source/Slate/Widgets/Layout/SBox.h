#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/Layout/Children.h"//TSingleWidgetChildrenWithBasicLayoutSlot depends on it

#include "SlateCore/Types/SlateStructs.h"//FOptionalSize depends on it

namespace DoDo {
	/*
	* sbox is the simplest layout element
	*/
	class SBox : public SPanel
	{
		SLATE_DECLARE_WIDGET(SBox, SPanel)

	public:
		SLATE_BEGIN_ARGS(SBox)
			: _HAlign(HAlign_Fill)
			, _VAlign(VAlign_Fill)
			, _Padding(0.0f)
			, _Content()
			, _WidthOverride(FOptionalSize())
			, _HeightOverride(FOptionalSize())
			, _MinDesiredWidth(FOptionalSize())
			, _MinDesiredHeight(FOptionalSize())
			, _MaxDesiredWidth(FOptionalSize())
			, _MaxDesiredHeight(FOptionalSize())
		{
			//add visibility		
		}

		/*horizontal alignment of content in the area allocated to the SBox by its parent*/
		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)

		/*vertical alignment of content in the area allocated to the SBox by its parent*/
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)

		/*padding between the SBox and the content that it presents, padding affects desired size*/
		SLATE_ATTRIBUTE(FMargin, Padding)

		/*the widget content presented by the SBox*/
		SLATE_DEFAULT_SLOT(FArguments, Content)

		/*when specified, ignore the content's desired size and report the width override as the box's desired width*/
		SLATE_ATTRIBUTE(FOptionalSize, WidthOverride)

		/*when specified, ignore the content's desired size and report the height override as the box's desired height*/
		SLATE_ATTRIBUTE(FOptionalSize, HeightOverride)

		/*when specified, will report the min desired with if larger than the content's desired width*/
		SLATE_ATTRIBUTE(FOptionalSize, MinDesiredWidth)

		/*when specified, will report the min desired height if larger than the content's desired height*/
		SLATE_ATTRIBUTE(FOptionalSize, MinDesiredHeight)

		/*when specified, will report the max desired width if smaller than the content's desired width*/
		SLATE_ATTRIBUTE(FOptionalSize, MaxDesiredWidth)

		/*when specified, will report the max desired height if smaller than the content's desired height*/
		SLATE_ATTRIBUTE(FOptionalSize, MaxDesiredHeight)

		SLATE_ATTRIBUTE(FOptionalSize, MinAspectRatio)

		SLATE_ATTRIBUTE(FOptionalSize, MaxAspectRatio)
		SLATE_END_ARGS()

		SBox();

		void Construct(const FArguments& in_args);

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

		virtual FChildren* Get_Children() override;

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		/*set halign argument*/
		void set_halign(EHorizontalAlignment halign);

		/*set valign argument*/
		void set_valign(EVerticalAlignment valign);
		
		/*set padding attribute*/
		void set_padding(TAttribute<FMargin> in_padding);

		/*set width override attribute*/
		void set_width_override(TAttribute<FOptionalSize> in_width_override);

		/*set height override attribute*/
		void set_height_override(TAttribute<FOptionalSize> in_height_override);

		/*set min desired width attribute*/
		void set_min_desired_width(TAttribute<FOptionalSize> in_min_desired_width);

		void set_min_desired_height(TAttribute<FOptionalSize> in_min_desired_height);

		void set_max_desired_width(TAttribute<FOptionalSize> in_max_desired_width);

		/*set max desired height attribute*/
		void set_max_desired_height(TAttribute<FOptionalSize> in_max_desired_height);

		void set_min_aspect_ratio(TAttribute<FOptionalSize> in_min_aspect_ratio);

		void set_max_aspect_ratio(TAttribute<FOptionalSize> in_max_aspect_ratio);
	protected:
		//begin SWidget overrides
		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override;
		float compute_desired_width() const;
		float compute_desired_height() const;
		//end SWidget overrides
	protected:
		struct FBoxOneChildSlot : TSingleWidgetChildrenWithBasicLayoutSlot<EInvalidateWidgetReason::None>
		{
			friend SBox;
			using TSingleWidgetChildrenWithBasicLayoutSlot<EInvalidateWidgetReason::None>::TSingleWidgetChildrenWithBasicLayoutSlot;
		};

		FBoxOneChildSlot m_child_slot;

	private:
		TSlateAttribute<FOptionalSize> m_width_override;

		TSlateAttribute<FOptionalSize> m_height_override;

		TSlateAttribute<FOptionalSize> m_min_desired_width;

		TSlateAttribute<FOptionalSize> m_min_desired_height;

		TSlateAttribute<FOptionalSize> m_max_desired_width;

		TSlateAttribute<FOptionalSize> m_max_desired_height;

		TSlateAttribute<FOptionalSize> m_min_aspect_ratio;

		TSlateAttribute<FOptionalSize> m_max_aspect_ratio;
	};
}