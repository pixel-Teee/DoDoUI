#include <PreCompileHeader.h>

#include "TextureAtlas.h"

namespace DoDo
{
	FSlateTextureAtlas::~FSlateTextureAtlas()
	{
		empty_atlas_data();
	}

	void FSlateTextureAtlas::empty_atlas_data()
	{
		//remove all nodes
		std::vector<FAtlasedTextureSlot*> delete_slots;

		for(FAtlasedTextureSlot::TIterator slot_it(m_atlas_used_slots); slot_it; slot_it.next())
		{
			FAtlasedTextureSlot& cur_slot = *slot_it;
			delete_slots.push_back(&cur_slot);
		}

		for(FAtlasedTextureSlot::TIterator slot_it(m_atlas_empty_slots); slot_it; slot_it.next())
		{
			FAtlasedTextureSlot& cur_slot = *slot_it;
			delete_slots.push_back(&cur_slot);
		}

		m_atlas_used_slots = nullptr;
		m_atlas_empty_slots = nullptr;

		for(FAtlasedTextureSlot* cur_slot : delete_slots)
		{
			delete cur_slot;
		}

		delete_slots.clear();
	}

	void FSlateTextureAtlas::init_atlas_data()
	{
		FAtlasedTextureSlot* root_slot = new FAtlasedTextureSlot(0, 0, m_atlas_width, m_atlas_height, 0);//no padding

		root_slot->link_head(m_atlas_empty_slots);//parameter is link head

		m_atlas_data.resize(m_atlas_width * m_atlas_height * m_bytes_per_pixel, 0);
	}

	const FAtlasedTextureSlot* FSlateTextureAtlas::add_texture(uint32_t texture_width, uint32_t texture_height,
		const std::vector<uint8_t>& data)
	{
		//find a spot for the character in the texture
		const FAtlasedTextureSlot* new_slot = find_slot_for_texture(texture_width, texture_height);
	}

	const FAtlasedTextureSlot* FSlateTextureAtlas::find_slot_for_texture(uint32_t in_width, uint32_t in_height)
	{
		FAtlasedTextureSlot* return_val = nullptr;

		//account for padding on both sides
		const uint8_t padding = 0;
		const uint32_t total_padding = padding * 2;
		const uint32_t padded_width = in_width + total_padding;
		const uint32_t padded_height = in_height + total_padding;

		//previously, slots were stored as a binary tree - this has been replaced with a linked-list of slots on the edge of the tree
		//(slots on the edge of the tree represents empty slots), this iterates empty slots in same order as a binary depth-first-search
		//except must faster
		for(FAtlasedTextureSlot::TIterator slot_it(m_atlas_empty_slots); slot_it; ++slot_it)
		{
			FAtlasedTextureSlot& cur_slot = *slot_it;

			if(padded_width <= cur_slot.m_width && padded_height <= cur_slot.m_height)
			{
				return_val = &cur_slot;
				break;
			}
		}

		if(return_val != nullptr)
		{
			//the width and height of the new child one
			const uint32_t remaining_width = std::max(0u, return_val->m_width - padded_width);
			const uint32_t remaining_height = std::max(0u, return_val->m_height - padded_height);


		}

		return return_val;
	}
}
