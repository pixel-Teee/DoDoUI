#pragma once

namespace DoDo {
	/*
	* generic platform application interface
	*/
	class GenericApplication
	{
	public:
		GenericApplication();

		virtual ~GenericApplication();

		virtual void Tick(const float time_delta);

		static std::shared_ptr<GenericApplication> Create();
	};
}