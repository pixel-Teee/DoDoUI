#include <PreCompileHeader.h>

#include "SlateInvalidationRoot.h"

namespace DoDo
{
	FSlateInvalidationRoot::FSlateInvalidationRoot()
	{
	}

	FSlateInvalidationRoot::~FSlateInvalidationRoot()
	{
	}

	FSlateInvalidationResult FSlateInvalidationRoot::paint_invalidation_root(const FSlateInvalidationContext& context)
	{
		FSlateInvalidationResult result;

		m_cached_max_layer_id = paint_slow_path(context);

		result.m_max_layer_id_painted = m_cached_max_layer_id;//fill in the result

		return result;
	}
}
