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

		//handle cases like space, where the size of the glyph is zero, the copy data code doesn't handle zero sized source data with a padding
		//so make sure to skip this call
		if (new_slot && texture_width > 0 && texture_height > 0)
		{
			//todo:implement copy data into slot
			//todo:implement mark texture dirty
		}

		return new_slot;
	}

	const FAtlasedTextureSlot* FSlateTextureAtlas::find_slot_for_texture(uint32_t in_width, uint32_t in_height)
	{
		FAtlasedTextureSlot* return_val = nullptr;

		//account for padding on both sides
		const uint8_t padding = 1;
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

			/*
			* new slots must have a minimum width/height, to avoid excessive slots, i.e. excessive memory usage and iteration.
			* no glyphs seem to use slots this small, and cutting these slots out improves performance/memory-usage a fair bit
			*/
			const uint32_t min_slot_dim = 2;

			//split the remaining area around this slot into two children
			if (remaining_height >= min_slot_dim || remaining_width >= min_slot_dim)
			{
				FAtlasedTextureSlot* left_slot = nullptr;
				FAtlasedTextureSlot* right_slot = nullptr;

				if (remaining_width <= remaining_height)
				{
					//split vertically
					// ---------------
					// |	 | 	     |
					// |slot |		 |
					// |	 |		 |
					// |- - -| right |
					// |	 |		 |
					// |left |		 |
					// |	 |		 |
					// ---------------
					//position and width/height
					left_slot = new FAtlasedTextureSlot(return_val->m_x, return_val->m_y + padded_height, padded_width, remaining_height, padding);
					right_slot = new FAtlasedTextureSlot(return_val->m_x + padded_width, return_val->m_y, remaining_width, return_val->m_height, padding);
				}
				else
				{
					//split horizontally
					// ----------------
					// |      |       |
					// |  Slot|	 Left |
					// |	  |	      |
					// |- - - - - - - |
					// |	 		  |
					// |	 Right	  |
					// |	 		  |
					// ----------------
					left_slot = new FAtlasedTextureSlot(return_val->m_x + padded_width, return_val->m_y, remaining_width, padded_height, padding);
					right_slot = new FAtlasedTextureSlot(return_val->m_x, return_val->m_y + padded_height, return_val->m_width, remaining_height, padding);
				}

				//replace the old slot within atlas empty slots, with the new left and right slot, then add the old slot to atlas used slots
				left_slot->link_replace(return_val);
				right_slot->link_after(left_slot);

				return_val->link_head(m_atlas_used_slots);//head insertion method
			}
			else
			{
				//remove the old slot from atlas empty slots, into atlas used slots
				return_val->un_link();
				return_val->link_head(m_atlas_used_slots);
			}

			//shrink the slot to the remaining area
			return_val->m_width = padded_width;
			return_val->m_height = padded_height;
		}

		return return_val;
	}
	void FSlateTextureAtlas::copy_data_into_slot(const FAtlasedTextureSlot* slot_to_copy_to, const std::vector<uint8_t>& data)
	{
		//copy pixel data to the texture
		uint8_t* start = &m_atlas_data[slot_to_copy_to->m_y * m_atlas_width * m_bytes_per_pixel + slot_to_copy_to->m_x * m_bytes_per_pixel];//one-dimension

		//account for same padding on each sides
		const uint32_t padding = 1;
		const uint32_t all_padding = padding * 2;

		//make sure the actual slot is not zero-are
		//todo:check

		//the width of the source texture without padding (actual width)
		const uint32_t source_width = slot_to_copy_to->m_width - all_padding;
		const uint32_t source_height = slot_to_copy_to->m_height - all_padding;

		FCopyRawData copy_raw_data;
		copy_raw_data.dest_data = start;
		copy_raw_data.src_data = data.data();
		copy_raw_data.dest_texture_width = m_atlas_width;
		copy_raw_data.src_texture_width = source_width;
		copy_raw_data.row_width = slot_to_copy_to->m_width;

		//apply the padding for bilinear filtering
		//not used if no padding(assumes sampling outside boundaries of the sub texture is not possible)
		if (padding > 0)
		{
			//copy first color row into padding
			copy_raw_data.src_raw = 0;
			copy_raw_data.dest_raw = 0;


		}
	}
}
