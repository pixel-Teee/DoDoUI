#pragma once

#include "Core/Core.h"

namespace DoDo {
	class Shader;
	class PipelineStateObject
	{
	public:
		//PipelineStateObject();

		virtual ~PipelineStateObject();

		virtual void Destroy(void* logic_device) = 0;

		virtual void* get_native_handle() = 0;

		virtual void set_vertex_shader(Shader& vertex_shader, const std::string& target_point) = 0;

		virtual void set_pixel_shader(Shader& pixel_shader, const std::string& target_point) = 0;

		virtual void set_input_vertex_layout(void* input_layout) = 0;

		virtual void* get_render_pass_native_handle() = 0;

		virtual void set_render_pass(void* render_pass) = 0;

		virtual void finalize(void* logic_device) = 0;

		static Scope<PipelineStateObject> Create(void* logic_device);
	};
}