#pragma once

#include "Core/Misc/EnumClassFlags.h"//ENUM_CLASS_FLAGS

namespace DoDo
{
	class FSlateDrawElement;//forward declare
	/*
	 * effects that can be applied to elements when rendered
	 * note : new effects added should be in mask form
	 * if you add a type here you must also implement the proper shader type(TSlateElementPS), see slate shaders.h
	 */
	enum class ESlateDrawEffect
	{
		/*no effect applied*/
		None = 0,
		/*advanced : draw the element with no blending*/
		NoBlending = 1 << 0,
		/*advanced : blend using pre-multiplied alpha, ignored if no blending is set*/
		PreMultipliedAlpha = 1 << 1,
		/*advanced : no gamma correction should be done*/
		NoGamma = 1 << 2,
		/*advanced : change the alpha value to 1 - Alpha*/
		InvertAlpha = 1 << 3
	};

	ENUM_CLASS_FLAGS(ESlateDrawEffect)

	typedef std::vector<FSlateDrawElement> FSlateDrawElementArray;
}
