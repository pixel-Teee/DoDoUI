#pragma once

#include <vector>
#include <memory>
#include <Core/String/DoDoString.h>

#include "Core/Templates/TypeHash.h"//HashCombine

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

		/*get the filename of the font to use, this may not actually exist on disk in editor builds*/
		const DoDoUtf8String& get_font_file_name() const;

		/*get the index of the sub-face that should be used*/
		int32_t get_sub_face_index() const;

		friend inline uint32_t Get_Type_Hash(const FFontData& key)
		{
			uint32_t key_hash = 0;

			key_hash = Hash_Combine(key_hash, key.m_font_file_name_hash);

			return key_hash;
		}

		bool operator==(const FFontData& other) const;

	private:
		/*
		 * the filename of the front to use
		 * this variable is ignored if we have a font face asset, and is set to the .ufont file in a cooked build
		 */
		DoDoUtf8String m_font_file_name;

		/*
		 * cached hash value of font file name
		 * must be updated everytime font file name changes
		 */
		uint32_t m_font_file_name_hash;

		/*
		 * the index of the sub-face that should be used
		 * this is typically zero unless using a ttc/otc font
		 */
		int32_t m_sub_face_index;
	};

	struct FTypefaceEntry
	{
		FTypefaceEntry()
		{}

		/*construct the entry from a name*/
		FTypefaceEntry(const DoDoUtf8String& in_font_name)
			: m_name(in_font_name)
		{}

		/*construct the entry from a filename and font data attributes*/
		FTypefaceEntry(const DoDoUtf8String& in_font_name, DoDoUtf8String in_font_file_name, const EFontHinting in_hinting, const EFontLoadingPolicy in_loading_policy)
			: m_name(in_font_name)
			, m_font(std::move(in_font_file_name), in_hinting, in_loading_policy)
		{}

		/*named used to identify this font within its typeface*/
		DoDoUtf8String m_name;

		/*raw font data for this font*/
		FFontData m_font;
	};

	/*definition for a typeface (a family of fonts)*/
	struct FTypeface
	{
		/*default constructor*/
		FTypeface()
		{
		}

		/*convenience constructor for when your font family only contains a single font*/
		FTypeface(const DoDoUtf8String& in_font_name, DoDoUtf8String in_font_file_name, const EFontHinting in_hinting, const EFontLoadingPolicy in_loading_policy)
		{

		}

		/*append a new font into this family*/
		FTypeface& append_font(const DoDoUtf8String& in_font_name, DoDoUtf8String in_font_file_name, const EFontHinting in_hinting, const EFontLoadingPolicy in_loading_policy)
		{
			m_fonts.emplace_back(in_font_name, in_font_file_name, in_hinting, in_loading_policy);
			return *this;
		}

		/*the fonts contained within this family*/
		std::vector<FTypefaceEntry> m_fonts;
	};

	struct FCompositeFont
	{
		/*default constructor*/
		FCompositeFont()
		{

		}

		FCompositeFont(const DoDoUtf8String& in_font_name, DoDoUtf8String in_font_file_name, const EFontHinting in_hinting, const EFontLoadingPolicy in_loading_policy)
			: m_default_type_face(in_font_name, std::move(in_font_file_name), in_hinting, in_loading_policy)
		{}

		/*the default typeface that will be used when not overridden by a sub-typeface*/
		FTypeface m_default_type_face;
	};

	//todo:implement FGCObject
	struct FStandaloneCompositeFont : public FCompositeFont
	{
		FStandaloneCompositeFont() {}

		FStandaloneCompositeFont(const DoDoUtf8String& in_font_name, DoDoUtf8String in_font_file_name, const EFontHinting in_hinting, const EFontLoadingPolicy
			in_loading_policy)
			: FCompositeFont(in_font_name, std::move(in_font_file_name), in_hinting, in_loading_policy)
		{}
	};
}


