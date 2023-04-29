#pragma once

#include "IMenu.h"

namespace DoDo {
	/*
	* represents the base class of popup menus
	*/
	class FMenuBase : public IMenu, public std::enable_shared_from_this<FMenuBase>
	{
	public:
		virtual std::shared_ptr<SWidget> get_content() const override { return m_content; }

	protected:
		FMenuBase(std::shared_ptr<SWidget> in_content, const bool b_collapsed_by_parent);

		std::shared_ptr<SWidget> m_content;

		bool m_b_is_collapsed_by_parent;
	};

	/*
	* represents a popup menu that is shown in its own SWindow
	*/
	class FMenuInWindow : public FMenuBase
	{
	public:
		FMenuInWindow(std::shared_ptr<SWindow> in_window, std::shared_ptr<SWidget> in_content, const bool b_is_collapsed_by_parent);
		virtual ~FMenuInWindow();

		virtual std::shared_ptr<SWindow> get_parent_window() const override;
		virtual std::shared_ptr<SWindow> get_owned_window() const override { return get_parent_window(); }

	private:
		std::weak_ptr<SWindow> m_window;
	};
}