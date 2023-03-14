#include <PreCompileHeader.h>

#include "SNullWidget.h"

#include "SlateCore/Widgets/SWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	class SNullWidgetContent : public SWidget
	{
	public:
		SLATE_BEGIN_ARGS(SNullWidgetContent)
		{
			//_Visibility = EVisibility::SelfHitTestInvisible
			//todo:add visibility
		}
		SLATE_END_ARGS()

		SNullWidgetContent()
		{

		}

		void Construct(const FArguments& in_args)
		{

		}

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override
		{
			return layer_id;
		}

		virtual FChildren* Get_Children() override final
		{
			return nullptr;//todo:fix me
		}

		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const override final
		{
			return glm::vec2(0.0f, 0.0f);
		}

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override final
		{
			//nothing to do
		}

	protected:
	};

	namespace NullWidgetPrivate
	{
		std::shared_ptr<SWidget> Construct()
		{
			static std::shared_ptr<SWidget> Result = SNew(SNullWidgetContent);//todo:add visibility

			return Result;
		}
	}
	std::shared_ptr<SWidget> SNullWidget::NullWidget = NullWidgetPrivate::Construct();
}