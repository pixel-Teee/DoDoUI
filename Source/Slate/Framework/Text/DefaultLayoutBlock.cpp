#include "PreCompileHeader.h"

#include "DefaultLayoutBlock.h"

namespace DoDo {

	std::shared_ptr<IRun> FDefaultLayoutBlock::get_run() const
	{
		return m_run;
	}

	FTextRange FDefaultLayoutBlock::get_text_range() const
	{
		return m_range;
	}

	glm::vec2 FDefaultLayoutBlock::get_size() const
	{
		return m_size;
	}

	FLayoutBlockTextContext FDefaultLayoutBlock::get_text_context() const
	{
		return m_text_context;
	}

	std::shared_ptr<IRunRenderer> FDefaultLayoutBlock::get_renderer() const
	{
		return m_renderer;
	}

	glm::vec2 FDefaultLayoutBlock::get_location_offset() const
	{
		return m_location_offset;
	}

	void FDefaultLayoutBlock::set_location_offset(const glm::vec2& in_location_offset)
	{
		m_location_offset = in_location_offset;
	}

}