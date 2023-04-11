#include "PreCompileHeader.h"

#include "SDockingTarget.h"

#include "SlateCore/Widgets/Images/SImage.h"

namespace DoDo {
	static const FSlateBrush* border_brush_from_dock_direction(SDockingNode::RelativeDirection dock_direction)
	{
		//pick the appropriate border based on direction
		switch (dock_direction)
		{
		case SDockingNode::LeftOf: return FCoreStyle::get().get_brush("Docking.Cross.BorderLeft"); break;
		case SDockingNode::Above: return FCoreStyle::get().get_brush("Docking.Cross.BorderTop"); break;
		case SDockingNode::RightOf: return FCoreStyle::get().get_brush("Docking.Cross.BorderRight"); break;
		case SDockingNode::Below: return FCoreStyle::get().get_brush("Docking.Cross.BorderBottom"); break;
		default:
		case SDockingNode::Center: return FCoreStyle::get().get_brush("Docking.Cross.BorderCenter"); break;
		}
	}
	void SDockingTarget::Construct(const FArguments& in_args)
	{
		m_owner_node = in_args._OwnerNode;

		m_dock_direction = in_args._DockDirection;

		m_b_is_drag_overed = false;

		//pick the appropriate image based on direction
		switch (m_dock_direction)
		{
		case DoDo::SDockingNode::LeftOf:
			m_target_image = FCoreStyle::get().get_brush("Docking.Cross.DockLeft");
			m_target_image_hovered = FCoreStyle::get().get_brush("Docking.Cross.DockLeft_Hovered");
			break;
		case DoDo::SDockingNode::Above:
			m_target_image = FCoreStyle::get().get_brush("Docking.Cross.DockTop");
			m_target_image_hovered = FCoreStyle::get().get_brush("Docking.Cross.DockTop_Hovered");
			break;
		case DoDo::SDockingNode::RightOf:
			m_target_image = FCoreStyle::get().get_brush("Docking.Cross.DockRight");
			m_target_image_hovered = FCoreStyle::get().get_brush("Docking.Cross.DockRight_Hovered");
			break;
		case DoDo::SDockingNode::Below:
			m_target_image = FCoreStyle::get().get_brush("Docking.Cross.DockBottom");
			m_target_image_hovered = FCoreStyle::get().get_brush("Docking.Cross.DockBottom_Hovered");
			break;
		default:
		case DoDo::SDockingNode::Center:
			m_target_image = FCoreStyle::get().get_brush("Docking.Cross.DockCenter");
			m_target_image_hovered = FCoreStyle::get().get_brush("Docking.Cross.DockCenter_Hovered");
			break;
		}

		SBorder::Construct(SBorder::FArguments()
			.ColorAndOpacity(FCoreStyle::get().get_color("Docking.Cross.Tint"))
			.BorderBackgroundColor(FCoreStyle::get().get_color("Docking.Cross.Tint"))
			.BorderImage(border_brush_from_dock_direction(in_args._DockDirection))
			.HAlign(m_dock_direction == SDockingNode::Center ? HAlign_Center : HAlign_Fill)
			.VAlign(m_dock_direction == SDockingNode::Center ? VAlign_Center : VAlign_Fill)
			[
				SNew(SImage)
				.Image(this, &SDockingTarget::get_target_image)
			]
		);
	}
	void SDockingTarget::On_Drag_Enter(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event)
	{
	}
	void SDockingTarget::On_Drag_Leave(const FDragDropEvent& drag_drop_event)
	{
	}
	FReply SDockingTarget::On_Drop(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event)
	{
		return FReply::un_handled();
	}
	std::shared_ptr<SDockingNode> SDockingTarget::get_owner() const
	{
		return m_owner_node.lock();
	}
	SDockingNode::RelativeDirection SDockingTarget::get_dock_direction() const
	{
		return m_dock_direction;
	}
	const FSlateBrush* SDockingTarget::get_target_image() const
	{
		return (m_b_is_drag_overed) ? m_target_image_hovered : m_target_image;
	}
}