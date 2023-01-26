#pragma once

namespace DoDo {
	
	class Window;
	struct FGenericWindowDefinition;
	/*
	* generic platform application interface
	*/
	class GenericApplication
	{
	public:
		GenericApplication();

		virtual ~GenericApplication();

		virtual void Tick(const float time_delta);

		virtual std::shared_ptr<Window> make_window();

		virtual void initialize_window(const std::shared_ptr<Window>& window, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<Window>& in_parent, const bool b_show_immediately) {}

		static std::shared_ptr<GenericApplication> Create();
	};
}