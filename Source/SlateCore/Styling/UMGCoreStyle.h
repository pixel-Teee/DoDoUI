#pragma once

namespace DoDo {
	class ISlateStyle;
	/*
	* core slate style
	*/
	class FUMGCoreStyle
	{
	public:

		static std::shared_ptr<ISlateStyle> create();

		static void reset_to_default();

	private:
		static void set_style(const std::shared_ptr<ISlateStyle>& new_style);

		/*singleton instances of this style*/
		static std::shared_ptr<ISlateStyle> m_instance;
	};
}