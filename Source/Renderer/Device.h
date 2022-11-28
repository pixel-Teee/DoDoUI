#pragma once

#include "Core/Core.h"

namespace DoDo {
	//physical device is adapter
	//logic device
	class Device {
	public:
		virtual ~Device();

		virtual void destroy() = 0;

		virtual void* get_native_handle() = 0;

		virtual void* get_graphics_queue() = 0;
										
		virtual void* get_present_queue() = 0;

		static Scope<Device> CreateDevice(void* adapter, void* surface);
	};
}