#include <PreCompileHeader.h>

#include <chrono>

#include "Application.h"

#include <Renderer/Renderer.h>

#include <string>//to_string depends on it

//#include <vulkan/vulkan.h>

#include "Renderer/RendererInstance.h"
#include "Core/Window.h"
#include "SlateCore/Widgets/SWindow.h"

#include "Slate/Widgets/Layout/SBorder.h"//SBorder depends on it

#include "Slate/Widgets/Layout/SConstraintCanvas.h"

#include "SlateCore/Widgets/SBoxPanel.h"//SHorizontalBoxPanel

#include "Slate/Widgets/Input/SButton.h"

#include "SlateCore/Rendering/SlateDrawBuffer.h"//FDrawWindowArgs depends on it

#ifdef WIN32
#include "Platform/Application/GLFWApplication.h"//GenericApplication depends on it
#include "Platform/Application/WindowsPlatformApplicationMisc.h"
#else
#include "Platform/Application/AndroidApplication.h"
#endif

#include "Slate/Widgets/Layout/SSplitter.h"//SSplitter

#include "SlateCore/Rendering/DrawElements.h"
#include "SlateCore/Styling/WidgetStyle.h"

#include "SlateCore/Styling/CoreStyle.h"//initialize core style depends on it

#include "SlateCore/Styling/StarshipCoreStyle.h"

//#include "SlateCore/Layout/WidgetPath.h"//FWidgetPath

#include "Slate/Widgets/Colors/SComplexGradient.h"//SComplexGradient depends on it

#include "SlateCore/Application/SlateWindowHelper.h"//SlateWindowHelper

#include "Slate/Widgets/Text/STextBlock.h"//STextBlock.h depends on it

#include "Slate/Widgets/Input/SSlider.h"//SSlider depends on it

#include "SlateCore/Widgets/Images/SImage.h"//SImage depends on it

#include "Slate/Widgets/Colors/SColorWheel.h"//SColorWheel depends on it

#include "Slate/Widgets/Docking/SDockTab.h"//SDockTab depends on it

#include "Slate/Framework/Docking/SDockingTabStack.h"//SDockTabStack depends on it

#include "SlateCore/Layout/LayoutUtils.h"//calculate_popup_window_position depends on it

#include "SlateCore/Styling/UMGCoreStyle.h"//FUMGCoreStyle depends on it

namespace DoDo
{
	std::shared_ptr<GenericApplication> Application::s_platform_application = nullptr;//global platform application

	std::shared_ptr<Application> Application::s_current_application = nullptr;//global slate application

    const uint32_t Application::m_cursor_pointer_index = 10;

    const uint32_t Application::m_cursor_user_index = 0;

    class FEventRouter
    {
    public:

		class FBubblePolicy
		{
		public:
            FBubblePolicy(const FWidgetPath& in_routing_path)
	            : m_widget_index(in_routing_path.m_widgets.num() - 1)
				, m_routing_path(in_routing_path)
            {}

            static DoDoUtf8String m_name;//

            void next()
            {
                --m_widget_index;
            }

            bool should_keep_going() const
            {
                return m_widget_index >= 0;
            }

            const FWidgetPath& get_routing_path() const
            {
                return m_routing_path;
            }

            const FWidgetPath* get_widgets_under_cursor()
            {
                return &m_routing_path;
            }

            FWidgetAndPointer get_widget() const
            {
                return FWidgetAndPointer(m_routing_path.m_widgets[m_widget_index], m_routing_path.get_virtual_pointer_position(m_widget_index));//from widget path to construct a widget that have virtual pointer position
            }
		private:
            int32_t m_widget_index;
            const FWidgetPath& m_routing_path;
		};

        class FToLeafmostPolicy
        {
        public:
            FToLeafmostPolicy(const FWidgetPath& in_routing_path)
	        : m_b_event_sent(false)
			, m_routing_path(in_routing_path)
            {}

            static DoDoUtf8String m_name;

            void next()
            {
                m_b_event_sent = true;
            }

            bool should_keep_going() const
            {
                return !m_b_event_sent && m_routing_path.m_widgets.num() > 0;
            }

            const FWidgetPath& get_routing_path() const
            {
                return m_routing_path;
            }

            const FWidgetPath* get_widgets_under_cursor()
            {
                return &m_routing_path;
            }

            FWidgetAndPointer get_widget() const
            {
                const int32_t widget_index = m_routing_path.m_widgets.num() - 1;
                return FWidgetAndPointer(m_routing_path.m_widgets[widget_index], m_routing_path.get_virtual_pointer_position(widget_index));
            }

        private:
            bool m_b_event_sent;
            const FWidgetPath& m_routing_path;
        };

        /*
         * route an event based on the routing policy
         */
        template<typename ReplyType, typename RoutingPolicyType, typename EventType, typename FuncType>
        static ReplyType Route(Application* this_application, RoutingPolicyType routing_policy, EventType event_copy, const FuncType& lambda)
        {
            ReplyType reply = ReplyType::un_handled();//todo:interms of widget reply to block

            const FWidgetPath& routing_path = routing_policy.get_routing_path();
            const FWidgetPath* widgets_under_cursor = routing_policy.get_widgets_under_cursor();

            event_copy.set_event_path(routing_path);//event_copy is FPointerEvent etc, to set widget path in this

            for(; !reply.is_event_handled() && routing_policy.should_keep_going(); routing_policy.next()) //from last to first, in terms of policy to traversal widgets
            {
                const FWidgetAndPointer& arranged_widget = routing_policy.get_widget();

                if constexpr (Translate<EventType>::translation_needed())
                {
                    const EventType translated_event = Translate<EventType>::pointer_event(arranged_widget, event_copy);
                    reply = lambda(arranged_widget, translated_event).set_handler(arranged_widget.m_widget);
                    process_reply(this_application, routing_path, reply, widgets_under_cursor, &translated_event);
                }
                else
                {
                    reply = lambda(arranged_widget, event_copy).set_handler(arranged_widget.m_widget);
                    process_reply(this_application, routing_path, reply, widgets_under_cursor, &event_copy);
                }
            }

            return reply;
        }

        //noting to do
        static void process_reply(Application* application, const FWidgetPath& routing_path, const FNoReply& reply, const FWidgetPath* widgets_under_cursor, const FInputEvent*)
        {
	        
        }

        static void process_reply(Application* application, const FWidgetPath& routing_path, const FReply& reply, const FWidgetPath* widgets_under_cursor, const FInputEvent* pointer_event)
        {
            //todo:handle process_reply
        }

        static void process_reply(Application* application, const FWidgetPath& routing_path, const FReply& reply, const FWidgetPath* widgets_under_cursor, const FPointerEvent* pointer_event)
        {
            application->process_reply(routing_path, reply, widgets_under_cursor, pointer_event, pointer_event->get_user_index());
        }

        template<typename EventType>
        struct Translate
        {
            static constexpr bool translation_needed() { return false; }

            static EventType pointer_event(const FWidgetAndPointer& in_position, const EventType& in_event)
            {
                //most events do not do any coordinate translation
                return in_event;
            }
        };
    };

    template<>
    struct FEventRouter::Translate<FPointerEvent>
    {
        static constexpr bool translation_needed() { return true; }

