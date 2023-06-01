#pragma once

//#include "Core/Core.h"

#include <memory>
//#include "ICUCulture.h"

#include "ICUCulture.h"//unique_ptr need complete size

//class FICUCultureImplementation;
//typedef FICUCultureImplementation FCultureImplementation;
class FICUCultureImplementation;

namespace DoDo {
	class FCulture
	{
		friend class DoDoUtf8String;
		friend class FICUBreakIteratorManager;

	public:

		FCulture();

		~FCulture();
		
		static std::shared_ptr<FCulture> create(std::unique_ptr<FICUCultureImplementation>&& in_implementation);

		explicit FCulture(std::unique_ptr<FICUCultureImplementation>&& in_implementation);

		std::unique_ptr<FICUCultureImplementation> m_implementation;

	};
}