#pragma once

#include "SlateCore/Input/DragAndDrop.h"

namespace DoDo {
	/*a sample implementation of IDragDropOpation*/
	class FDockingDragOperation : public FDragDropOperation
	{
	public:
		DRAG_DROP_OPERATE_TYPE(FDockingDragOperation, FDragDropOperation)


	};
}