#pragma once

namespace DoDo {
	/*
		represents a 2d transformation in the following order
		scale then translate used by FGeometry for it's layout transformations

		Matrix form looks like:
		[Vx Vy 1] * [ S  0  0 ]
					[ 0  S  0 ]
					[ Tx Ty 1 ]
	*/
	class FSlateLayoutTransform
	{
	public:

		explicit FSlateLayoutTransform(float in_scale = 1.0f);

	private:
		float m_scale;//scale
	};
}