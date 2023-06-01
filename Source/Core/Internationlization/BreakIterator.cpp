#include "PreCompileHeader.h"

#include "BreakIterator.h"

#include "ICUBreakIterator.h"

#include "Internationalization.h"

namespace DoDo {
	std::shared_ptr<IBreakIterator> FBreakIterator::create_character_boundary_iterator()
	{
		FInternationalization::get();//todo:fix this
		return std::make_shared<FICUBreakIterator>(FICUBreakIteratorManager::get().create_character_boundary_iterator());
		//return nullptr;
	}
}