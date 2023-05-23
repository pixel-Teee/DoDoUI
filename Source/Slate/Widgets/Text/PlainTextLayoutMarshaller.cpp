#include "PreCompileHeader.h"

#include "PlainTextLayoutMarshaller.h"

#include "SlateCore/Styling/SlateTypes.h"//FTextBlockStyle depends on it

#include "Slate/Framework/Text/SlateTextLayout.h"//FSlateTextLayout depends on it

#include "Slate/Framework/Text/SlateTextRun.h"//FSlateTextRun depends on it

namespace DoDo {
	std::shared_ptr<FPlainTextLayoutMarshaller> FPlainTextLayoutMarshaller::Create()
	{
		//return std::shared_ptr<FPlainTextLayoutMarshaller>();
		return std::make_shared<FPlainTextLayoutMarshaller>();
	}
	FPlainTextLayoutMarshaller::~FPlainTextLayoutMarshaller()
	{
	}
	void FPlainTextLayoutMarshaller::set_text(const DoDoUtf8String& source_string, FTextLayout& target_text_layout)
	{
		const FTextBlockStyle& default_text_style = static_cast<FSlateTextLayout&>(target_text_layout).get_default_text_style();

		std::vector<FTextRange> line_ranges;
		FTextRange::calculate_line_ranges_from_string(source_string, line_ranges);

		std::vector<FTextLayout::FNewLineData> lines_to_add;
		lines_to_add.reserve(line_ranges.size());

		for (int32_t line_index = 0; line_index < line_ranges.size(); ++line_index)
		{
			const FTextRange& line_range = line_ranges[line_index];

			std::shared_ptr<DoDoUtf8String> line_text = std::make_shared<DoDoUtf8String>(const_cast<DoDoUtf8String&>(source_string).sub_str(line_range.m_begin_index, line_range.len()));

			std::vector<std::shared_ptr<IRun>> runs;

			runs.push_back(FSlateTextRun::Create(FRunInfo(), line_text, default_text_style));

			lines_to_add.emplace_back(std::move(line_text), std::move(runs));
		}

		target_text_layout.add_lines(lines_to_add);
	}
	void FPlainTextLayoutMarshaller::get_text(const DoDoUtf8String& source_string, FTextLayout& source_text_layout)
	{
	}
}