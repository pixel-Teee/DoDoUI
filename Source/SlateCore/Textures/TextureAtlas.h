#pragma once

#include "Core/Containers/List.h"

namespace DoDo
{
	class FSlateShaderResource;
	/*
	 * specifies how to handle texture atlas padding (when specified for the atlas)
	 * we only support one pixel of padding because we don't support mips or aniso filtering on atlas textures right noew
	 */
	enum ESlateTextureAtlasPaddingStyle
	{
		/*don't pad the atlas*/
		NoPadding,
		/*dilate the texture by one pixel to pad the atlas*/
		DilateBorder,
		/*one pixel uniform padding border filled with zeros*/
		PadWithZero
	};

	/*
	 * structure holding information about where a texture is located in the atlas, inherits a linked-list interface
	 *
	 * when a slot is occupied by texture data, the remaining space in the slot (if big enough) is split off into two new (smaller) slots,
	 * building a tree of texture rectangles which, instead of being stored as a tree, are flattened into two linked-lists:
	 * - AtlasEmptySlots : a linked-list of empty slots ready for texture data - iterates in same order as a depth-first-search on a tree
	 * - AtlasUsedSlots : an unordered linked-list of slots containing texture data
	 */
	struct FAtlasedTextureSlot : public TIntrusiveLinkedList<FAtlasedTextureSlot>
	{
		/*the x position of the character in the texture*/
		uint32_t m_x;

		/*the y position of the character in the texture*/
		uint32_t m_y;

		/*the width of the character*/
		uint32_t m_width;

		/*the height of the character*/
		uint32_t m_height;

		/*uniform padding, can only be zero or one, see ESlateTextureAtlasPaddingStyle*/
		uint8_t m_padding;

		FAtlasedTextureSlot(uint32_t in_x, uint32_t in_y, uint32_t in_width, uint32_t in_height, uint8_t in_padding)
			: m_x(in_x)
			, m_y(in_y)
			, m_width(in_width)
			, m_height(in_height)
			, m_padding(in_padding)
		{}
	};

	/*
	 * base class texture atlases in slate
	 */
	class FSlateTextureAtlas
	{
	public:
		FSlateTextureAtlas(uint32_t in_width, uint32_t in_height, uint32_t in_bytes_per_pixel)
			: m_atlas_data()
			, m_atlas_used_slots(nullptr)
			, m_atlas_empty_slots(nullptr)
			, m_atlas_width(in_width)
			, m_atlas_height(in_height)
			, m_bytes_per_pixel(in_bytes_per_pixel)
			, m_b_needs_update(false)
		{
			init_atlas_data();
		}

		virtual ~FSlateTextureAtlas();

		/*
		 * clears atlas cpu data, it does not clear rendering data
		 */
		void empty_atlas_data();

		/*
		 * creates enough space for a single texture the width and height of the atlas
		 */
		void init_atlas_data();

		/*
		 * adds a texture to the atlas
		 *
		 * @param TextureWidth width of the texture
		 * @param TextureHeight height of the texture
		 * @param Data Raw texture data
		 */
		const FAtlasedTextureSlot* add_texture(uint32_t texture_width, uint32_t texture_height, const std::vector<uint8_t>& data);

		/*marks the texture as dirty and needing its rendering resources updated*/
		void mark_texture_dirty();

		/*
		 * finds the optimal slot for a texture in the atlas
		 *
		 * @param width the width of the texture we are adding
		 * @param height the height of the texture we are adding
		 */
		const FAtlasedTextureSlot* find_slot_for_texture(uint32_t in_width, uint32_t in_height);

		struct FCopyRawData//this is a temp texture to copy one raw data
		{
			/*source data to copy*/
			const uint8_t* src_data;
			/*place to copy data to*/
			uint8_t* dest_data;
			/*the row number to copy*/
			uint32_t src_row;
			/*the row number to copy to*/
			uint32_t dest_row;
			/*the width of a source row*/
			uint32_t row_width;
			/*the width of the source texture*/
			uint32_t src_texture_width;
			/*the width of the dest texture*/
			uint32_t dest_texture_width;
		};

