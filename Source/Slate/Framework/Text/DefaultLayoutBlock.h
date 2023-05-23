#pragma once

#include "Slate/Framework/Text/ILayoutBlock.h"

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

namespace DoDo {
	class FDefaultLayoutBlock : public ILayoutBlock
	{
	public:

		static std::shared_ptr<FDefaultLayoutBlock> Create(const std::shared_ptr<IRun>& in_run, const FTextRange& in_range, glm::vec2 in_size, const FLayoutBlockTextContext& in_text_context)
		{
			return std::make_shared<FDefaultLayoutBlock>(in_run, in_range, in_size, in_text_context);
		}

		FDefaultLayoutBlock(const std::shared_ptr<IRun>& in_run, const FTextRange& in_range, glm::vec2 in_size, const FLayoutBlockTextContext& in_text_context)
			: m_run(in_run)
			, m_range(in_range)
			, m_size(in_size)
			, m_location_offset(glm::vec2(0.0f))
			, m_text_context(in_text_context)
		{}

		virtual ~FDefaultLayoutBlock() {}

		virtual std::shared_ptr<IRun> get_run() const;

		virtual FTextRange get_text_range() const override;

		virtual glm::vec2 get_size() const;

		virtual FLayoutBlockTextContext get_text_context() const override;

		virtual std::shared_ptr<IRunRenderer> get_renderer() const;

		virtual glm::vec2 get_location_offset() const override;

	private:
		std::shared_ptr<IRun> m_run;

		FTextRange m_range;

		glm::vec2 m_size;

		glm::vec2 m_location_offset;

		//todo:add FLayoutBlockTextContext
		FLayoutBlockTextContext m_text_context;

		std::shared_ptr<IRunRenderer> m_renderer;
	};
}