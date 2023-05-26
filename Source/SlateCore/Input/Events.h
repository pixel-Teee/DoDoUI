#pragma once

#include "ApplicationCore/GenericPlatform/GenericApplication.h"//FModifierKeyState depends on 
#include "Core/InputCore/InputCoreTypes.h"//FKey depends on it

#include "glm/vec2.hpp"

#include <set>

namespace DoDo
{
	/*context for focus change*/
	enum class EFocusCause : uint8_t
	{
		Mouse,

		Navigation,

		SetDirectly,

		Cleared,

		OtherWidgetLostFocus,

		WindowActivate
	};
	/*represents the current and last cursor position in a "virtual window" for events that are routed to widgets transformed in a 3D scene*/
	struct FVirtualPointerPosition
	{
		FVirtualPointerPosition()
			: m_current_cursor_position(glm::vec2(0.0f, 0.0f))
			, m_last_cursor_position(glm::vec2(0.0f, 0.0f))
		{}

		FVirtualPointerPosition(const glm::vec2& in_current_cursor_position, const glm::vec2& in_last_cursor_position)
			: m_current_cursor_position(in_current_cursor_position)
			, m_last_cursor_position(in_last_cursor_position)
		{}

		glm::vec2 m_current_cursor_position;
		glm::vec2 m_last_cursor_position;
	};
	class FWidgetPath;//todo:implement this type
	/*
	 * base class for all mouse and key events
	 */
	class SWidget;
	struct FGeometry;
	class SWindow;
	struct FInputEvent
	{
	public:

		FInputEvent()
			: m_modifier_keys(FModifierKeyState())
			, m_b_is_repeat(false)
			, m_user_index(0)
			, m_event_path(nullptr)
		{}

		/*
		 * constructor, events are immutable once constructed
		 *
		 * @param InModifierKeys modifier key state for this event
		 * @param bInIsRepeat true if this key is an auto-repeated keystoke
		 */
		FInputEvent(const FModifierKeyState& in_modifier_keys, const int32_t in_user_index, const bool b_in_is_repeate)
			: m_modifier_keys(in_modifier_keys)
			, m_b_is_repeat(b_in_is_repeate)
			, m_user_index(in_user_index)
			, m_event_path(nullptr)
		{}

		virtual ~FInputEvent() {}

		/*set the widget path along which this event will be routed*/
		void set_event_path(const FWidgetPath& in_event_path)
		{
			m_event_path = &in_event_path;
		}

		/*
		 * returns the index of the user that generated this event
		 */
		uint32_t get_user_index() const
		{
			return m_user_index;
		}

		/*the event path provides additional context for handling*/
		FGeometry find_geometry(const std::shared_ptr<SWidget>& widget_to_find) const;

		std::shared_ptr<SWindow> get_window() const;

	public:
		/*
		* returns whether or not this character is an auto-repeated keystroke
		* 
		* @return true if this character is a repeat
		*/
		bool is_repeat() const
		{
			return m_b_is_repeat;
		}

	protected:
		//state of modifier keys when this event happened
		FModifierKeyState m_modifier_keys;//shift tab caps lock

		//true if this key was auto-repeated
		bool m_b_is_repeat;

		//the index of the user that caused the event
		uint32_t m_user_index;

		//events are sent along paths, see (GetEventPath)
		const FWidgetPath* m_event_path;
	};

	/*
	 * FKeyEvent describes a key action (keyboard/controller key/button pressed or released)
	 * it is passed to event handlers dealing with key input
	 */
	struct FKeyEvent : public FInputEvent
	{
	public:
		FKeyEvent()
			: FInputEvent(FModifierKeyState(), 0, false)
			, m_key()
			, m_character_code(0)
			, m_key_code(0)
		{}
		/*
		 * constructor, events are immutable once constructed
		 *
		 * @param InKeyName character name
		 * @param InModifierKeys modifier key state for this event
		 * @param bInIsRepeat true if this key is an auto-repeated keystroke
		 */
		FKeyEvent(const FKey in_key,
					const FModifierKeyState& in_modifier_keys,
					const uint32_t in_user_index,
					const bool b_in_is_repeat,
					const uint32_t in_character_code,
					const uint32_t in_key_code)
		: FInputEvent(in_modifier_keys, in_user_index, b_in_is_repeat)
		, m_key(in_key)
		, m_character_code(in_character_code)
		, m_key_code(in_key_code)
		{}

