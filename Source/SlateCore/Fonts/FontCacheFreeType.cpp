#include "FontCacheFreeType.h"

#include <PreCompileHeader.h>

#include "FontCacheFreeType.h"

#include "freetype/ftmodapi.h"

namespace DoDo
{
	FFreeTypeLibrary::FFreeTypeLibrary()
	{
		FT_Error error = FT_Init_FreeType(&m_ft_library);

		if(error)
		{
			std::cout << "could not init FreeType, error code: " << error << std::endl;
		}

		FT_Add_Default_Modules(m_ft_library);
	}

	FFreeTypeLibrary::~FFreeTypeLibrary()
	{
	}

	FFreeTypeFace::FFreeTypeFace(const FFreeTypeLibrary* in_ft_library, const DoDoUtf8String& in_file_name, const int32_t in_face_index)
		: m_ft_face(nullptr)
		, m_ft_stream_handler(in_file_name)
	{
		//load face
		std::memset(&m_ft_stream, 0, sizeof(m_ft_stream));

		m_ft_stream.size = (uint32_t)m_ft_stream_handler.m_font_size_bytes;
		m_ft_stream.descriptor.pointer = &m_ft_stream_handler;
		m_ft_stream.close = &FFTStreamHandler::close_file;
		m_ft_stream.read = &FFTStreamHandler::read_data;

		std::memset(&m_ft_face_open_args, 0, sizeof(m_ft_face_open_args));
		m_ft_face_open_args.flags = FT_OPEN_STREAM;
		m_ft_face_open_args.stream = &m_ft_stream;

		FT_Error error = FT_Open_Face(in_ft_library->get_library(), &m_ft_face_open_args, in_face_index, &m_ft_face);

		if(error)
		{
			m_ft_face = nullptr;
		}
	}

	FFreeTypeFace::~FFreeTypeFace()
	{
		if(m_ft_face)
		{
			FT_Done_Face(m_ft_face);
		}
	}

	FFreeTypeFace::FFTStreamHandler::FFTStreamHandler()
	{
	}

	FFreeTypeFace::FFTStreamHandler::FFTStreamHandler(const DoDoUtf8String& in_file_name)
	{
		m_file_handle.open(in_file_name.c_str());//open file

		m_file_handle.seekg(0, std::ios::end);//move handle

		m_font_size_bytes = m_file_handle.tellg();//get file size

		m_file_handle.seekg(0, std::ios::beg);//reset file pointer handle
	}

	void FFreeTypeFace::FFTStreamHandler::close_file(FT_Stream in_stream)
	{
		FFTStreamHandler* my_stream = (FFTStreamHandler*)in_stream->descriptor.pointer;//cast to specific pointer

		if(my_stream->m_file_handle)
		{
			my_stream->m_file_handle.close();
		}
	}

	unsigned long FFreeTypeFace::FFTStreamHandler::read_data(FT_Stream in_stream, unsigned long in_offset,
		unsigned char* in_buffer, unsigned long in_count)
	{
		FFTStreamHandler* my_stream = (FFTStreamHandler*)in_stream->descriptor.pointer;

		//if(my_stream->m_file_handle)
		//{
		//	
		//}

		//todo:check in_offset over boundary

		if(in_count)
		{
			if(my_stream->m_file_handle)
			{
				if(!my_stream->m_file_handle.read((char*)in_buffer, in_count))
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}

		return in_count;
	}

	namespace FreeTypeUtils
	{
		FT_Error load_glyph(FT_Face in_face, const uint32_t in_glyph_index, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale)
		{
			//todo:implement apply size and scale
			return FT_Load_Glyph(in_face, in_glyph_index, in_load_flags);
		}
	}
	
}