        static FPointerEvent pointer_event(const FWidgetAndPointer& in_position, const FPointerEvent& in_event)
        {
            //most events do not do any coordinate translation
			if(!in_position.get_pointer_position().has_value())
			{
                return in_event;
			}
            else
            {
                return FPointerEvent::make_translated_event<FPointerEvent>(in_event, in_position.get_pointer_position().value());
            }
        }
    };

    DoDoUtf8String FEventRouter::FBubblePolicy::m_name = "Bubble";

    struct FDrawWindowArgs
    {
        //FDrawWindowArgs() {}
        //todo:implement FSlateDrawBuffer
        //todo:implement FWidgetPath
        FDrawWindowArgs(FSlateDrawBuffer& in_draw_buffer)
            : m_out_draw_buffer(in_draw_buffer)
        {}

        FSlateDrawBuffer& m_out_draw_buffer;
    };

    Application::Application()
	    : m_last_tick_time(0.0f)
        , m_current_time(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count())
		, m_average_delta_time(1.0f / 30.0f)
    {
        m_last_time = m_current_time;
        //m_p_vk_instance = CreateScope<VkInstance>();
        //m_p_window = Window::Create();

        //std::shared_ptr<SBorder> border;
        //std::shared_ptr<SBorder> border2;
        //SAssignNew(border, SBorder)
        //    .ContentScale(1.0f)
        //    .VAlign(VAlign_Fill)
        //    .HAlign(HAlign_Fill)
        //    [
        //        SAssignNew(border2, SBorder)
        //        .ContentScale(1.0f)
		//		.VAlign(VAlign_Fill)
		//		.HAlign(HAlign_Fill)
        //    ];
		//std::shared_ptr<SWindow> root_window;
        //
		//SAssignNew(root_window, SWindow)
		//	.Title("hello")
		//	.ClientSize(glm::vec2(1280.0f, 720.0f))
		//	.ScreenPosition(glm::vec2(200.0f, 200.0f));
        //
		//add_window(root_window);
        //
        //std::shared_ptr<SWindow> root_window2;
        //
        //SAssignNew(root_window2, SWindow)
        //    .Title("hello2");
        //
        //add_window(root_window2);

        //add the standard 'default' user
        register_new_user(m_cursor_user_index);//this is to identity user
    }
        

    Application::~Application()
    {
        //m_renderer_instance = nullptr;
       // std::cout << "error" << std::endl;
        //m_renderer_instance->Destroy();
    }

    FReply test_bind(const FGeometry& geometry, const FPointerEvent& event)
    {
        std::cout << "widget size(" << geometry.m_size.x << ", " << geometry.m_size.y << ")\n";

        return FReply::un_handled();
    }

