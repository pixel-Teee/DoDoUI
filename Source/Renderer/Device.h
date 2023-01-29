#pragma once

#include "Core/Core.h"

namespace DoDo {
	//physical device is adapter
	//logic device
	struct DeletionQueue;
	class Device {
	public:
		virtual ~Device();

		//todo:need to remove this function
		virtual void destroy() = 0;

		virtual void* get_native_handle() = 0;

		virtual void* get_graphics_queue() = 0;
										
		virtual void* get_present_queue() = 0;

		static Scope<Device> CreateDevice(void* adapter, void* surface);

		static std::shared_ptr<Device> create(void* adapter, DeletionQueue& deletion_queue);
	};
}