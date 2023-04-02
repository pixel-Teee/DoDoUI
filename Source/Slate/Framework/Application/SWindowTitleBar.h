#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Application/SlateApplicationBase.h"//IWindowTitleBar depends on it

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

#include "SlateCore/Styling/ISlateStyle.h"

#include "SlateCore/Styling/SlateTypes.h"//FWindowStyle depends on it

#include "SlateCore/Widgets/SWindow.h"

#include "Slate/Widgets/Layout/SBox.h"

#include "Slate/Widgets/Text/STextBlock.h"

#include "Slate/Widgets/Layout/SBorder.h"

#include "SlateCore/Widgets/SOverlay.h"

#include "SlateCore/Widgets/SNullWidget.h"

#include "SlateCore/Widgets/SBoxPanel.h"

#include "Slate/Widgets/Layout/SSpacer.h"

#include "Slate/Widgets/Input/SButton.h"

#include "SlateCore/Widgets/Images/SImage.h"

#include "Application/Application.h"

namespace DoDo {
	/*
	* widget that represents the app icon + system menu button, usually drawn in the top left of a windows app
	*/
	class SAppIconWidget : public SCompoundWidget
	{
		SLATE_BEGIN_ARGS(SAppIconWidget)
			: _IconColorAndOpacity(FLinearColor::White)
		{

		}
		/*icon color and opacity*/
		SLATE_ATTRIBUTE(FSlateColor, IconColorAndOpacity)
		SLATE_END_ARGS()

		void Construct(const FArguments& args)
		{
			m_icon_image = Application::get().make_image(
				Application::get().get_app_icon(),
				args._IconColorAndOpacity,
				EVisibility::HitTestInvisible
			);

			this->m_child_slot//todo:add margin
			[
				m_icon_image
			];
		}
	private:
		std::shared_ptr<SImage> m_icon_image;
	};
	/*
	* implements a window title bar widget
	*/
	class SWindowTitleBar : public SCompoundWidget, public IWindowTitleBar
	{
	public:
		SLATE_BEGIN_ARGS(SWindowTitleBar)
			: _Style(&FCoreStyle::get().get_widget_style<FWindowStyle>("Window"))
			, _show_app_icon(true)
		{}
			SLATE_STYLE_ARGUMENT(FWindowStyle, Style)
			SLATE_ARGUMENT(bool, show_app_icon)
			SLATE_ATTRIBUTE(DoDoUtf8String, Title)
		SLATE_END_ARGS()

	public:
		/*
		* creates and initializes a new window title bar widget
		* 
		* @param InArgs the construction arguments
		* @param InWindow the window to create the title bar for
		* @param InCenterContent the content for the title bar's content area
		* @param CenterContentAlignment the horizontal alignment of the center content
		*/
		void Construct(const FArguments& in_args, const std::shared_ptr<SWindow>& in_window, const std::shared_ptr<SWidget>& in_center_content,
			EHorizontalAlignment in_center_content_alignment)
		{
			m_owner_window_ptr = in_window;
			m_style = in_args._Style;
			m_show_app_icon = in_args._show_app_icon;
			m_title = in_args._Title;

			//todo:implement handle window title text
			if (!m_title.Is_Set() && !m_title.Is_Bound())
			{
				m_title = TAttribute<DoDoUtf8String>::Create(TAttribute<DoDoUtf8String>::FGetter::CreateSP(this, &SWindowTitleBar::handle_window_title_text));
			}

			m_child_slot
			[
				 SNew(SBorder)
				.Padding(0.0f)//todo:add visibility
				.Visibility(EVisibility::SelfHitTestInvisible)
				.BorderImage(this, &SWindowTitleBar::get_window_title_background_image)
				[
					SNew(SOverlay)
					.Visibility(EVisibility::SelfHitTestInvisible)
					+ SOverlay::Slot()
					[
						make_title_bar_content(in_center_content, in_center_content_alignment)
					]
				]
			];
		}

		virtual EWindowZone::Type get_window_zone_override() const override
		{
			return EWindowZone::TitleBar;
		}

