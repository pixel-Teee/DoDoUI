#pragma once

#include <vector>
#include <memory>
#include <Core/String/DoDoString.h>

namespace DoDo
{
	enum class EFontHinting : uint8_t
	{
		Default,

		Auto,

		AutoLight,

		Monochrome,

		None
	};

	enum class EFontLoadingPolicy : uint8_t
	{
		LazyLoad,

		Stream,

		InLine
	};

	enum class EFontLayoutMethod : uint8_t
	{
		/*layout the font using the metrics data available in the font. this is typically the desired option, however some fonts have broken or incorrect metrics so may yield better results when using the bounding box values to layout the font*/
		Metrics,
		/*layout the font using the values from its bounding box, this typically a larger line height for fonts that have valid metrics, however it can also produce much better results for fonts that have broken or incorrect metrics*/
		BoundingBox
	};

	struct FFontFaceData;
	typedef std::shared_ptr<FFontFaceData> FFontFaceDataPtr;
	typedef std::shared_ptr<const FFontFaceData> FFontFaceDataConstPtr;
	/*
	 * raw font data for a font face asset
	 */
	struct FFontFaceData
	{
	public:
		/*default constructor*/
		FFontFaceData()
		{
		}

		/*construct from an existing block of font data*/
		FFontFaceData(std::vector<uint8_t>&& in_data)
			: m_data(std::move(in_data))
		{
			//todo:track memory usage
		}

		/*destructor*/
		~FFontFaceData()
		{
		}

		/*@return true if this instance has data set on it*/
		bool has_data() const
		{
			return m_data.size() > 0;
		}

	private:

		/*internal data*/
		std::vector<uint8_t> m_data;
	};

	/*payload data describing an individual font in a typeface, keep this lean as it's also used as a key!*/
	struct FFontData
	{
		/*default constructor*/
		FFontData();

		/*construct the raw data from a filename and the font data attributes*/
		FFontData(DoDoUtf8String in_font_file_name, const EFontHinting in_hinting, const EFontLoadingPolicy in_loading_policy, const int32_t in_sub_face_index = 0);
	};
}