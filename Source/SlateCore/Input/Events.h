#pragma once

#include "ApplicationCore/GenericPlatform/GenericApplication.h"//FModifierKeyState depends on 
#include "Core/InputCore/InputCoreTypes.h"//FKey depends on it

namespace DoDo
{
	class FWidgetPath;//todo:implement this type
	/*
	 * base class for all mouse and key events
	 */
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
	private:
		//name of the key that was pressed
		FKey m_key;

		//the character code of the key that was pressed, only applicable to typed character keys, 0 otherwise
		uint32_t m_character_code;

		//original key code received from hardware before any conversion/mapping
		uint32_t m_key_code;//note:original
	};
}
