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
		FAtlasedTextureSlot* root_slot = new FAtlasedTextureSlot(0, 0, m_atlas_width, m_atlas_height, get_padding_amount());//padding

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
			copy_data_into_slot(new_slot, data);
			//todo:implement mark texture dirty
			mark_texture_dirty();
		}

		return new_slot;
	}

	void FSlateTextureAtlas::mark_texture_dirty()
	{
		m_b_needs_update = true;
	}

	const FAtlasedTextureSlot* FSlateTextureAtlas::find_slot_for_texture(uint32_t in_width, uint32_t in_height)
	{
		FAtlasedTextureSlot* return_val = nullptr;

		//account for padding on both sides
		const uint8_t padding = get_padding_amount();
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

	void FSlateTextureAtlas::copy_row(const FCopyRawData& copy_row_data)
	{
		const uint8_t* data = copy_row_data.src_data;
		uint8_t* start = copy_row_data.dest_data;

		const uint32_t source_width = copy_row_data.src_texture_width;
		const uint32_t dest_width = copy_row_data.dest_texture_width;
		const uint32_t src_row = copy_row_data.src_row;//the row number to copy
		const uint32_t dest_row = copy_row_data.dest_row;//the row number to copy to

		//this can only be one or zero
		const uint32_t padding = get_padding_amount();

		const uint8_t* source_data_addr = &data[(src_row * source_width) * m_bytes_per_pixel];
		uint8_t* dest_data_addr = &start[(dest_row * dest_width + padding) * m_bytes_per_pixel];
		std::memcpy(dest_data_addr, source_data_addr, source_width * m_bytes_per_pixel);//note:first to copy actual image data, then handle padding data

		if(padding > 0)
		{
			uint8_t* dest_padding_pixel_left = &start[(dest_row * dest_width) * m_bytes_per_pixel];
			uint8_t* dest_padding_pixel_right = dest_padding_pixel_left + ((copy_row_data.row_width - 1) * m_bytes_per_pixel);
			if(m_padding_style == ESlateTextureAtlasPaddingStyle::DilateBorder)//use border color to fill padding
			{
				const uint8_t* first_pixel = source_data_addr;
				const uint8_t* last_pixel = source_data_addr + ((source_width - 1) * m_bytes_per_pixel);
				std::memcpy(dest_padding_pixel_left, first_pixel, m_bytes_per_pixel);
				std::memcpy(dest_padding_pixel_right, last_pixel, m_bytes_per_pixel);
			}
			else//use zero to fill padding
			{
				std::memset(dest_padding_pixel_left, 0, m_bytes_per_pixel);
				std::memset(dest_padding_pixel_right, 0, m_bytes_per_pixel);
			}
		}
	}

	void FSlateTextureAtlas::zero_row(const FCopyRawData& copy_row_data)
	{
		const uint32_t source_width = copy_row_data.src_texture_width;
		const uint32_t dest_width = copy_row_data.dest_texture_width;
		const uint32_t dest_row = copy_row_data.dest_row;

		uint8_t* dest_data_addr = &copy_row_data.dest_data[dest_row * dest_width * m_bytes_per_pixel];
		std::memset(dest_data_addr, 0, copy_row_data.row_width * m_bytes_per_pixel);
	}

	void FSlateTextureAtlas::copy_data_into_slot(const FAtlasedTextureSlot* slot_to_copy_to, const std::vector<uint8_t>& data)
	{
		//------copy row by row------
		//copy pixel data to the texture
		uint8_t* start = &m_atlas_data[slot_to_copy_to->m_y * m_atlas_width * m_bytes_per_pixel + slot_to_copy_to->m_x * m_bytes_per_pixel];//one-dimension

		//account for same padding on each sides
		const uint32_t padding = get_padding_amount();
		const uint32_t all_padding = padding * 2;

		//make sure the actual slot is not zero-are
		//todo:check

		//the width of the source texture without padding (actual width)
		const uint32_t source_width = slot_to_copy_to->m_width - all_padding;
		const uint32_t source_height = slot_to_copy_to->m_height - all_padding;

		FCopyRawData copy_raw_data;
		copy_raw_data.dest_data = start;//place to copy data to
		copy_raw_data.src_data = data.data();//source data to copy
		copy_raw_data.dest_texture_width = m_atlas_width;//the width of dest texture
		copy_raw_data.src_texture_width = source_width;//the width of source texture
		copy_raw_data.row_width = slot_to_copy_to->m_width;//have padding + source_width

		//apply the padding for bilinear filtering
		//not used if no padding(assumes sampling outside boundaries of the sub texture is not possible)
		if (padding > 0)
		{
			//copy first color row into padding
			copy_raw_data.src_row = 0;
			copy_raw_data.dest_row = 0;//note:the number number(first row, second row, ...)

			if(m_padding_style == ESlateTextureAtlasPaddingStyle::DilateBorder)
			{
				copy_row(copy_raw_data);//copy one row
			}
			else
			{
				zero_row(copy_raw_data);
			}
		}

		//copy each row of the texture
		for(uint32_t row = padding; row < slot_to_copy_to->m_height - padding; ++row)
		{
			copy_raw_data.src_row = row - padding;
			copy_raw_data.dest_row = row;

			copy_row(copy_raw_data);
		}

		if(padding > 0)
		{
			//copy last color row into padding row for bilinear filtering
			copy_raw_data.src_row = source_height - 1;
			copy_raw_data.dest_row = slot_to_copy_to->m_height - padding;

			if(m_padding_style == ESlateTextureAtlasPaddingStyle::DilateBorder)
			{
				copy_row(copy_raw_data);
			}
			else
			{
				zero_row(copy_raw_data);
			}
		}
	}
	FSlateFlushableAtlasCache::FSlateFlushableAtlasCache(const FAtlasFlushParams* in_flush_params)
		: m_flush_params(in_flush_params)
	{
		//note:what is life time of FAtlasFlushParams?

		m_current_max_gray_scale_atlas_pages_before_flush_request = m_flush_params->m_initial_max_atlas_pages_before_flush_request;
		m_current_max_color_atlas_pages_before_flush_request = m_flush_params->m_initial_max_atlas_pages_before_flush_request;
		m_current_max_non_atlased_textures_before_flush_request = m_flush_params->m_initial_max_non_atlas_pages_before_flush_request;
	}
}
