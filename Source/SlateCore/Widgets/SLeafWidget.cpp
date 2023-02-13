#include <PreCompileHeader.h>

#include "SLeafWidget.h"

namespace DoDo
{
    FChildren* SLeafWidget::Get_Children()
    {
        return nullptr;//todo:implement NoChildrenInstance
    }
    void SLeafWidget::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
    {
    }
}
