#pragma once

#include "ITextLayoutMarshaller.h"

namespace DoDo {
	/*
	* base class implementing some common functionality for all text marshallers
	*/
	class FBaseTextLayoutMarshaller : public ITextLayoutMarshaller
	{
	public:
		virtual ~FBaseTextLayoutMarshaller() {}

		virtual void make_dirty() override
		{
			m_b_is_dirty = true;
		}

		virtual void clear_dirty() override
		{
			m_b_is_dirty = false;
		}

		virtual bool is_dirty() const override
		{
			return m_b_is_dirty;
		}
	protected:
		FBaseTextLayoutMarshaller()
			: m_b_is_dirty(false)
		{}
	private:
		bool m_b_is_dirty;
	};
}