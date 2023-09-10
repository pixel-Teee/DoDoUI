#include "PreCompileHeader.h"

#include "SExpanderArrow.h"

#include "SlateCore/Widgets/Images/SImage.h"//SImage depends on it

#include "Slate/Widgets/Views/STableRow.h"

namespace DoDo {
	//todo:register should draw wires
	SLATE_IMPLEMENT_WIDGET(SExpanderArrow)
	void SExpanderArrow::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(attribute_initializer, m_b_show_draw_wires, EInvalidateWidgetReason::Paint);
	}

	SExpanderArrow::SExpanderArrow()
		: m_b_show_draw_wires(*this, false)
	{

	}

	void SExpanderArrow::Construct(const FArguments& in_args, const std::shared_ptr<ITableRow>& table_row)
	{
		m_owner_row_ptr = table_row;
		m_style_set = in_args._StyleSet;
		m_indent_amount = in_args._IndentAmount;
		m_base_indent_level = in_args._BaseIndentLevel;
		m_b_show_draw_wires.Assign(*this, in_args._ShouldDrawWires);

		this->m_child_slot
		.Padding(TAttribute<FMargin>(this, &SExpanderArrow::get_expander_padding))
		[
			SAssignNew(m_expander_arrow, SButton)
			.ButtonStyle(FCoreStyle::get(), "NoBorder")
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Visibility(this, &SExpanderArrow::get_expander_visibility)
			.OnClicked(this, &SExpanderArrow::on_arrow_clicked)
			.ContentPadding(0.0f)
			.ForegroundColor(FSlateColor::use_foreground())
			[
				SNew(SImage)
				.Image(this, &SExpanderArrow::get_expander_image)
				.ColorAndOpacity(FSlateColor::use_foreground())
			]
		];
	}
	int32_t SExpanderArrow::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		static const float wire_thickness = 2.0f;
		static const float half_wire_thickness = wire_thickness / 2.0f;

		//we want to support drawing wires for the tree
		//				Needs Wire Array
		// v-[A]				{}
		// |-v[B]				{1}
		// | '-v[B]				{1, 1}
		// |	|--[C]			{1, 0, 1}
		// |	|--[D]			{1, 0, 1}
		// |	'--[E]			{1, 0, 1}
		// |>-[F]				{}
		// '--[G]				{}

		static const DoDoUtf8String name_vertical_bar_brush = "WhiteBrush";
		const float indent = m_indent_amount.Get(10.0f);
		const FSlateBrush* vertical_bar_brush = (m_style_set == nullptr) ? nullptr : m_style_set->get_brush(name_vertical_bar_brush);

		if (m_b_show_draw_wires.Get() == true && vertical_bar_brush != nullptr)
		{

		}

		layer_id = SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);
		return layer_id;
	}
	EVisibility SExpanderArrow::get_expander_visibility() const
	{
		return m_owner_row_ptr.lock()->does_item_have_children() ? EVisibility::visible : EVisibility::Hidden;
	}
	FMargin SExpanderArrow::get_expander_padding() const
	{
		const int32_t nesting_depth = std::max(0, m_owner_row_ptr.lock()->get_indent_level() - m_base_indent_level.Get());

		const float indent = m_indent_amount.Get(10.0f);

		return FMargin(nesting_depth * indent, 0, 0, 0);
	}
	FReply SExpanderArrow::on_arrow_clicked()
	{
		//recurse the expansion if "shift" is being pressed
		//todo:add shift logic

		m_owner_row_ptr.lock()->toggle_expansion();

		return FReply::handled();
	}
	const FSlateBrush* SExpanderArrow::get_expander_image() const
	{
		const bool b_is_item_expanded = m_owner_row_ptr.lock()->is_item_expanded();

		DoDoUtf8String resource_name;
		if (b_is_item_expanded)
		{
			if (m_expander_arrow->is_hovered())
			{
				static DoDoUtf8String expanded_hovered_name = "TreeArrow_Expanded_Hovered";
				resource_name = expanded_hovered_name;
			}
			else
			{
				static DoDoUtf8String expanded_name = "TreeArrow_Expanded";
				resource_name = expanded_name;
			}
		}
		else
		{
			if (m_expander_arrow->is_hovered())
			{
				static DoDoUtf8String collapsed_hovered_name = "TreeArrow_Collapsed_Hovered";
				resource_name = collapsed_hovered_name;
			}
			else
			{
				static DoDoUtf8String collapsed_name = "TreeArrow_Collapsed";
				resource_name = collapsed_name;
			}
		}

		return m_style_set->get_brush(resource_name);
	}
}