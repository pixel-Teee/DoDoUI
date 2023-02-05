#pragma once

#include <vector>

#include <memory>

namespace DoDo {
	/*
	* holds texture data for upload to a rendering resource
	*/
	struct FSlateTextureData
	{
		FSlateTextureData(uint32_t in_width = 0, uint32_t in_height = 0, uint32_t in_bytes_per_pixel = 0, const std::vector<uint8_t>& in_bytes = std::vector<uint8_t>())
			: m_bytes(in_bytes)
			, m_width(in_width)
			, m_height(in_height)
			, m_bytes_per_pixel(in_bytes_per_pixel)
		{
		}

		uint32_t get_width() const
		{
			return m_width;
		}

		uint32_t get_height() const
		{
			return m_height;
		}

		const std::vector<uint8_t>& get_raw_bytes() const
		{
			return m_bytes;
		}
	private:
		/*raw uncompressed texture data*/
		std::vector<uint8_t> m_bytes;

		/*width of the texture*/
		uint32_t m_width;

		/*height of the texture*/
		uint32_t m_height;

		/*the number of bytes of each pixel*/
		uint32_t m_bytes_per_pixel;
	};

	typedef std::shared_ptr<FSlateTextureData> FSlateTextureDataPtr;
}