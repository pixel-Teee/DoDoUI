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
		std::memset(&m_ft_stream, 0, sizeof(FT_StreamRec));

		m_ft_stream.size = (uint32_t)m_ft_stream_handler.m_font_size_bytes;
		m_ft_stream.descriptor.pointer = &m_ft_stream_handler;
		m_ft_stream.close = &FFTStreamHandler::close_file;
		m_ft_stream.read = &FFTStreamHandler::read_data;

		std::memset(&m_ft_face_open_args, 0, sizeof(FT_Open_Args));
		m_ft_face_open_args.flags = FT_OPEN_STREAM;
		m_ft_face_open_args.stream = &m_ft_stream;

		//FT_Error error = FT_Open_Face(in_ft_library->get_library(), &m_ft_face_open_args, in_face_index, &m_ft_face);

		FT_New_Face(in_ft_library->get_library(), in_file_name.c_str(), 0, &m_ft_face);//todo:fix me

		//if(error)
		//{
		//	m_ft_face = nullptr;
		//}
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

		my_stream->m_file_handle.close();
	}

	unsigned long FFreeTypeFace::FFTStreamHandler::read_data(FT_Stream in_stream, unsigned long in_offset,
		unsigned char* in_buffer, unsigned long in_count)
	{
		FFTStreamHandler* my_stream = (FFTStreamHandler*)in_stream->descriptor.pointer;

		my_stream->m_file_handle.seekg(in_offset, std::ios::beg);

		if (in_count > 0)
		{
			my_stream->m_file_handle.read((char*)in_buffer, in_count);
		}

		return in_count;
	}

	namespace FreeTypeUtils
	{
		void apply_size_and_scale(FT_Face in_face, const int32_t in_font_size, const float in_font_scale)
		{
			//covert to fixed scale for maximum precision
			const FT_F26Dot6 fixed_font_size = ConvertPixelTo26Dot26<FT_F26Dot6>(in_font_size);//to pixel size to dot size
			const FT_Long fixed_font_scale = ConvertPixelTo16Dot16<FT_Long>(in_font_scale);//convert pixel size 

			//convert the requested font size to a pixel size based on our render dpi and the requested scale
			FT_F26Dot6 required_fixed_font_pixel_size = 0;
			{
				required_fixed_font_pixel_size = (fixed_font_size * (FT_Pos)96 + 36) / 72;

				//scale the 26.6 pixel size by the desired 16.6 fractional scaling value
				required_fixed_font_pixel_size = FT_MulFix(required_fixed_font_pixel_size, fixed_font_scale);
			}

			//convert the 26.6 scaled pixel size back into pixel space
			const uint32_t required_font_pixel_size = Convert26Dot6ToRoundedPixel<uint32_t>(required_fixed_font_pixel_size);

			FT_Error error = FT_Set_Pixel_Sizes(in_face, required_font_pixel_size, required_font_pixel_size);

			assert(error == 0);


		}

		FT_Error load_glyph(FT_Face in_face, const uint32_t in_glyph_index, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale)
		{
			apply_size_and_scale(in_face, in_font_size, in_font_scale);
			return FT_Load_Glyph(in_face, in_glyph_index, in_load_flags);
		}
	}
	
}