		/*
		* returns the name of the key for this event
		* 
		* @return key name
		*/
		FKey get_key() const
		{
			return m_key;
		}
	private:
		//name of the key that was pressed
		FKey m_key;

		//the character code of the key that was pressed, only applicable to typed character keys, 0 otherwise
		uint32_t m_character_code;

		//original key code received from hardware before any conversion/mapping
		uint32_t m_key_code;//note:original
	};

	/*
	* FFocusEvent is used when notifying widgets about keyboard focus changes
	* it is passed to event handlers dealing with keyboard focus
	*/
	struct FFocusEvent
	{
	public:
		FFocusEvent()
			: m_cause(EFocusCause::SetDirectly)
			, m_user_index(0)
		{}

		FFocusEvent(const EFocusCause in_cause, uint32_t in_user_index)
			: m_cause(in_cause)
			, m_user_index(in_user_index)
		{}

		EFocusCause get_cause() const
		{
			return m_cause;	
		}

		uint32_t get_user() const
		{
			return m_user_index;
		}
	private:
		/*the cause of the focus change*/
		EFocusCause m_cause;

		/*user that is changing focus*/
		uint32_t m_user_index;
	};

	/*
	* FCharacterEvent describes a keyboard action where the utf8 code is given, used for on key char messages
	*/
	struct FCharacterEvent : public FInputEvent
	{
	public:
		FCharacterEvent()
			: FInputEvent(FModifierKeyState(), 0, false)
			, m_character("")//0
		{}

		FCharacterEvent(const DoDoUtf8String in_character, const FModifierKeyState& in_modifier_keys, const uint32_t in_user_index, const bool b_in_is_repeat)
			: FInputEvent(in_modifier_keys, in_user_index, b_in_is_repeat)
			, m_character(in_character)
		{}

		/*
		* returns the character for this event
		* 
		* @return Character
		*/
		DoDoUtf8String get_character() const
		{
			return m_character;
		}

	private:
		//the character that was pressed
		DoDoUtf8String m_character;
	};

