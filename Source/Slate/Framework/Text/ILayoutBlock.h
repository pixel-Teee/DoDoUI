#pragma once

#include "Core/Core.h"

namespace DoDo {
	class IRun;
	class IRunRenderer;
	class ILayoutBlock
	{
	public:

		virtual ~ILayoutBlock() {}

		virtual std::shared_ptr<IRun> get_run() const = 0;

		virtual std::shared_ptr<IRunRenderer> get_renderer() const = 0;
	};
}