		/*@return an appropriate resource for the window title background depending on whether the window is active*/
		const FSlateBrush* get_window_title_background_image() const
		{
			std::shared_ptr<SWindow> owner_window = m_owner_window_ptr.lock();

			if (!owner_window)
			{
				return nullptr;
			}

			std::shared_ptr<Window> native_window = owner_window->get_native_window();

			const bool b_is_active = native_window != nullptr;//todo:add is foreground window check

			return b_is_active ? &m_style->m_active_title_brush : &m_style->m_inactive_title_brush;
		}

		/*
		* creates widgets for this window's title bar area
		* 
		* this is an advanced method, only for fancy windows that want to
		* override the look of the title area by arranging those widgets itself
		*/
		virtual void make_title_bar_content_widgets(std::shared_ptr<SWidget>& out_left_content, std::shared_ptr<SWidget>& out_right_content)
		{
			std::shared_ptr<SWindow> owner_window = m_owner_window_ptr.lock();

			if (!owner_window)
			{
				return;
			}

			//todo:check owner window have close button

			const bool b_has_window_buttons = true;

			if (b_has_window_buttons)
			{
				m_close_button = SNew(SButton)
								.ButtonStyle(FCoreStyle::get(), "NoBorder")
								.OnClicked(this, &SWindowTitleBar::close_button_on_clicked)
								[
									SNew(SImage)
									.Image(this, &SWindowTitleBar::get_close_image)
									.ColorAndOpacity(this, &SWindowTitleBar::get_window_title_content_color)
								];
			}

			//windows ui layout
			

			if (m_show_app_icon && b_has_window_buttons)
			{
				out_left_content =
					SNew(SHorizontalBox)
					//.Visibility(EVisibility::SelfHitTestInvisible)
					+ SHorizontalBox::Slot()
					.auto_width()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Top)
					[
						SAssignNew(m_app_icon_widget, SAppIconWidget)
					];

				//todo:add window menu slot

				
			}
			else
			{

			}