	/*
	 * FPointerEvent describes a mouse or touch action (e.g. Press, Release, Move, etc)
	 * it is passed to event handlers dealing with pointer-based input
	 */
	struct FPointerEvent
		: public FInputEvent
	{
	public:
		FPointerEvent() {}
		/*
		 * Events are immutable once constructed
		 */
		FPointerEvent(
			uint32_t in_pointer_index,
			const glm::vec2& in_screen_space_position,
			const glm::vec2& in_last_screen_space_position,
			const FModifierKeyState& in_modifier_keys
		)
			: FInputEvent(in_modifier_keys, 0, false)
			, m_screen_space_position(in_screen_space_position)
			, m_last_screen_space_position(in_last_screen_space_position)
			, m_cursor_delta(in_screen_space_position - in_last_screen_space_position)
			, m_pointer_index(in_pointer_index)
		{}

		FPointerEvent(
			uint32_t in_pointer_index,
			const glm::vec2& in_screen_space_position,
			const glm::vec2& in_last_screen_space_position,
			const std::set<FKey>& in_pressed_buttons,
			FKey in_effecting_button,
			float in_wheel_delta,
			const FModifierKeyState& in_modifier_keys
		)
			: FInputEvent(in_modifier_keys, 0, false)
			, m_screen_space_position(in_screen_space_position)
			, m_last_screen_space_position(in_last_screen_space_position)
			, m_cursor_delta(in_screen_space_position - in_last_screen_space_position)
			, m_pressed_buttons(&in_pressed_buttons)
			, m_effecting_button(in_effecting_button)
			, m_pointer_index(in_pointer_index)
		{}

		FPointerEvent(
			uint32_t in_user_index,
			uint32_t in_pointer_index,
			const glm::vec2& in_screen_space_position,
			const glm::vec2& in_last_screen_space_position,
			const std::set<FKey>& in_pressed_buttons,
			FKey in_effecting_button,
			float in_wheel_delta,
			const FModifierKeyState& in_modifier_keys
		)
			: FInputEvent(in_modifier_keys, in_user_index, false)
			, m_screen_space_position(in_screen_space_position)
			, m_last_screen_space_position(in_last_screen_space_position)
			, m_cursor_delta(in_screen_space_position - in_last_screen_space_position)
			, m_pressed_buttons(&in_pressed_buttons)
			, m_effecting_button(in_effecting_button)
			, m_pointer_index(in_pointer_index)
		{}

		FPointerEvent(
			uint32_t in_user_index,
			uint32_t in_pointer_index,
			const glm::vec2& in_screen_space_position,
			const glm::vec2& in_last_screen_space_position,
			const glm::vec2& in_delta,
			const std::set<FKey>& in_pressed_buttons,
			const FModifierKeyState& in_modifier_keys
		)
			: FInputEvent(in_modifier_keys, in_user_index, false)
			, m_screen_space_position(in_screen_space_position)
			, m_last_screen_space_position(in_last_screen_space_position)
			, m_cursor_delta(in_delta)
			, m_pressed_buttons(&in_pressed_buttons) //note:this is owned by the platform application
			, m_pointer_index(in_pointer_index)
		{} //todo:add more state

		/*a constructor to alter cursor positions*/
		FPointerEvent(
			const FPointerEvent& other,
			const glm::vec2& in_screen_space_position,
			const glm::vec2& in_last_screen_space_position
		)
		{
			*this = other;
			m_screen_space_position = in_screen_space_position;
			m_last_screen_space_position = in_last_screen_space_position;
		}

		/*returns the position of the cursor in screen space*/
		const glm::vec2& get_screen_space_position() const { return m_screen_space_position; }

		/*returns the position of the cursor in screen space last time we handled an input event*/
		const glm::vec2& get_last_screen_space_position() const { return m_last_screen_space_position; }

		/*returns the distance the mouse traveled since the last event was handled*/
		const glm::vec2& get_cursor_delta() const { return m_cursor_delta; }

		/*mouse buttons that are currently pressed*/
		bool is_mouse_button_down(FKey mouse_button) const { return m_pressed_buttons->find(mouse_button) != m_pressed_buttons->end(); }

		template<typename PointerEventType>
		static PointerEventType make_translated_event(const PointerEventType& in_pointer_event, const FVirtualPointerPosition& virtual_position)
		{
			PointerEventType new_event = in_pointer_event;
			new_event.m_screen_space_position = virtual_position.m_current_cursor_position;
			new_event.m_last_screen_space_position = virtual_position.m_last_cursor_position;

			return new_event;
		}

		/*mouse button that caused this event to be raised (possibly FKey::Invalid)*/
		FKey get_effecting_button() const { return m_effecting_button; }

		/*returns the index of the user that caused the event*/
		int32_t get_user_index() const { return m_user_index; }

		/*returns the unique identifier of the pointer(e.g., finger index)*/
		uint32_t get_pointer_index() const { return m_pointer_index; }
	private:
		glm::vec2 m_screen_space_position;//screen space position
		glm::vec2 m_last_screen_space_position;
		glm::vec2 m_cursor_delta;


		const std::set<FKey>* m_pressed_buttons;
		FKey m_effecting_button;

		//todo:implement TSet<FKey>
		//todo:implement FKey
		uint32_t m_pointer_index;

		//todo:implement other information and members
	};

	/*
	* FWindowActivateEvent describes a window being activated or deactivated
	* (i.e. brought to the foreground or moved to the background)
	* this event is only passed to top level windows, most widgets are incapable
	* of receiving this event
	*/
	class SWindow;
	class FWindowActivateEvent
	{
	public:
		enum EActivationType
		{
			EA_Activate,
			EA_ActivateByMouse,
			EA_Deactivate
		};

		FWindowActivateEvent(EActivationType in_activation_type, std::shared_ptr<SWindow> in_affected_window)
			: m_affected_window(in_affected_window)
			, m_activation_type(in_activation_type)
		{}

	public:
		/*describes what actually happened to the window (e.h activated, deactivaed, activated by a mouse click)*/
		EActivationType get_activation_type() const
		{
			return m_activation_type;
		}

		/*the window that this activation/deactivation happened to*/
		std::shared_ptr<SWindow> get_affected_window() const
		{
			return m_affected_window;
		}

	private:
		EActivationType m_activation_type;
		std::shared_ptr<SWindow> m_affected_window;
	};
}