    std::shared_ptr<SWindow> Application::test_create_widget()
    {
        s_current_application->test_create_sdock();


        std::shared_ptr<SWindow> root_window;
        
        //std::shared_ptr<SBorder> border;
        
        //std::shared_ptr<SBorder> border2;
        //
        //std::shared_ptr<SButton> button2;
        /*
        std::vector<glm::vec4> colors = { {0.51f, 0.99f, 0.73f, 1.0f}, {1.0f, 0.98f, 0.49f, 1.0f}};

        std::vector<glm::vec4> colors2 = { {0.55f, 0.77f, 0.98f, 1.0f}, {0.87f, 0.76f, 0.98f, 1.0f} };

        std::vector<glm::vec4> colors3 = { {0.98f, 0.67f, 0.49f, 1.0f}, {0.96f, 0.87f, 0.40f, 1.0f} };

        std::shared_ptr<STextBlock> temp_text_block = nullptr;
        
        SAssignNew(root_window, SWindow)
            .Title("hello")
            .ClientSize(glm::vec2(1280.0f, 720.0f))
            .ScreenPosition(glm::vec2(200.0f, 200.0f))
            [
                //SNew(SDockTab)
				SAssignNew(s_current_application->m_border, SBorder)
				.BorderBackgroundColor(glm::vec4(1.0f, 0.8f, 0.4f, 1.0f))
				//.BorderImage(FCoreStyle::get().get_brush("Checkboard"))
				.Padding(100.0f)
				[
				    SNew(SConstraintCanvas)
					//+ SConstraintCanvas::Slot()
					//.Anchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f))//middle
					//.Offset(FMargin(0.0f, 0.0f, 147.0f, 253.0f))//position and size
					//.Alignment(glm::vec2(0.5f, 0.5f))
					//.AutoSize(false)
					//[
					//    SNew(SBorder)
					//    .BorderBackgroundColor(glm::vec4(0.3f, 0.7f, 0.8f, 1.0f))
					//	.OnMouseMove_Static(test_bind)
					//    //.BorderImage(FCoreStyle::get().get_brush("Checkboard"))
					//]
					//+ SConstraintCanvas::Slot()
					//.Anchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f))//middle
					//.Offset(FMargin(0.0f, 0.0f, 280.0f, 480.0f))//position and size
					//.Alignment(glm::vec2(0.5f, 0.5f))//control where to place the anchor
					//.AutoSize(false)
					//[
					//	SNew(SBorder)
					//	.BorderBackgroundColor(glm::vec4(0.9f, 0.3f, 0.2f, 1.0f))
					//    .BorderImage(FCoreStyle::get().get_brush("Checkboard"))
					//    .OnMouseMove_Static(test_bind)
					//]
					//+ SConstraintCanvas::Slot()
					//.Anchors(FAnchors(0.3f, 0.3f, 0.3f, 0.3f))//middle
					//.Offset(FMargin(0.0f, 0.0f, 480.0f, 280.0f))//position and size
					//.Alignment(glm::vec2(0.5f, 0.5f))//control where to place the anchor
					//.AutoSize(false)
					//[
					//	SNew(SComplexGradient)
					//	.GradientColors(colors2)
					//]
					+ SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-50.0f, -100.0f, 240.0f, 100.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
					[
					     SNew(SButton)
					    .ButtonColorAndOpacity(glm::vec4(0.7f, 0.3f, 0.9f, 1.0f))
					    .Text(s_current_application, &Application::calculate_frame_per_second)
				        [
				            SNew(SImage)
				            .Image(FAppStyle::get().get_brush("Icons.ArrowLeft"))
				        ]
					    //.OnPressed(border2, &SBorder::set_color)
					]
					//+ SConstraintCanvas::Slot()
					//.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					//.Offset(FMargin(-200.0f, -200.0f, 700.0f, 700.0f))
					//.Alignment(glm::vec2(1.0f, 1.0f))
					//[
					//    SNew(SComplexGradient)
					//    .GradientColors(colors)
					//	//.OnPressed(border2, &SBorder::set_color)
					//]
					//+ SConstraintCanvas::Slot()
					//.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					//.Offset(FMargin(-300.0f, -100.0f, 100.0f, 100.0f))
					//.Alignment(glm::vec2(1.0f, 1.0f))
					//[
					//	SNew(SBorder)
					//	.BorderBackgroundColor(glm::vec4(0.4f, 0.7f, 0.8f, 1.0f))
					//]
					+ SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-400.0f, -200.0f, 16.0f, 16.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
					[
						SNew(SImage)
						.Image(FAppStyle::get().get_brush("Icons.ArrowLeft"))
					]
				    + SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-600.0f, -400.0f, 100.0f, 100.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
				    [
						SNew(SImage)
						.Image(FAppStyle::get().get_brush("Icons.C++"))
				    ]
					+ SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-400.0f, -400.0f, 100.0f, 100.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
					[
						SNew(SImage)
						.Image(FAppStyle::get().get_brush("Icons.heart2"))
					]
					+ SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-300.0f, -400.0f, 100.0f, 100.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
					[
						SNew(SImage)
						.Image(FAppStyle::get().get_brush("Icons.solar-system"))
					]
					+ SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-400.0f, -300.0f, 100.0f, 100.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
					[
						SNew(SSlider)
						.MinValue(0.0f)
				        .MaxValue(1.0f)
				        .OnValueChanged(s_current_application, &Application::test_slider_value_changed)
					]
					+ SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-400.0f, -500.0f, 100.0f, 100.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
					[
						SAssignNew(temp_text_block, STextBlock)
					]
					+ SConstraintCanvas::Slot()
					.Anchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f))
					.Offset(FMargin(-400.0f, -100.0f, 100.0f, 100.0f))
					.Alignment(glm::vec2(1.0f, 1.0f))
					[
						SNew(SColorWheel)
				        .OnValueChanged(s_current_application, &Application::test_color_wheel_value_changed)
					]
				    + SConstraintCanvas::Slot()
					.Anchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f))
					.Offset(FMargin(100.0f, 100.0f, 400.0f, 200.0f))
					.Alignment(glm::vec2(0.0f, 0.0f))
					[
				        SNew(SSplitter)
				        .Orientation(EOrientation::Orient_Horizontal)
				        + SSplitter::Slot()
				        [
				            SNew(SSplitter)
				            .Orientation(EOrientation::Orient_Vertical)
				            + SSplitter::Slot()
				            .MinSize(20.0f)
				            .Resizable(true)
				            [
				                SNew(SBorder)
				                .BorderBackgroundColor(glm::vec4(0.2f, 0.8f, 0.4f, 1.0f))
				                [
				                    SNew(SImage)
				                    .Image(FAppStyle::get().get_brush("Icons.heart2"))
				                ]
				            ]
							+ SSplitter::Slot()
				            .Resizable(true)
				            [
				                SNew(SBorder)
				                .BorderBackgroundColor(glm::vec4(0.4f, 0.8f, 0.9f, 1.0f))
				            ]
							+ SSplitter::Slot()
				            .Resizable(true)
				            [
				                SNew(SBorder)
				                .BorderBackgroundColor(glm::vec4(0.8f, 0.2f, 0.3f, 1.0f))
				            ]
				        ]
						+ SSplitter::Slot()
				        [
				            SNew(SBorder)
				            .BorderBackgroundColor(glm::vec4(0.8f, 0.7f, 0.3f, 1.0f))
				            [
				                SNew(SSlider)
				                .MinValue(0.0f)
				                .MaxValue(1.0f)
				            ]
				        ]
					]
				]
            ];

       s_current_application->m_text_block = temp_text_block;

       // button2->set_on_pressed(FSimpleDelegate::CreateSP(border2, &SBorder::set_color));
        */
#ifdef Andorid
        get().add_window(root_window, false);
#else
        //get().add_window(root_window, true);
#endif
        /*
		std::shared_ptr<SWindow> root_window2;
		
		SAssignNew(root_window2, SWindow)
			.Title("hello2")
			.ClientSize(glm::vec2(1280.0f, 720.0f))
			.ScreenPosition(glm::vec2(1000.0f, 200.0f))
		    [
		        SNew(SBorder)
		        .BorderBackgroundColor(glm::vec4(0.7f, 0.3f, 0.2f, 1.0f))
				[
		            SNew(SHorizontalBox)
		            + SHorizontalBox::Slot()
		            .Padding(30.0f, 30.0f)
		            .fill_width(0.2f)
					.max_width(600.0f)
		            [
		                SNew(SBorder)
		                .BorderImage(FCoreStyle::get().get_brush("Checkboard"))
		            ]
					+ SHorizontalBox::Slot()
		            .Padding(40.0f, 40.0f)
		            .fill_width(0.8f)
		            .max_width(600.0f)
		            [
		                SNew(SBorder)
		                .BorderBackgroundColor(glm::vec4(0.95f, 0.3f, 0.6f, 1.0f))
		            ]
		            + SHorizontalBox::Slot()
		            .Padding(40.0f, 40.0f)
		            .fill_width(0.3f)
		            .max_width(600.0f)
		            [
		                SNew(SBorder)
		                .BorderBackgroundColor(glm::vec4(0.2f, 0.5f, 0.4f, 1.0f))
		            ]
		            + SHorizontalBox::Slot()
		            .Padding(40.0f, 40.0f)
		            .fill_width(0.3f)
		            .max_width(600.0f)
		            [
		                SNew(SBorder)
		                .BorderBackgroundColor(glm::vec4(0.43f, 0.2f, 0.8f, 1.0f))
		            ]
		            + SHorizontalBox::Slot()
		            .Padding(40.0f, 40.0f)
		            .fill_width(0.4f)
		            .max_width(600.0f)
		            [
		                SNew(SBorder)
		                .BorderImage(FCoreStyle::get().get_brush("Checkboard"))
		            ]
		            + SHorizontalBox::Slot()
		            .Padding(40.0f, 40.0f)
		            .fill_width(0.7f)
		            .max_width(600.0f)
		            [
		                SNew(SDockTab)
                        .Label("Label")
		            ]
				]
		
		    ];
		
		get().add_window(root_window2);

		std::shared_ptr<SWindow> root_window4;
		
		SAssignNew(root_window4, SWindow)
		    .Title("hello4")
		    .ClientSize(glm::vec2(800.0f, 600.0f))
		    .ScreenPosition(glm::vec2(100.0f, 100.0f))
		    [
		        SNew(SBorder)
		        .BorderBackgroundColor(glm::vec4(0.85f, 0.83f, 0.95f, 1.0f))//control background color
				.Padding(100.0f)
				[
					SNew(SSplitter)
		            + SSplitter::Slot()
					.SizeRule(SSplitter::FractionOfParent)
					.MinSize(20.0f)
					.Value(0.2f)
		            [
                        SNew(SComplexGradient)
                        .GradientColors(colors3)
		            ]
					+ SSplitter::Slot()
		            .SizeRule(SSplitter::FractionOfParent)
		            .MinSize(20.0f)
		            .Value(0.2f)
		            [
		                SNew(SComplexGradient)
		                .GradientColors(colors2)
		            ]
		            + SSplitter::Slot()
		            .SizeRule(SSplitter::FractionOfParent)
		            .MinSize(20.0f)
		            .Value(0.7f)
		            [
                        SNew(SComplexGradient)
                        .GradientColors(colors)
                        .Orientation(EOrientation::Orient_Horizontal)
		            ]
		            + SSplitter::Slot()
		            .SizeRule(SSplitter::FractionOfParent)
		            .MinSize(20.0f)
		            .Value(0.7f)
		            [
                        SNew(SComplexGradient)
                        .GradientColors(colors2)
                        .Orientation(EOrientation::Orient_Vertical)
		            ]
                    + SSplitter::Slot()
                    .SizeRule(SSplitter::FractionOfParent)
                    .MinSize(20.0f)
                    .Value(0.7f)
                    [
                        SNew(SButton)
                        .ForegroundColor(glm::vec4(0.9f, 0.3f, 0.2f, 1.0f))
                    ]
		        ]
		    ];
		
		get().add_window(root_window4);
        */
        return root_window;

		//std::shared_ptr<SWindow> root_window2;
		//
		//std::shared_ptr<SBorder> border3;
		//
		//std::shared_ptr<SBorder> border4;
		//
		//std::shared_ptr<SBorder> border5;
		//
		//std::shared_ptr<SConstraintCanvas> canvas;
		//
		//SAssignNew(root_window2, SWindow)
		//    .Title("hello2")
		//    .ClientSize(glm::vec2(1280.0f, 720.0f))
		//    .ScreenPosition(glm::vec2(200.0f, 200.0f))
		//    [
		//        SAssignNew(border3, SBorder)
		//        .BorderBackgroundColor(glm::vec4(0.9f, 0.2f, 0.4f, 1.0f))
		//        .Padding(150.0f)
		//        [
		//            SAssignNew(canvas, SConstraintCanvas)
		//            + SConstraintCanvas::Slot()
		//			.Anchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f))//middle
		//			.Offset(FMargin(0.0f, 0.0f, 200.0f, 200.0f))//position and size
		//            .Alignment(glm::vec2(0.5f, 0.5f))
		//			.AutoSize(false)
		//		    [
		//                SAssignNew(border5, SBorder)
		//                .BorderBackgroundColor(glm::vec4(0.6f, 0.3f, 0.4f, 1.0f))
		//            ]
		//            + SConstraintCanvas::Slot()
		//            .Anchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f))//middle
		//            .Offset(FMargin(0.0f, 0.0f, 147.0f, 253.0f))//position and size
		//            .Alignment(glm::vec2(0.5f, 0.5f))
		//            .AutoSize(false)
		//            [
		//                SNew(SBorder)
		//                .BorderBackgroundColor(glm::vec4(0.9f, 0.7f, 0.2f, 1.0f))
		//            ]
		//        ]
		//    ] ;
		//
		//
		//get().add_window(root_window2);
		//
		//std::shared_ptr<SWindow> root_window3;
		//
		//std::shared_ptr<SBorder> border6;
		//std::shared_ptr<SBorder> border7;
		//
		//SAssignNew(root_window3, SWindow)
		//    .Title("hello3")
		//    .ClientSize(glm::vec2(1280.0f, 720.0f))
		//    .ScreenPosition(glm::vec2(200.0f, 600.0f))
		//    [
		//        SAssignNew(border6, SBorder)
		//        .BorderBackgroundColor(glm::vec4(0.2f, 0.8f, 0.4f, 1.0f))
		//    .Padding(100.0f)
		//    [
		//        SAssignNew(border7, SBorder)
		//        .BorderBackgroundColor(glm::vec4(0.8f, 0.6f, 0.2f, 1.0f))
		//    ]
		//    ];
		//
		//get().add_window(root_window3);

		//for(size_t i = 0; i < 10; ++i)
		//{
		//    std::shared_ptr<SWindow> window;
		//
		//    SAssignNew(window, SWindow)
		//        .Title("hello")
		//        .ClientSize(glm::vec2(1280.0f, 720.0f))
		//        .ScreenPosition(glm::vec2(400.0f + i * 100, 400.0f + i * 100));
		//
		//    get().add_window(window);
		//}
    }

