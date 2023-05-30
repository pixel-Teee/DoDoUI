#pragma once

#include "Core/Core.h"

namespace DoDo {
	struct FGenericWindowDefinition;
	class RendererInstance;
	class Window {
	public:
		virtual void Update(RendererInstance& render_instance);

		virtual ~Window() {}

		virtual void* get_window_native_handle();

		static Scope<Window> Create();

		/*@return true if native window exists underneath the coordinates*/
		virtual bool is_point_in_window(int32_t x, int32_t y) const;

		/*native windows should implement move window to by relocating the platform-specific window to (x, y)*/
		virtual void move_window_to(int32_t x, int32_t y);

		virtual void reshape_window(int32_t new_x, int32_t new_y, int32_t new_width, int32_t new_height);

		/*
		* sets the opacity of this window
		* 
		* @param InOpacity the new window opacity represented as a floating scalar
		*/
		virtual void set_opacity(const float in_opacity);

		/*native windows should implement this function by asking the os to destroy os-specific resource associated with the window*/
		virtual void destroy();

		/*
		* @return ratio of pixels to slate units in this window
		* e.g. dpi scale of 2.0 means there is 2x2 pixel square for evey 1x1 slateunit
		*/
		virtual float get_dpi_scale_factor() const;

	protected:
		std::shared_ptr<FGenericWindowDefinition> m_definition;
	};
}