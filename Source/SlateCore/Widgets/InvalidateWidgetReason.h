#pragma once

#include "Core/Core.h"

#include "Core/Misc/EnumClassFlags.h"

namespace DoDo {
	/*
		the different types of invalidation that are possible for a widget
	*/
	//uint8_t, enum value type is uint8_t
	enum class EInvalidateWidgetReason : uint8_t
	{
		None = 0,

		//use layout invalidation if your widget needs to change desired size
		//this is an expensive invalidation so do not use if all you need to do is redraw a widget
		Layout = 1 << 0,

		//use when the painting of widget has been altered, but nothing affecting sizing
		Paint = 1 << 1,

		//use if just the volatility of the widget has been adjusted
		Volatility = 1 << 2,

		//a child was added or removed(this implies prepass and layout)
		Child_Order = 1 << 3,

		//a widgets render transform changed
		Render_Transform = 1 << 4,

		//changing visibility(this implies layout)
		Visibility = 1 << 5,

		//attributes got bound or unbound(it's used by the SlateAttributeMetaData)
		Attribute_Registration = 1 << 6,

		//re-cache desized size of all of this widget's children recursively(this implies layout)
		Prepass = 1 << 7,

		/*
			use paint invalidation if you're changing a normal property involving painting or sizing
			additionally if the property that was changed affects volatility in anyway, it's important
			that you invalidate volatility so that it can be recalculated and cached
		*/
		Paint_And_Volatility = Paint | Volatility,

		/*
			use layout invalidation if you're changing a normal property involving painting or sizing
			additionally if the property that was changed affects volatility in anyway, it's important
			that you invalidate volatility so that it can be recalculated and cached
		*/
		Layout_And_Volatility = Layout | Volatility
	};

	ENUM_CLASS_FLAGS(EInvalidateWidgetReason)
}