			if (b_has_window_buttons)
			{
				out_right_content = SNew(SBox)
					.Visibility(EVisibility::SelfHitTestInvisible)
					.Padding(FMargin(2.0f, 0.0f, 0.0f, 0.0f))
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Top)
						.auto_width()
						[
							m_close_button
						]
					];
			}
			else
			{

			}
		}

		/*
		* creates the title bar's content
		* 
		* @param CenterContent the content for the title bar's center area
		* @param CenterContentAlignment the horizontal alignment of the center content
		* 
		* @return the content widget
		*/
		std::shared_ptr<SWidget> make_title_bar_content(std::shared_ptr<SWidget> center_content, EHorizontalAlignment center_content_alignment)
		{
			std::shared_ptr<SWidget> left_content;
			std::shared_ptr<SWidget> right_content;

			make_title_bar_content_widgets(left_content, right_content);

			//create window title if no content was provided
			if (!center_content)//todo:fix me
			{
				center_content = SNew(SBox)
								.HAlign(HAlign_Center)
								.Visibility(EVisibility::SelfHitTestInvisible)
								.Padding(FMargin(5.0f, 2.0f, 5.0f, 2.0f))
								[
									//note:we bind the window's title text to our windows' get title method, so that if the
									//title is changed later, the text will always be visually up to date
									SNew(STextBlock)
									.Visibility(EVisibility::SelfHitTestInvisible)
									.TextStyle(&m_style->m_title_text_style)
									.Text(m_title)
								];
			}

			//adjust the center content alignment if needed, windows without any title bar buttons look better if the title is centered
			if (left_content == SNullWidget::NullWidget && right_content == SNullWidget::NullWidget && center_content_alignment == EHorizontalAlignment::HAlign_Left)
			{
				center_content_alignment = EHorizontalAlignment::HAlign_Center;
			}

			//calculate content dimensions
			left_content->slate_prepass();
			right_content->slate_prepass();

			glm::vec2 left_size = left_content->get_desired_size();
			glm::vec2 right_size = right_content->get_desired_size();

			if (center_content_alignment == HAlign_Center)
			{
				//left_size = std::max(left_size, right_size);
				left_size = (left_size.x >= right_size.x && left_size.y >= right_size.y) ? left_size : right_size;
				right_size = left_size;
			}

			float spacer_height = std::max(left_size.y, right_size.y);//note:left content and right content's max height

			//create title bar
			return SAssignNew(m_title_area, SBox)
				  .Visibility(EVisibility::SelfHitTestInvisible)
				  [
				  	SNew(SOverlay)
				  	.Visibility(EVisibility::SelfHitTestInvisible)
				  	+ SOverlay::Slot()
				  	[
				  		SNew(SHorizontalBox)
				  		.Visibility(EVisibility::SelfHitTestInvisible)
				  		+ SHorizontalBox::Slot()
				  		.auto_width()
				  		.HAlign(HAlign_Left)
				  		.VAlign(VAlign_Top)
				  		[
				  			SNew(SSpacer)
				  			.Size(glm::vec2(left_size.x, spacer_height))
				  		]
				  		+ SHorizontalBox::Slot()
				  		.HAlign(center_content_alignment)
				  		.VAlign(VAlign_Center)
				  		.fill_width(1.0f)
				  		[
				  			center_content
				  		]
				  		+ SHorizontalBox::Slot()
				  		.auto_width()
				  		.HAlign(HAlign_Right)
				  		.VAlign(VAlign_Top)
				  		[
				  			SNew(SSpacer)
				  			.Size(glm::vec2(right_size.x, spacer_height))
				  		]
				  	]
				  	+ SOverlay::Slot()
				  	[
				  		SNew(SHorizontalBox)
				  		.Visibility(EVisibility::SelfHitTestInvisible)
						+ SHorizontalBox::Slot()
						.auto_width()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Top)
						[
							left_content
						]
						+ SHorizontalBox::Slot()
						.fill_width(1.0f) //note:this may block hittest grid
				  		+ SHorizontalBox::Slot()
				  		.auto_width()
				  		.HAlign(HAlign_Right)
				  		.VAlign(VAlign_Top)
				  		[
				  			right_content
				  		]
				  	]
				  ];
		}

		DoDoUtf8String handle_window_title_text() const
		{
			std::shared_ptr<SWindow> owner_window = m_owner_window_ptr.lock();

			if (!owner_window)
			{
				return "";
			}

			return owner_window->get_title();
		}

	protected:
		//hold a weak pointer to the owner window
		std::weak_ptr<SWindow> m_owner_window_ptr;

		FSlateColor get_window_title_content_color() const
		{
			//color of the title area contents - modulates the icon and buttons

			return FSlateColor(FLinearColor::White);
		}

	private:
		//callback for clicking the close button
		FReply close_button_on_clicked()
		{
			std::shared_ptr<SWindow> owner_window = m_owner_window_ptr.lock();

			if (owner_window)
			{
				owner_window->request_destroy_window();
			}

			return FReply::handled();
		}
		//callback for getting the image of the close button
		const FSlateBrush* get_close_image() const
		{
			std::shared_ptr<SWindow> owner_window = m_owner_window_ptr.lock();

			if (!owner_window)
			{
				return nullptr;
			}

			std::shared_ptr<Window> native_window = owner_window->get_native_window();

			if (m_close_button->is_pressed())
			{
				return &m_style->m_close_button_style.m_pressed;
			}

			if (m_close_button->is_hovered())
			{
				return &m_style->m_close_button_style.m_hovered;
			}

			return &m_style->m_close_button_style.m_normal;
		}

		//holds the window style to use (for buttons, text, etc.)
		const FWindowStyle* m_style;

		//holds the content widget of the title area
		std::shared_ptr<SWidget> m_title_area;

		TAttribute<DoDoUtf8String> m_title;

		//holds the close button
		std::shared_ptr<SButton> m_close_button;

		std::shared_ptr<SAppIconWidget> m_app_icon_widget;

		bool m_show_app_icon;
	};
}