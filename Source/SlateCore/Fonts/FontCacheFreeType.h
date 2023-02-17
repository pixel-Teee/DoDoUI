#pragma once

#include "CompositeFont.h"//FFontFaceDataConstPtr
#include "Core/String/DoDoString.h"//DoDoUtf8String
#include <set>
#include "freetype/freetype.h"

namespace DoDo
{
	/*
	 * wrapped around a free type library instance
	 * this instance will be create using our memory allocator
	 */
	class FFreeTypeLibrary
	{
	public:
		FFreeTypeLibrary();
		~FFreeTypeLibrary();

	private:
		//non-copyable
		FFreeTypeLibrary(const FFreeTypeLibrary&);
		FFreeTypeLibrary& operator=(const FFreeTypeLibrary&);

		FT_Library m_ftl_library;
		//FT_Memory m_custom_memory;
	};
	/*
	 * wrapped around a free type face instance
	 * it will either steal the given buffer, or stream the given file from disk
	 */
	class FFreeTypeFace
	{
	public:

	private:
		//non-copyable
		FFreeTypeFace(const FFreeTypeFace&);
		FFreeTypeFace& operator=(const FFreeTypeFace&);

		FT_Face m_ft_face;//free type face

		FFontFaceDataConstPtr m_memory;//just a uint8_t memory block

		std::set<DoDoUtf8String>  m_attributes;

		EFontLayoutMethod m_layout_method;
	};
}