#pragma once

#include "Core/Containers/List.h"

namespace DoDo
{
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

		/*
		 * finds the optimal slot for a texture in the atlas
		 *
		 * @param width the width of the texture we are adding
		 * @param height the height of the texture we are adding
		 */
		const FAtlasedTextureSlot* find_slot_for_texture(uint32_t in_width, uint32_t in_height);
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

		/*true if this texture needs to have its rendering resources updated*/
		bool m_b_needs_update;
	};

	class FSlateShaderResource;
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