    void Application::Create()
    {
        //Create(GenericApplication::Create());

        Create(FPlatformApplicationMisc::create_application());
    }

    void Application::shut_down()
    {
        s_current_application->destroy_renderer();
    }

    std::shared_ptr<Application> Application::Create(const std::shared_ptr<GenericApplication>& in_platform_application)
    {
        //todo:initialize EKeys

        initialize_core_style();

        //return std::shared_ptr<Application>();
        s_platform_application = in_platform_application;//platform application

        s_current_application = std::make_shared<Application>();//slate application

        s_platform_application->set_message_handler(s_current_application);//note:this is important

        return s_current_application;
    }

    void Application::initialize_core_style()
    {
        //initialize FCoreStyle
        if (FCoreStyle::is_star_ship_style())
        {
            FStarshipCoreStyle::reset_to_default();//create a ISlateStyle
            FAppStyle::set_app_style(FStarshipCoreStyle::get_core_style());
        }

        FUMGCoreStyle::reset_to_default();
    }

	void Application::Initialize_Renderer(std::shared_ptr<Renderer> in_renderer)
	{
        //reset work directroy
        //std::filesystem::path work_path("../");
        //std::filesystem::current_path(work_path);
        
        //std::cout << std::filesystem::current_path() << std::endl;

        //std::cout << "Hello World" << std::endl;

        m_renderer = in_renderer;

        //m_renderer->
        //todo:call renderer initialize
        bool b_result = m_renderer->initialize();

        if(!b_result)
        {
            std::cout << "create vulkan context error!" << std::endl;
        }
        //m_renderer_instance = RendererInstance::Create(*m_p_window);

        //std::unique_ptr<DoDo::UIRenderer> p_Renderer = DoDo::UIRenderer::Create();    
	}

    glm::vec2 Application::get_cursor_size() const
    {
        if (s_platform_application->m_cursor)
        {
            int32_t x = 0;
            int32_t y = 0;

            //todo:from platform application to get cursor size
            s_platform_application->m_cursor->get_size(x, y);

            return glm::vec2(x, y);
        }

        return glm::vec2(1.0f, 1.0f);
    }

    glm::vec2 Application::get_cursor_pos() const
	{
        return get_cursor_user()->get_cursor_position();
	}

	glm::vec2 Application::get_last_cursor_pos() const
	{
        return get_cursor_user()->get_previous_cursor_position();
	}

    glm::vec2 Application::calculate_popup_window_position(const FSlateRect& in_anchor, const glm::vec2& in_size, bool b_auto_adjust_for_dpi_scale, const glm::vec2& in_proposed_placement, const EOrientation orientation) const
    {
        glm::vec2 calculated_pop_up_window_position(0.0f, 0.0f);

        float dpi_scale = 1.0f;

        if (b_auto_adjust_for_dpi_scale)
        {
            dpi_scale = FPlatformApplicationMisc::get_dpi_scale_factor_at_point(in_anchor.left, in_anchor.top);

            glm::vec2 adjusted_size = in_size * dpi_scale;

            FPlatformRect anchor_rect;
            anchor_rect.left = in_anchor.left;
            anchor_rect.top = in_anchor.top;
            anchor_rect.right = in_anchor.right;
            anchor_rect.bottom = in_anchor.bottom;

            EPopUpOrientation::Type pop_up_orientation = EPopUpOrientation::Horizontal;

            if (orientation == EOrientation::Orient_Vertical)
            {
                pop_up_orientation = EPopUpOrientation::Vertical;
            }

            {
                //calculate the rectangle around our work area
                //user our own rect, this window as probably doesn't have a size or position yet
                //use a size of 1 to get the closest monitor to the start point
                FPlatformRect work_area_finder_rect(anchor_rect);

                work_area_finder_rect.right = anchor_rect.left + 1;
                work_area_finder_rect.bottom = anchor_rect.top + 1;
                const FPlatformRect platform_work_area = s_platform_application->get_work_area(work_area_finder_rect);

                const FSlateRect work_area_rect(
                    platform_work_area.left,
                    platform_work_area.top,
                    platform_work_area.right,
                    platform_work_area.bottom
                );

                glm::vec2 proposed_placement = in_proposed_placement;

                if (proposed_placement == glm::vec2(0.0f))
                {
                    //assumes natural left-to-right, top-to-bottom flow, position popup below and to the right
                    proposed_placement = glm::vec2(
                        orientation == Orient_Horizontal ? anchor_rect.right : anchor_rect.left,
                        orientation == Orient_Vertical ? anchor_rect.top : anchor_rect.bottom
                    );
                }

                return compute_pop_up_fit_in_rect(in_anchor, FSlateRect(proposed_placement, proposed_placement + adjusted_size), orientation, work_area_rect) / dpi_scale;
            }
        }
    }

