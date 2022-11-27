#pragma once

#include "Core/Core.h"

namespace DoDo {
	class Shader {
	public:
		virtual void* get_native_handle() = 0;

		virtual ~Shader();

		virtual void Destroy(void* logic_device) = 0;

		static Scope<Shader> Create(const std::string& file_path, void* logic_device);
	};
}