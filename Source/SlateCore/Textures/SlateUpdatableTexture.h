#pragma once

#include <vector>

namespace DoDo
{
	/*
	 * an interface to deal with a slate texture that can be updated dynamically
	 */
	class FSlateUpdatableTexture
	{
	public:
		virtual ~FSlateUpdatableTexture() {}

		/*
		 * updates the texture contents via a byte array
		 * note: this method is not thread safe so make sure you do not use the bytes data on another after it is passed in
		 *
		 * @param Bytes array of texture data
		 */
		virtual void update_texture(const std::vector<uint8_t>& bytes) = 0;
	};
}