    glm::vec2 Application::calculate_tooltip_window_position(const FSlateRect& in_anchor_rect, const glm::vec2& in_size, bool b_auto_adjust_for_dpi_scale) const
    {
        //first use the calculate popup window position and if cursor is not inside it, proceed with it to avoid behaviour change
        glm::vec2 pop_up_position = calculate_popup_window_position(in_anchor_rect, in_size, b_auto_adjust_for_dpi_scale);

        return pop_up_position;
    }

    void Application::Tick()
    {
        const float delta_time = get_delta_time();
        //todo:implement TickPlatform
        //TickPlatform is just something to handle message
        tick_platform(delta_time);

        //todo:implement TickTime
        tick_time();

        //todo:implement TickAndDrawWidgets
        Tick_And_Draw_Widgets(delta_time);

        //TODO:in the future, move this window to SWindow
        //m_p_window->Update(*m_renderer_instance);
    }

    Window& Application::get_window()
    {
        return *m_p_window;
    }

    void Application::destroy_renderer()
    {
        if(m_renderer)
        {
            m_renderer->destroy();
        }
    }

    void Application::for_each_user(std::function<void(FSlateUser&)> in_prediacte, bool b_include_virtual_users)
    {
        for (const std::shared_ptr<FSlateUser>& user : m_users)
        {
            //ignore virtual users unless told not do
            if (user)//todo:check FSlateUser is virtual user
            {
                in_prediacte(*user);
            }
        }
    }

    std::shared_ptr<FSlateUser> Application::get_or_create_user(int32_t user_index)
    {
        if(std::shared_ptr<FSlateUser> found_user = get_user(user_index))
        {
            return found_user;
        }
        return register_new_user(user_index);
    }

    std::shared_ptr<FSlateUser> Application::register_new_user(int32_t user_index)
    {
        //todo:check

        std::shared_ptr<ICursor> user_cursor;
        if(user_index == m_cursor_user_index && s_platform_application->m_cursor)//from platform to get cursor
        {
            user_cursor = s_platform_application->m_cursor;
        }

        std::shared_ptr<FSlateUser> new_user = FSlateUser::Create(user_index, user_cursor);

        if(user_index >= m_users.size())
        {
            m_users.resize(user_index + 1);
        }

        m_users[user_index] = new_user;

        //broad cast to user register

        return new_user;
    }

    FPointerEvent Application::transform_pointer_event(const FPointerEvent& pointer_event, const std::shared_ptr<SWindow>& window) const
    {
        FPointerEvent transformed_pointer_event = pointer_event;

        if (window)
        {
            //todo:implement this function
        }

        return transformed_pointer_event;
    }

    void Application::tick_time()
    {
        m_last_tick_time = m_current_time;
        //m_current_time =
        //todo:implement FPlatformTime
        m_current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        if(m_current_time - m_last_time > 1.0f)
        {
            m_last_time = m_current_time;
            m_last_frame_count = m_frame_count;
            m_frame_count = 0;
        }
        ++m_frame_count;
    }

    void Application::tick_platform(float delta_time)
    {
        s_platform_application->pump_messages(delta_time);
        //todo:handle messenger
        s_platform_application->Tick(delta_time);

        //todo:process deferred events

        //note:update cursor and tooltip
        auto predicate = [this](FSlateUser& user) {
            user.update_cursor();

            //user.update_tool_tip();
        };
        for_each_user(predicate);
    }

    void Application::Tick_And_Draw_Widgets(float delta_time)
    {
        //todo:implement pre tick event delegate
        {
            m_pre_tick_event.Invoke(delta_time);//broad cast
        }
        /*
         * update average time between ticks
         * this is used to monitor how responsive the application "feels"
         */
        {
	        /*
	         * scalar percent of new delta time that contributes to running average
	         * use a lower value to add more smoothing to the average frame rate
	         * a value of 1.0 will disable smoothing
	         */
            const float running_average_scale = 0.1f;

            m_average_delta_time = m_average_delta_time * (1.0f - running_average_scale) + get_delta_time() * running_average_scale;

        }

        //draw all windows
        Draw_Windows();

        {
            m_post_tick_event.Invoke(delta_time);//broad cast
        }
    }

    void Application::Draw_Windows()
    {
        Private_Draw_Windows();
    }

    void Application::Private_Draw_Windows(std::shared_ptr<SWindow> draw_only_this_window)
    {
        //todo:implement FWidgetPath

        //prepass the window
        Draw_Pre_Pass(draw_only_this_window);

        //todo:implement FDrawWindowArgs
        //todo:every frame to get draw buffer, will clear last frame data
        FDrawWindowArgs draw_window_args(m_renderer->get_draw_buffer());

        //todo:implement SWindow
        //get the active modal window
        //todo:implement Draw Window And Children

        //todo:draw tooltip windows

        //todo:draw notification windows

        //draw all windows
        //use of an old-style iterator is intentional here, as slate windows
        //array may be mutated by user logic in draw calls, the iterator
        //prevents us from reading off the end and only keeps an index
        //internally
        for (std::vector<std::shared_ptr<SWindow>>::const_iterator itr = m_windows.begin(); itr != m_windows.end(); ++itr)
        {
            std::shared_ptr<SWindow> current_window = *itr;

            //only draw visible windows or in off-screen rendering mode
            //todo:need pass FDrawWindowArgs
            Draw_Window_And_Children(current_window, draw_window_args);//generate draw elements
        }

        //todo:implement renderer's draw windows
        //m_renderer_instance
        m_renderer->draw_windows(draw_window_args.m_out_draw_buffer);
    }

    static void prepass_window_and_children(std::shared_ptr<SWindow> window_to_prepass)
    {
        const bool b_is_window_visible = window_to_prepass->is_visible();//todo:add check is window minimized

        if(b_is_window_visible)
        {
	        //todo:process window invalidation
            {
                window_to_prepass->slate_prepass(1.0f);//todo:pass dpi scale
            }

            //note:iterate over copy since num children can change during resize above
            std::vector<std::shared_ptr<SWindow>> child_windows(window_to_prepass->get_child_windows());
            for(const std::shared_ptr<SWindow>& child_window : child_windows)
            {
                prepass_window_and_children(child_window);
            }
        }
    }

    void Application::Draw_Pre_Pass(std::shared_ptr<SWindow> draw_only_this_window)
    {
        if(draw_only_this_window)
        {
            prepass_window_and_children(draw_only_this_window);
        }
        else
        {
	        //draw all windows
            for(const std::shared_ptr<SWindow>& current_window : m_windows)
            {
                prepass_window_and_children(current_window);
            }
        }
    }

