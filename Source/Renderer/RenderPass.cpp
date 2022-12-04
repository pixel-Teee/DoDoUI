#include <PreCompileHeader.h>

#include "RenderPass.h"

namespace DoDo {
	RenderPass::~RenderPass()
	{
	}
	Scope<RenderPass> RenderPass::Create()
	{
		return Scope<RenderPass>();
	}
}