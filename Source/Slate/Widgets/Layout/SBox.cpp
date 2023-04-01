#include <PreCompileHeader.h>

#include "SBox.h"

#include "SlateCore/Layout/ArrangedChildren.h"//FArrangedChildren depends on it

#include "SlateCore/Layout/LayoutUtils.h"

#include "SlateCore/Types/PaintArgs.h"//FPaintArgs depends on it

namespace DoDo {
    void SBox::Private_Register_Attributes(FSlateAttributeInitializer&)
    {

    }

    SBox::SBox()
        : m_child_slot(this)
        , m_width_override(*this)
        , m_height_override(*this)
        , m_min_desired_width(*this)
        , m_min_desired_height(*this)
        , m_max_desired_width(*this)
        , m_max_desired_height(*this)
        , m_min_aspect_ratio(*this)
        , m_max_aspect_ratio(*this)
    {

    }

    void SBox::Construct(const FArguments& in_args)
    {
        set_width_override(in_args._WidthOverride);
        set_height_override(in_args._HeightOverride);

        set_min_desired_width(in_args._MinDesiredWidth);
        set_min_desired_height(in_args._MinDesiredHeight);

        set_max_desired_width(in_args._MaxDesiredWidth);
        set_max_desired_height(in_args._MaxDesiredHeight);

        set_min_aspect_ratio(in_args._MinAspectRatio);
        set_max_aspect_ratio(in_args._MaxAspectRatio);

        m_child_slot
            .HAlign(in_args._HAlign)
            .VAlign(in_args._VAlign)
            .Padding(in_args._Padding)
            [
                in_args._Content.m_widget
            ];
    }
    void SBox::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
    {
        const EVisibility child_visibility = m_child_slot.get_widget()->get_visibility();

        if (arranged_children.Accepts(child_visibility))
        {
            const FOptionalSize current_min_aspect_ratio = m_min_aspect_ratio.Get();
            const FOptionalSize current_max_aspect_ratio = m_max_aspect_ratio.Get();

            const FMargin slot_padding(m_child_slot.get_padding());

            bool b_align_children = true;

            AlignmentArrangeResult x_alignment_result(0, 0);
            AlignmentArrangeResult y_alignment_result(0, 0);

            if (current_max_aspect_ratio.is_set() || current_min_aspect_ratio.is_set())
            {
                float current_width = std::min(allotted_geometry.m_size.x, m_child_slot.get_widget()->get_desired_size().x);
                float current_height = std::min(allotted_geometry.m_size.y, m_child_slot.get_widget()->get_desired_size().y);

                float min_aspect_ratio_width = current_min_aspect_ratio.is_set() ? current_min_aspect_ratio.get() : 0.0f;
                float max_aspect_ratio_width = current_max_aspect_ratio.is_set() ? current_max_aspect_ratio.get() : 0.0f;

                if (current_height > 0 && current_width > 0)
                {
                    const float current_ratio_width = (allotted_geometry.get_local_size().x / allotted_geometry.get_local_size().y);

                    bool b_fit_max_ratio = (current_ratio_width > max_aspect_ratio_width && max_aspect_ratio_width != 0.0f);
                    bool b_fit_min_ratio = (current_ratio_width < min_aspect_ratio_width && min_aspect_ratio_width != 0.0f);

                    if (b_fit_max_ratio || b_fit_min_ratio)
                    {
                        x_alignment_result = Align_Child<Orient_Horizontal>(allotted_geometry.get_local_size().x, m_child_slot, slot_padding);
                        y_alignment_result = Align_Child<Orient_Vertical>(allotted_geometry.get_local_size().y, m_child_slot, slot_padding);

                        float new_width;
                        float new_height;

                        if (b_fit_max_ratio)
                        {
                            const float max_aspect_ratio_height = 1.0f / max_aspect_ratio_width;
                            new_width = max_aspect_ratio_width * x_alignment_result.m_size;
                            new_height = max_aspect_ratio_height * new_width;
                        }
                        else
                        {
                            const float min_aspect_ratio_height = 1.0f / min_aspect_ratio_width;
                            new_width = min_aspect_ratio_width * x_alignment_result.m_size;
                            new_height = min_aspect_ratio_height * new_width;
                        }

                        const float max_width = allotted_geometry.m_size.x - slot_padding.Get_Total_Space_Along<Orient_Horizontal>();
                        const float max_height = allotted_geometry.m_size.y - slot_padding.Get_Total_Space_Along<Orient_Vertical>();

                        if (new_width > max_width)
                        {
                            float scale = max_width / new_width;
                            new_width *= scale;
                            new_height *= scale;
                        }

                        if (new_height > max_height)
                        {
                            float scale = max_height / new_height;
                            new_width *= scale;
                            new_height *= scale;
                        }

                        x_alignment_result.m_size = new_width;
                        y_alignment_result.m_size = new_height;

                        b_align_children = false;
                    }
                }
            }

            if (b_align_children)
            {
				x_alignment_result = Align_Child<Orient_Horizontal>(allotted_geometry.get_local_size().x, m_child_slot, slot_padding);
				y_alignment_result = Align_Child<Orient_Vertical>(allotted_geometry.get_local_size().y, m_child_slot, slot_padding);
            }

            const float aligned_size_x = x_alignment_result.m_size;
            const float aligned_size_y = y_alignment_result.m_size;

            arranged_children.add_widget(
                allotted_geometry.make_child(
                    m_child_slot.get_widget(),
                    glm::vec2(x_alignment_result.m_offset, y_alignment_result.m_offset),
                    glm::vec2(aligned_size_x, aligned_size_y)));
        }
    }
    FChildren* SBox::Get_Children()
    {
        return &m_child_slot;
    }
    int32_t SBox::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
    {
       //an sbox just draws its only child
        FArrangedChildren arranged_children(EVisibility::visible);
        this->Arrange_Children(allotted_geometry, arranged_children);

        //maybe none of our children are visible
        if (arranged_children.num() > 0)
        {
            FArrangedWidget& the_child = arranged_children[0];

            return the_child.m_widget->paint(args.with_new_parent(this), the_child.m_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style,
                should_be_enabled(b_parent_enabled));
        }
    }
    void SBox::set_halign(EHorizontalAlignment halign)
    {
        m_child_slot.set_horizontal_alignment(halign);
    }
    void SBox::set_valign(EVerticalAlignment valign)
    {
        m_child_slot.set_vertical_alignment(valign);
    }
    void SBox::set_padding(TAttribute<FMargin> in_padding)
    {
        m_child_slot.set_padding(in_padding);
    }
    void SBox::set_width_override(TAttribute<FOptionalSize> in_width_override)
    {
        m_width_override.Assign(*this, in_width_override);
    }
    void SBox::set_height_override(TAttribute<FOptionalSize> in_height_override)
    {
        m_height_override.Assign(*this, in_height_override);
    }
    void SBox::set_min_desired_width(TAttribute<FOptionalSize> in_min_desired_width)
    {
        m_min_desired_width.Assign(*this, in_min_desired_width);
    }
    void SBox::set_min_desired_height(TAttribute<FOptionalSize> in_min_desired_height)
    {
        m_max_desired_height.Assign(*this, in_min_desired_height);
    }
    void SBox::set_max_desired_width(TAttribute<FOptionalSize> in_max_desired_width)
    {
        m_max_desired_width.Assign(*this, in_max_desired_width);
    }
    void SBox::set_max_desired_height(TAttribute<FOptionalSize> in_max_desired_height)
    {
        m_max_desired_height.Assign(*this, in_max_desired_height);
    }
    void SBox::set_min_aspect_ratio(TAttribute<FOptionalSize> in_min_aspect_ratio)
    {
        m_min_aspect_ratio.Assign(*this, in_min_aspect_ratio);
    }
    void SBox::set_max_aspect_ratio(TAttribute<FOptionalSize> in_max_aspect_ratio)
    {
        m_max_aspect_ratio.Assign(*this, in_max_aspect_ratio);
    }
    glm::vec2 SBox::Compute_Desired_Size(float Layout_Scale_Multiplier) const
    {
        EVisibility child_visibility = m_child_slot.get_widget()->get_visibility();

        if (child_visibility != EVisibility::Collapsed)
        {
            const FOptionalSize current_width_override = m_width_override.Get();
            const FOptionalSize current_height_override = m_height_override.Get();

            return glm::vec2(
                (current_width_override.is_set()) ? current_width_override.get() : compute_desired_width(),
                (current_height_override.is_set()) ? current_height_override.get() : compute_desired_height()
            );
        }

        return glm::vec2(0.0f);
    }