    void Application::Draw_Window_And_Children(const std::shared_ptr<SWindow>& window_to_draw, struct FDrawWindowArgs& draw_window_args)
    {
        //todo:lack the FDrawWindowArgs parameter

        /*
         * on mac, where child windows can be on screen if their parent is hidden or minimized, we want to always draw child windows
         * on other platforms we set bDrawChildWindows to true only if we draw the current window
         */

        //todo:this is a mac platform macro
        //bool b_Draw_Child_Windows = ;

        //only draw visible windows or in off-screen rendering mode


        //get the draw window args's out draw buffer to add window element list
        //this function will clear last frame element data
        FSlateWindowElementList& window_element_list = draw_window_args.m_out_draw_buffer.add_window_element_list(window_to_draw);

        //todo:call SWindow's paint window function
        //drawing is done in window space, so null out the positions and keep the size
        int32_t max_layer_id = 0;
        max_layer_id = window_to_draw->paint_window(
            m_current_time,
            (m_current_time - m_last_tick_time),
            window_element_list,
            FWidgetStyle(),
            window_to_draw->Is_Enabled()
        );

        //draw windowless drag drop operations

        //draw software cursors

        //todo:set bDrawChildWindows to true

        //todo:draw the child windows
    }

    FWidgetPath Application::locate_widget_in_window(glm::vec2 screen_space_mouse_coordinate,
	    const std::shared_ptr<SWindow>& window, bool b_ignore_enabled_status, int32_t user_index) const
    {
        //locate widget
        const bool b_accepts_input = window->is_visible();//todo:check window accepts input

        //if(b_accepts_input && window->is)
        //note:screen_space_mouse_coordinate is in local space
        if(b_accepts_input && window->is_screen_space_mouse_within(screen_space_mouse_coordinate))
        {
            glm::vec2 cursor_position = screen_space_mouse_coordinate;

            //todo:handle full screen

            //todo:implement get bubble path
            //todo:implement get cursor radius
            std::vector<FWidgetAndPointer> widgets_and_cursors = window->get_hittest_grid().get_bubble_path(cursor_position, 0.3f, b_ignore_enabled_status, user_index);
            return FWidgetPath(widgets_and_cursors); 
        }
        else
        {
            return FWidgetPath();
        }
    }

    std::vector<std::shared_ptr<SWindow>> Application::get_interactive_top_level_windows()
    {
        //todo:add modal window handle

        return m_windows;
    }

    bool Application::process_mouse_move_event(const FPointerEvent& mouse_event, bool b_is_synthetic)
    {
        //todo:implement input preprocessors and update tool tip

        /*
         * when the event came from the OS, we are guaranteed to be over a slate window
         * otherwise, we are synthesizing a MouseMove ourselves, and must verify that the
         * cursor is indeed over a slate window, synthesized device input while
         * the application is inactive also needs to populate the widget path
         */
        const bool b_over_slate_window = !b_is_synthetic;//todo:implement is_cursor_directly_over_slate_window

        FWidgetPath widget_under_cursor = b_over_slate_window ? locate_window_under_mouse(mouse_event.get_screen_space_position(),
            m_windows, false, -1) : FWidgetPath();//todo:implement locate window under mouse


        bool b_result = route_pointer_move_event(widget_under_cursor, mouse_event, b_is_synthetic);

        //todo:implement route pointer move event

        return b_result;
    }

    bool Application::process_mouse_button_down_event(const std::shared_ptr<Window>& platform_window,
	    const FPointerEvent& in_mouse_event)
    {
        FReply reply = FReply::un_handled();

        //only process mouse down messages if we are not drag/dropping
        std::shared_ptr<FSlateUser> slate_user = get_or_create_user(in_mouse_event);

        FWidgetPath widgets_under_cursor = locate_window_under_mouse(in_mouse_event.get_screen_space_position(), m_windows, false, slate_user->get_user_index());

        reply = route_pointer_down_event(widgets_under_cursor, in_mouse_event);

        return true;
    }

    bool Application::process_mouse_button_up_event(const FPointerEvent& mouse_event)
    {
        //an empty widget path is passed in, as an optimization, one will be generated only if a captures mouse event isn't routed
        FWidgetPath empty_path;
        const bool b_handled = route_pointer_up_event(empty_path, mouse_event).is_event_handled();

        return b_handled;
    }

