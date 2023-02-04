#pragma once

namespace DoDo {

	//shift tab
	/*
	 * FModifierKeyState stores the pressed state of keys that are commonly used as modifiers
	 */
	class FModifierKeyState
	{
	public:
		FModifierKeyState(const bool b_in_is_left_shift_down,
							const bool b_in_is_is_right_shift_down,
							const bool b_in_is_left_control_down,
							const bool b_in_is_right_control_down,
							const bool b_in_is_left_alt_down,
							const bool b_in_is_right_alt_down,
							const bool b_in_is_left_command_down,
							const bool b_in_is_right_command_down,
							const bool b_in_are_caps_locked)
		: m_b_is_left_shift_down(b_in_is_left_shift_down)
		, m_b_is_right_shift_down(b_in_is_is_right_shift_down)
		, m_b_is_left_control_down(b_in_is_left_control_down)
		, m_b_is_right_control_down(b_in_is_right_control_down)
		, m_b_is_left_alt_down(b_in_is_left_alt_down)
		, m_b_is_right_alt_down(b_in_is_right_alt_down)
		, m_b_are_caps_locked(b_in_are_caps_locked)
		{
			
		}

		FModifierKeyState()
			: m_b_is_left_shift_down(false)
			, m_b_is_right_shift_down(false)
			, m_b_is_left_control_down(false)
			, m_b_is_right_control_down(false)
			, m_b_is_left_alt_down(false)
			, m_b_is_right_alt_down(false)
			, m_b_is_left_command_down(false)
			, m_b_is_right_command_down(false)
			, m_b_are_caps_locked(false)
		{
			
		}
	private:
		/*true if the left shift key was down when this event occurred*/
		uint16_t m_b_is_left_shift_down : 1;

		/*true if the right shift key was down when this event occurred*/
		uint16_t m_b_is_right_shift_down : 1;

		/*true if the left control key was down when this event occurred*/
		uint16_t m_b_is_left_control_down : 1;

		/*true if the right control key was down when this event occurred*/
		uint16_t m_b_is_right_control_down : 1;

		/*true if the left alt key was down when this event occurred*/
		uint16_t m_b_is_left_alt_down : 1;

		/*true if the right alt key was down when this event occurred*/
		uint16_t m_b_is_right_alt_down : 1;

		/*true if the left command key was down when this event occurred*/
		uint16_t m_b_is_left_command_down : 1;

		/*true if the right command key was down when this event occurred*/
		uint16_t m_b_is_right_command_down : 1;

		/*true if the cas lock key has been toggled to the enabled state*/
		uint16_t m_b_are_caps_locked : 1;
	};

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

	protected:
		//todo:implement FGenericApplicationMessageHandler
	};
}