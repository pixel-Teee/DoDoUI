#include "PreCompileHeader.h"

#include "ICUBreakIterator.h"

//#include "ICUInternationalization.h"

#include "Internationalization.h"

#include "ICUCulture.h"//EBreakIteratorType

namespace DoDo {
	FICUBreakIteratorManager* FICUBreakIteratorManager::m_singleton = nullptr;

	void FICUBreakIteratorManager::create()
	{
		m_singleton = new FICUBreakIteratorManager();
	}
	void FICUBreakIteratorManager::destroy()
	{
		delete m_singleton;
		m_singleton = nullptr;
	}
	bool FICUBreakIteratorManager::is_initialized()
	{
		return m_singleton != nullptr;
	}
	FICUBreakIteratorManager& FICUBreakIteratorManager::get()
	{
		return *m_singleton;
	}
	std::weak_ptr<icu::BreakIterator> FICUBreakIteratorManager::create_character_boundary_iterator()
	{
		//std::shared_ptr<icu::BreakIterator> iterator = std::make_shared<
		std::shared_ptr<icu::BreakIterator> iterator = std::shared_ptr<icu::BreakIterator>(FInternationalization::get().get_default_culture()->m_implementation->get_break_iterator(EBreakIteratorType::Grapheme)->clone());

		m_allocated_iterators.insert(iterator);

		return iterator;
	}
	std::weak_ptr<icu::BreakIterator> FICUBreakIteratorManager::create_word_break_iterator()
	{
		return std::weak_ptr<icu::BreakIterator>();
	}
	std::weak_ptr<icu::BreakIterator> FICUBreakIteratorManager::create_line_break_iterator()
	{
		return std::weak_ptr<icu::BreakIterator>();
	}
	void FICUBreakIteratorManager::destroy_iterator(std::weak_ptr<icu::BreakIterator>& in_iterator)
	{
	}

	FICUBreakIterator::FICUBreakIterator(std::weak_ptr<icu::BreakIterator> in_icu_break_iterator)
		: m_icu_break_iterator_handle(in_icu_break_iterator)
	{
	}
	FICUBreakIterator::~FICUBreakIterator()
	{
	}

	void FICUBreakIterator::set_string(DoDoUtf8String in_string)
	{
		//get_internal_break_iterator()->adoptText()
	}

	std::shared_ptr<icu::BreakIterator> FICUBreakIterator::get_internal_break_iterator() const
	{
		return m_icu_break_iterator_handle.lock();
	}

}