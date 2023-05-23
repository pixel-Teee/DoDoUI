#pragma once

#include "ITextLayoutMarshaller.h"

namespace DoDo {
	/*
	* base class implementing some common functionality for all text marshallers
	*/
	class FBaseTextLayoutMarshaller : public ITextLayoutMarshaller
	{
	public:
		virtual ~FBaseTextLayoutMarshaller() {}
	};
}