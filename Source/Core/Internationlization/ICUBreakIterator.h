#pragma once

#include "IBreakIterator.h"

#include <unicode/brkiter.h>//icu::BreakIterator depends on it

namespace DoDo {
	/*
	* managers the lifespan of icu break iterators
	*/
	class FICUBreakIteratorManager
	{
	public:
		static void create();
		static void destroy();
		static bool is_initialized();
		static FICUBreakIteratorManager& get();

		std::weak_ptr<icu::BreakIterator> create_character_boundary_iterator();
		std::weak_ptr<icu::BreakIterator> create_word_break_iterator();
		std::weak_ptr<icu::BreakIterator> create_line_break_iterator();
		void destroy_iterator(std::weak_ptr<icu::BreakIterator>& in_iterator);
	private:
		static FICUBreakIteratorManager* m_singleton;

		std::set<std::shared_ptr<icu::BreakIterator>> m_allocated_iterators;
	};
	/*
	* wraps an icu break iterator instance inside our own break iterator api
	*/
	class FICUBreakIterator : public IBreakIterator
	{
	public:
		FICUBreakIterator(std::weak_ptr<icu::BreakIterator>& in_icu_break_iterator);

		virtual ~FICUBreakIterator();
	private:
		std::weak_ptr<icu::BreakIterator> m_icu_break_iterator_handle;
	};
}