#pragma once

#include "Core/Core.h"

#include "glm/glm.hpp"//glm::vec2

#include "Slate/Framework/Text/IRun.h"//FLayoutBlockTextContext depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

namespace DoDo {
	class IRun;
	class IRunRenderer;
	class ILayoutBlock
	{
	public:

		virtual ~ILayoutBlock() {}

		virtual std::shared_ptr<IRun> get_run() const = 0;

		virtual FTextRange get_text_range() const = 0;

		virtual glm::vec2 get_size() const = 0;

		virtual FLayoutBlockTextContext get_text_context() const = 0;

		virtual std::shared_ptr<IRunRenderer> get_renderer() const = 0;

		virtual glm::vec2 get_location_offset() const = 0;

		virtual void set_location_offset(const glm::vec2& in_location_offset) = 0;
	};
}