    std::shared_ptr<SWindow> Application::add_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately)
    {
        /*
        * add the slate window to the slate application's top-level window array, note that neither the slate window
        * or the active window are ready to br used yet, however we need to make sure they're in the slate window
        * array so that we can properly respond to OS window messages as soon as they're sent, for example, a window
        * activation message may be sent by the OS as soon as the window is shown(in the init function), and if we
        * don't add the slate window to our window list, we wouldn't be able to route that message to the window
        */
        //todo:implement arrange window to front
        m_windows.push_back(in_slate_window);

        std::shared_ptr<Window> new_window = make_window(in_slate_window, b_show_immediately);

        if (b_show_immediately)
        {
            //todo:implement show window
            in_slate_window->show_window();//create swap chain and show window
        }

        return in_slate_window;
    }
    std::shared_ptr<Window> Application::make_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately)
    {
        std::shared_ptr<Window> native_parent = nullptr;
        std::shared_ptr<SWindow> parent_window = in_slate_window->get_parent_window();

        if (parent_window) //check valid
        {
            native_parent = parent_window->get_native_window();//get parent SWindow's native window
        }

        //get the SWindow's attribute to initialize native window's initialize information
        std::shared_ptr<FGenericWindowDefinition> definition = std::make_shared<FGenericWindowDefinition>();

        definition->m_type = in_slate_window->get_type();

        const glm::vec2 size = in_slate_window->get_initial_desired_size_in_screen();
        definition->m_width_desired_on_screen = size.x;
        definition->m_height_desired_on_screen = size.y;

        const glm::vec2 position = in_slate_window->get_initial_desired_position_in_screen();
        definition->x_desired_position_on_screen = position.x;
        definition->y_desired_position_on_screen = position.y;

        //todo:implement platform application's make window
        std::shared_ptr<Window> new_window = s_platform_application->make_window();

        in_slate_window->set_native_window(new_window);

        s_platform_application->initialize_window(new_window, definition, native_parent, b_show_immediately);

        return new_window;
    }

    FWidgetPath Application::locate_window_under_mouse(glm::vec2 screen_space_mouse_coordinate,
	    const std::vector<std::shared_ptr<SWindow>>& windows, bool b_ignore_enabled_status, int32_t user_index)
    {
        //todo:give the os a chance to tell use which window to use, in case a child window is not guaranteed to stay on top of it's parent window
        std::shared_ptr<Window> native_window_under_mouse = s_platform_application->get_window_under_cursor();

        if(native_window_under_mouse)
        {
            std::shared_ptr<SWindow> window = FSlateWindowHelper::find_window_by_platform_window(windows, native_window_under_mouse);//todo:implement FSlateWindowHelper

            if(window)
            {
                return locate_widget_in_window(screen_space_mouse_coordinate, window, b_ignore_enabled_status, user_index);
            }
        }

        //todo:implement this

        for(int32_t window_index = (int32_t)windows.size() - 1; window_index >= 0; --window_index)
        {
            const std::shared_ptr<SWindow>& window = windows[window_index];

            //todo:check window visible and window minimized

            //hittest the window's children first
            FWidgetPath resulting_path = locate_window_under_mouse(screen_space_mouse_coordinate, window->get_child_windows(), b_ignore_enabled_status, user_index);//recursive
            if(resulting_path.is_valid())
            {
                return resulting_path;
            }

            //if none of the children were hit, hittest the parent

            //only accept input if the current window accepts input and the current window is not under a modal window or an interactive tooltip

            //todo:implement this
            FWidgetPath path_to_located_widget = locate_widget_in_window(screen_space_mouse_coordinate, window, b_ignore_enabled_status, user_index);
            if(path_to_located_widget.is_valid())
            {
                return path_to_located_widget;
            }
        }

        return FWidgetPath();
    }

    FReply Application::route_pointer_down_event(const FWidgetPath& widgets_under_pointer,
	    const FPointerEvent& pointer_event)
    {
        FPointerEvent transformed_pointer_event = pointer_event;//todo:implement transform pointer event

        std::shared_ptr<FSlateUser> slate_user = get_or_create_user(pointer_event);

        //todo:update pointer position

        FReply reply = FEventRouter::Route<FReply>(this, FEventRouter::FBubblePolicy(widgets_under_pointer), transformed_pointer_event, [this](const FArrangedWidget& target_widget, const FPointerEvent& event)
        {
            FReply temp_reply = FReply::un_handled();

            if(!temp_reply.is_event_handled())
            {
                temp_reply = target_widget.m_widget->On_Mouse_Button_On_Down(target_widget.m_geometry, event);
            }

            return temp_reply;
        });

        return reply;
    }

    FReply Application::route_pointer_up_event(const FWidgetPath& widgets_under_pointer,
	    const FPointerEvent& pointer_event)
    {
        FPointerEvent transformed_pointer_event = pointer_event;//todo:implement transform pointer event

        std::shared_ptr<FSlateUser> slate_user = get_or_create_user(pointer_event);

        FWidgetPath local_widgets_under_pointer = widgets_under_pointer;

        FReply reply = FReply::un_handled();

        if(!widgets_under_pointer.is_valid())
        {
	        //generate a event
            local_widgets_under_pointer = locate_window_under_mouse(pointer_event.get_screen_space_position(), m_windows, false, slate_user->get_user_index());
        }

        reply = FEventRouter::Route<FReply>(this, FEventRouter::FBubblePolicy(local_widgets_under_pointer), transformed_pointer_event, [&](const FArrangedWidget& cur_widget, const FPointerEvent& event)
        {
            FReply temp_reply = FReply::un_handled();

            temp_reply = cur_widget.m_widget->On_Mouse_Button_On_Up(cur_widget.m_geometry, event);

            return temp_reply;
        });

        //slate_user->notify_pointer_released(pointer_event, local_widgets_under_pointer, reply.is_event_handled());//todo:modify add drag and drop

        return reply;
    }

    bool Application::route_pointer_move_event(const FWidgetPath& widgets_under_pointer,
                                               const FPointerEvent& pointer_event, bool b_is_synthetic)
    {
        bool b_handled = false;

        std::shared_ptr<FSlateUser> slate_user = get_or_create_user(pointer_event);
        slate_user->notify_pointer_move_begin(pointer_event);

        //bubble the mouse move event
        FReply reply = FEventRouter::Route<FReply>(this, FEventRouter::FBubblePolicy(widgets_under_pointer), pointer_event,
         [=](const FArrangedWidget& cur_widget, const FPointerEvent& event)
        {
            FReply temp_reply = FReply::un_handled();

            if(!temp_reply.is_event_handled())
            {
                temp_reply = cur_widget.m_widget->On_Mouse_Move(cur_widget.m_geometry, event);
            }

            return temp_reply;
        });

        b_handled = reply.is_event_handled();

        slate_user->notify_pointer_move_complete(pointer_event, widgets_under_pointer);

        return b_handled;
    }

    bool Application::does_widget_have_mouse_capture(const std::shared_ptr<const SWidget> widget) const
    {
        for (const std::shared_ptr<FSlateUser>& user : m_users)
        {
            if (user && user->does_widget_have_any_capture(widget))
            {
                return true;
            }
        }
        return false;
    }

    bool Application::does_widget_have_mouse_capture_by_user(const std::shared_ptr<const SWidget> widget, int32_t user_index, std::optional<int32_t> pointer_index) const
    {
        if (std::shared_ptr<const FSlateUser> found_user = get_user(user_index))
        {
            return pointer_index.has_value() ? found_user->does_widget_have_capture(widget, pointer_index.value()) : found_user->does_widget_have_any_capture(widget);
        }

        return false;
    }

    DoDoUtf8String Application::calculate_frame_per_second() const
    {
        return DoDoUtf8String(std::string("FPS:") + std::to_string(m_last_frame_count));
    }

    void Application::test_slider_value_changed(float new_value)
    {
        TAttribute<DoDoUtf8String> text(std::to_string(new_value));
        m_text_block->set_text(text);
    }

    void Application::test_color_wheel_value_changed(FSlateColor new_color)
    {
        m_border->set_border_back_ground_color(new_color);
    }

    static std::shared_ptr<SDockTab> spawn_star_ship_widgets(const FSpawnTabArgs& spawn_tab_args)
    {
        return SNew(SDockTab)
               //.ContentPadding(FMargin(0.0f, 0.0f, 200.0f, 200.0f))
               .ForegroundColor(FLinearColor(0.6f, 0.3f, 0.2f, 0.2f))
               [
                   SNew(SBorder)
                   .ColorAndOpacity(FLinearColor(1.0f, 0.8f, 0.7f, 1.0f))
					[
                        SNew(SImage)
                        .Image(FAppStyle::get().get_brush("Icons.solar-system"))
                    ]
               ];
    }

    void Application::test_create_sdock()
    {
        std::shared_ptr<FTabManager::FLayout> layout = FTabManager::new_layout("StarshipStyleGallery")
        ->add_area
        (
            FTabManager::new_area(1280, 720)
            ->split //note:put new node in FArea nodes array
            (
                FTabManager::new_stack()
                ->add_tab("starship widgets", ETabState::OpenedTab) //note:first parameter is tab type
                ->add_tab("test widget", ETabState::OpenedTab)
                ->set_foreground_tab(DoDoUtf8String("starship widgets"))
            )
			//->split
			//(
			//    FTabManager::new_stack()
			//    ->add_tab("test widget", ETabState::OpenedTab)
			//    ->set_foreground_tab(DoDoUtf8String("test widget"))
			//)
        );

        FGlobalTabmanager::get()->register_nomad_tab_spawner("starship widgets", FOnSpawnTab::CreateStatic(spawn_star_ship_widgets));//todo:implement this function
        FGlobalTabmanager::get()->register_nomad_tab_spawner("test widget", FOnSpawnTab::CreateStatic(spawn_star_ship_widgets));

        FGlobalTabmanager::get()->restore_from(layout, std::shared_ptr<SWindow>());
    }

    void Application::process_reply(const FWidgetPath& current_event_path, const FReply& the_reply, const FWidgetPath* widgets_under_mouse, const FPointerEvent* in_mouse_event, const int32_t m_user_index)
    {
        //release mouse capture if requested or if we are starting a drag and drop
        //make sure to only clobber widgets under cursor if we actually had a mouse capture
        uint32_t pointer_index = in_mouse_event != nullptr ? in_mouse_event->get_pointer_index() : m_cursor_pointer_index;//note:pointer index is finger index

        std::shared_ptr<FSlateUser> slate_user = get_or_create_user(m_user_index);
        if (slate_user->has_capture(pointer_index) && (the_reply.should_release_mouse()))//todo:implement should release mouse
        {
            slate_user->release_capture(pointer_index);
        }
        //todo:check slate user have capture and release capture

        //todo:clear focus if requested

        //todo:cancel drag and drop

        //todo:add app is active check
        std::shared_ptr<SWidget> requested_mouse_captor = the_reply.get_mouse_captor();
        if (requested_mouse_captor)//todo:check starting drag and drop
        {
            if (slate_user->set_pointer_captor(pointer_index, requested_mouse_captor, current_event_path))//pointer is finger
            {
                if (widgets_under_mouse)
                {
                    const FWeakWidgetPath last_widgets_under_cursor = slate_user->get_last_widgets_under_pointer(pointer_index);//note:weak widget path don't have geometry

                    if (last_widgets_under_cursor.is_valid())//note:have at least one widget in path
                    {
                        //note:get last widget under cursor and check is same? if doesn't call on mouse leave
                        for (int32_t widget_index = last_widgets_under_cursor.m_widgets.size() - 1; widget_index >= 0; --widget_index)//note:from last to first traver
                        {
                            std::shared_ptr<SWidget> widget_previously_under_cursor = last_widgets_under_cursor.m_widgets[widget_index].lock();

                            if (widget_previously_under_cursor)
                            {
                                if (widget_previously_under_cursor != requested_mouse_captor)//check is capture mouse, if not, then notify on mouse level
                                {
                                    //it's possible for mouse event to be null if we end up here from a keyboard event, if so, we should synthesize an event
                                    if (in_mouse_event)
                                    {
                                        FPointerEvent transformed_pointer_event = transform_pointer_event(*in_mouse_event, widgets_under_mouse->get_window());

                                        //note that the event's pointer position is not translated
                                        //todo:implement widget on mouse leave
                                        widget_previously_under_cursor->On_Mouse_Leave(transformed_pointer_event);
                                    }
                                    else
                                    {
                                        const FPointerEvent& simulated_pointer = FPointerEvent();
                                        //todo:implement widget on mouse leave
                                        widget_previously_under_cursor->On_Mouse_Leave(simulated_pointer);
                                    }
                                }
                                else
                                {
                                    //don't routing mouse leave
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            //when the cursor capture state changes we need to refresh cursor state
            slate_user->request_cursor_query();//note:this will be trigger by query_cursor() function
        }
    }

    void Application::set_cursor_pos(const glm::vec2 mouse_coordinate)
    {
        get_cursor_user()->set_cursor_position(mouse_coordinate);
    }

    FKey translate_mouse_button_to_key(const EMouseButtons::Type button)
    {
        FKey key = EKeys::Invalid;

        switch(button)
        {
        case EMouseButtons::Left:
            key = EKeys::LeftMouseButton;
            break;
        case EMouseButtons::Middle:
            key = EKeys::MiddleMouseButton;
            break;
        case EMouseButtons::Right:
            key = EKeys::RightMouseButton;
            break;
        case EMouseButtons::Thumb01:
            key = EKeys::ThumbMouseButton;
            break;
        case EMouseButtons::Thumb02:
            key = EKeys::ThumbMouseButton2;
            break;
        }

        return key;
    }

    bool Application::On_Mouse_Move()
    {
        bool result = true;

        const glm::vec2 current_cursor_position = get_cursor_pos();
        const glm::vec2 last_cursor_position = get_last_cursor_pos();//todo:implement get cursor pos

        //if(current_cursor_position != last_cursor_position)
        //{
	        //todo:implement last mouse move time
            FPointerEvent mouse_event(
                get_cursor_user()->get_user_index(),//todo:implement get user index for mouse
                m_cursor_pointer_index,
                current_cursor_position,
                last_cursor_position,
                m_pressed_mouse_buttons,
                EKeys::Invalid,
                0,
                s_platform_application->get_modifier_keys()
                );

            result = process_mouse_move_event(mouse_event);
        //}

        return result;
    }

    bool Application::On_Mouse_Move(double x, double y)
    {
        bool result = true;

        const glm::vec2 current_cursor_position = glm::vec2(x, y);
        const glm::vec2 last_cursor_position = get_last_cursor_pos();//todo:implement get cursor pos

        //if(current_cursor_position != last_cursor_position)
        //{
            //todo:implement last mouse move time
        FPointerEvent mouse_event(
            get_cursor_user()->get_user_index(),//todo:implement get user index for mouse
            m_cursor_pointer_index,
            current_cursor_position,
            last_cursor_position,
            m_pressed_mouse_buttons,
            EKeys::Invalid,
            0,
            s_platform_application->get_modifier_keys()
        );

        result = process_mouse_move_event(mouse_event);
        //}

        return result;
    }

    bool Application::On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button)
    {
	    return Application::On_Mouse_Down(window, button, get_cursor_pos());
    }

    bool Application::On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button,
                                    const glm::vec2 cursor_pos)
    {
        FKey key = translate_mouse_button_to_key(button);

        FPointerEvent mouse_event(
            get_cursor_user()->get_user_index(),
            m_cursor_pointer_index,
            cursor_pos,
            get_last_cursor_pos(),
            m_pressed_mouse_buttons,
            key,
            0,
            s_platform_application->get_modifier_keys()
        );

        return process_mouse_button_down_event(window, mouse_event);
    }

    bool Application::On_Mouse_Up(const EMouseButtons::Type button)
    {
        return On_Mouse_Up(button, get_cursor_pos());
    }

    bool Application::On_Mouse_Up(const EMouseButtons::Type button, const glm::vec2 cursor_pos)
    {
        FKey key = translate_mouse_button_to_key(button);

        FPointerEvent mouse_event(
            get_cursor_user()->get_user_index(),
            m_cursor_pointer_index,
            cursor_pos,
            get_last_cursor_pos(), m_pressed_mouse_buttons,
            key,
            0,
            s_platform_application->get_modifier_keys()
        );

        return process_mouse_button_up_event(mouse_event);
    }

    bool Application::On_Size_Changed(const std::shared_ptr<Window>& native_window, const int32_t width, const int32_t height,
	    bool b_was_minimized)
    {
        std::shared_ptr<SWindow> window = FSlateWindowHelper::find_window_by_platform_window(m_windows, native_window);

        if(window)
        {
            window->set_cached_size(glm::vec2(width, height));//note:store the new size

            m_renderer->request_resize(window, width, height);//note:recreate viewport

            if(!b_was_minimized)
            {
                Private_Draw_Windows(window);//note:immediately draw windows
            }

            //todo:implement flush commands

        }
        return true;
    }

    bool Application::On_Cursor_Set()
    {
        get_cursor_user()->request_cursor_query();//mark to update cursor

        return true;
    }

    FModifierKeyState Application::get_modifier_keys() const
    {
        return s_platform_application->get_modifier_keys();
    }

    const std::set<FKey>& Application::get_pressed_mouse_buttons() const
    {
        return m_pressed_mouse_buttons;
    }

    std::shared_ptr<SWindow> Application::get_first_window() {
        //todo:fix me
        return m_windows[0];//return first window
    }

    bool Application::make_platform_window_and_create_view_port(std::shared_ptr<SWindow> in_window) {
        in_window->show_window();//create swap chain and viewport
        return true;
    }
}
