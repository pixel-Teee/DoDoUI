#include "FontCacheFreeType.h"

#include <PreCompileHeader.h>

#include "FontCacheFreeType.h"

#include "freetype/ftmodapi.h"

#include "freetype/ftadvanc.h"

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

		FT_Pos get_scaled_height(FT_Face in_face, const EFontLayoutMethod in_layout_method)
		{
			if (FT_IS_SCALABLE(in_face))
			{
				//scalable fonts use the unscaled value (and apply the scale manually), as the metrices have had rounding applied to them
				return FT_MulFix((in_layout_method == EFontLayoutMethod::Metrics) ? in_face->height : (in_face->bbox.yMax - in_face->bbox.yMin), in_face->size->metrics.y_scale);
			}
			else if (FT_HAS_FIXED_SIZES(in_face))
			{
				//fixed size fonts don't support scaling, but we calculated the scale to use for the glyph in ApplySizeAndScale
				return FT_MulFix(in_face->size->metrics.height, in_face->size->metrics.y_scale);
			}
			return 0;
		}
	}
	
	FFreeTypeAdvanceCache::FFreeTypeAdvanceCache(FT_Face in_face, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale)
		: m_face(in_face)
		, m_load_flags(in_load_flags)
		, m_font_size(in_font_size)
		, m_font_scale(in_font_scale)
		, m_advance_map()
	{
	}

	bool FFreeTypeAdvanceCache::find_or_cache(const uint32_t in_glyph_index, FT_Fixed& out_cached_advance)
	{
		//try and find the advance from the cache...
		{
			auto it = m_advance_map.find(in_glyph_index);

			if (it != m_advance_map.end())
			{
				out_cached_advance = it->second;

				return true;
			}
		}

		FreeTypeUtils::apply_size_and_scale(m_face, m_font_size, m_font_scale);

		//no cached data, go ahead and add an entry for it...
		const FT_Error error = FT_Get_Advance(m_face, in_glyph_index, m_load_flags, &out_cached_advance);

		if (error == 0)
		{
			m_advance_map.insert({ in_glyph_index, out_cached_advance });

			return true;
		}

		return false;
	}

	std::shared_ptr<FFreeTypeAdvanceCache> FFreeTypeCacheDirectory::get_advance_cache(FT_Face in_face, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale)
	{
		const FFontKey key(in_face, in_load_flags, in_font_size, in_font_scale);

		auto it = m_advance_cache_map.find(key);

		if (it == m_advance_cache_map.end())
		{
			auto res = m_advance_cache_map.insert({ key, std::make_shared<FFreeTypeAdvanceCache>(in_face, in_load_flags, in_font_size, in_font_scale) });
			return res.first->second;
		}

		return it->second;
	}

}


