#pragma once

namespace DoDo {
	class FGenericApplicationMessageHandler;

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

	struct FPlatformRect
	{
		int32_t left;
		int32_t top;
		int32_t right;
		int32_t bottom;

		FPlatformRect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}

		FPlatformRect(int32_t in_left, int32_t in_top, int32_t in_right, int32_t in_bottom) : left(in_left), top(in_top), right(in_right), bottom(in_bottom) { }

		bool operator==(const FPlatformRect& other) const
		{
			return left == other.left && top == other.top && right == other.right && bottom == other.bottom;
		}
	};

	namespace EPopUpOrientation
	{
		enum Type
		{
			Horizontal,
			Vertical
		};
	}

	class Window;
	struct FGenericWindowDefinition;

	class ICursor;
	/*
	* generic platform application interface
	*/
	class GenericApplication
	{
	public:
		GenericApplication(const std::shared_ptr<ICursor>& in_cursor);

		virtual ~GenericApplication();

		virtual void pump_messages(const float time_delta) {}

		virtual void set_message_handler(const std::shared_ptr<FGenericApplicationMessageHandler>& in_message_handler) { m_message_handler = in_message_handler; }

		virtual std::shared_ptr<FGenericApplicationMessageHandler> get_message_handler() { return m_message_handler; }

		virtual void Tick(const float time_delta);

		virtual std::shared_ptr<Window> make_window();

		virtual void initialize_window(const std::shared_ptr<Window>& window, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<Window>& in_parent, const bool b_show_immediately) {}

		static std::shared_ptr<GenericApplication> Create();

		/** @return native window under the mouse cursor **/
		virtual std::shared_ptr<Window> get_window_under_cursor() { return std::shared_ptr<Window>(nullptr); }

		virtual FModifierKeyState get_modifier_keys() const { return FModifierKeyState(); }

		virtual FPlatformRect get_work_area(const FPlatformRect& current_window) const
		{
			FPlatformRect out_rect;

			out_rect.left = 0;
			out_rect.top = 0;
			out_rect.right = 0;
			out_rect.bottom = 0;

			return out_rect;
		}
	public:
		const std::shared_ptr<ICursor> m_cursor;//note:slate application will from there to get the ICursor
	protected:
		//todo:implement FGenericApplicationMessageHandler
		std::shared_ptr<FGenericApplicationMessageHandler> m_message_handler;

		
	};
}
