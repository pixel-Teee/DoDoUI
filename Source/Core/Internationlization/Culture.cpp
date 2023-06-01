#include "PreCompileHeader.h"

#include "Culture.h"



namespace DoDo {


	FCulture::FCulture()
	{
	}

	FCulture::~FCulture()
	{
	}


	std::shared_ptr<FCulture> FCulture::create(std::unique_ptr<FICUCultureImplementation>&& in_implementation)
	{
		return std::make_shared<FCulture>(std::move(in_implementation));
	}
	FCulture::FCulture(std::unique_ptr<FICUCultureImplementation>&& in_implementation)
	: m_implementation(std::move(in_implementation))
	{
		
	}

}