		/*
		 * copies a single row from a source texture to a dest texture
		 * respecting the padding
		 *
		 * @param CopyRawData information for how to copy a row
		 */
		void copy_row(const FCopyRawData& copy_row_data);

		/*
		 * zeros out a row in the dest texture (used with PaddingStyle == PadWithZero)
		 * respecting the padding
		 *
		 * @param CopyRowData information for how to copy a row
		 */
		void zero_row(const FCopyRawData& copy_row_data);

		/*
		 * updates the texture used for rendering if needed
		 */
		virtual void conditional_update_texture() = 0;

		/*
		* copies texture data into the atlas at a given slot
		* 
		* @param SlotToCopyTo the occupied slot in the atlas where texture data should be copied to
		* @param Data the data to copy into the atlas
		*/
		void copy_data_into_slot(const FAtlasedTextureSlot* slot_to_copy_to, const std::vector<uint8_t>& data);
	private:
		/*returns the amount of padding needed for the current padding style*/
		uint8_t get_padding_amount() const
		{
			return (m_padding_style == ESlateTextureAtlasPaddingStyle::NoPadding) ? 0 : 1;
		}
	protected:
		/*actual texture data contained in the atlas*/
		std::vector<uint8_t> m_atlas_data;
		/*the list of atlas slots pointing to used texture data in the atlas*/
		FAtlasedTextureSlot* m_atlas_used_slots;
		/*the list of atlas slots pointing to empty texture data in the atlas*/
		FAtlasedTextureSlot* m_atlas_empty_slots;
		/*width of the atlas*/
		uint32_t m_atlas_width;
		/*height of the atlas*/
		uint32_t m_atlas_height;
		/*bytes per pixel in the atlas*/
		uint32_t m_bytes_per_pixel;

		/**/
		//todo:add padding style
		ESlateTextureAtlasPaddingStyle m_padding_style;

		/*true if this texture needs to have its rendering resources updated*/
		bool m_b_needs_update;
	};

	/*a factory capable of generating a texture atlas or shader resource for textures too big to be in an atlas*/
	class ISlateTextureAtlasFactory
	{
	public:
		virtual ~ISlateTextureAtlasFactory() {}

		virtual std::unique_ptr<FSlateTextureAtlas> create_texture_atlas(int32_t atlas_size, int32_t atlas_stride, ESlateTextureAtlasPaddingStyle padding_style, bool b_updates_after_initialization) const = 0;

		virtual std::unique_ptr<FSlateTextureAtlas> create_non_atlased_texture(const uint32_t in_width, const uint32_t in_height, const std::vector<uint8_t>& in_raw_data) const = 0;

		virtual void release_texture_atlases(const std::vector<std::unique_ptr<FSlateTextureAtlas>>& in_texture_atlases, const std::vector<std::unique_ptr<FSlateShaderResource>>& in_non_atlased_textures, const bool b_wait_for_release) const = 0;
	};

	/*interface to allow the slate atlas visualizer to query atlas page information for an atlas provider*/
	class ISlateAtlasProvider
	{
	public:
		/*virtual destructor*/
		virtual ~ISlateAtlasProvider(){}

		/*get the number of atlas pages this atlas provider has available when calling get atlas page resource*/
		virtual int32_t get_num_atlas_pages() const = 0;

		/*get the page resource for the given index (verify with get num atlas pages)*/
		virtual class FSlateShaderResource* get_atlas_page_resource(const int32_t in_index) = 0;

		/*does the page resources for the given index only contain alpha information? this affects how the atlas visualizer will sample them (verify with get num atlas pages)*/
		virtual bool is_atlas_page_resource_alpha_only(const int32_t in_index) const = 0;
	};
}