    float SBox::compute_desired_width() const
    {
        //if the user specified a fixed width or height, those values override the box's content
        const glm::vec2& un_modified_child_desired_size = m_child_slot.get_widget()->get_desired_size() + m_child_slot.get_padding().get_desired_size();
        const FOptionalSize current_min_desired_width = m_min_desired_width.Get();
        const FOptionalSize current_max_desired_width = m_max_desired_width.Get();

        float current_width = un_modified_child_desired_size.x;

        if (current_min_desired_width.is_set())
        {
            current_width = std::max(current_width, current_min_desired_width.get());
        }

        if (current_max_desired_width.is_set())
        {
            current_width = std::min(current_width, current_max_desired_width.get());
        }

        return current_width;
    }

    float SBox::compute_desired_height() const
    {
		//if the user specified a fixed height or height, those values override the box's content
		const glm::vec2& un_modified_child_desired_size = m_child_slot.get_widget()->get_desired_size() + m_child_slot.get_padding().get_desired_size();
		const FOptionalSize current_min_desired_height = m_min_desired_height.Get();
		const FOptionalSize current_max_desired_height = m_max_desired_height.Get();

		float current_height = un_modified_child_desired_size.y;

		if (current_min_desired_height.is_set())
		{
			current_height = std::max(current_height, current_min_desired_height.get());
		}

		if (current_max_desired_height.is_set())
		{
			current_height = std::min(current_height, current_max_desired_height.get());
		}

		return current_height;
    }


}