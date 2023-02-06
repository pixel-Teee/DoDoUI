#include <PreCompileHeader.h>

#include "ArrangedWidget.h"

namespace DoDo
{
	const FArrangedWidget& FArrangedWidget::get_null_widget()
	{
		static FArrangedWidget null_arranged_widget(nullptr, FGeometry());

		return null_arranged_widget;
	}

	FWidgetAndPointer::FWidgetAndPointer()
		: FArrangedWidget(FArrangedWidget::get_null_widget())
	{
	}

	FWidgetAndPointer::FWidgetAndPointer(const FArrangedWidget& in_widget)
		: FArrangedWidget(in_widget)
		, m_optional_pointer_position()
	{
	}

	FWidgetAndPointer::FWidgetAndPointer(const FArrangedWidget& in_widget,
		std::optional<FVirtualPointerPosition> in_position)
		: FArrangedWidget(in_widget)
		, m_optional_pointer_position(in_position)
	{
	}
}
