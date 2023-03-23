#pragma once

#include "Core/Core.h"

#include "Core/Misc/CoreMiscDefines.h"//EForceInit

namespace DoDo
{
	/*enum for the different kinds of gamma spaces we expect to need to covert from/to*/
	enum class EGammaSpace : uint8_t
	{
		/*no gamma correction is applied to this space, the incoming colors area assumed to already be in linear space*/
		Linear,
		/*a simplified sRGB gamma correction is applied, pow(1/2.2)*/
		Pow22,
		/*use the standard sRGB conversion*/
		sRGB
	};

	struct FColor;

	/*
	 * a linear, 32-bit/component floating RGBA color
	 */
	struct FLinearColor
	{
		float R, G, B, A;

		/*static lookup table used for FColor -> FLinearColor conversion Pow(2.2)*/
		static float pow_22_one_over_255_table[256];

		/*static lookup table used for FColor -> FLinearColor conversion sRGB*/
		static float srgb_to_linear_table[256];

		FLinearColor() {}

		explicit FLinearColor(EForceInit)
			: R(0), G(0), B(0), A(0)
		{}

		constexpr FLinearColor(float in_r, float in_g, float in_b, float in_a = 1.0f) :
		R(in_r), G(in_g), B(in_b), A(in_a) {}

		/*
		 * converts an FColor which is assumed to be in sRGB space, into linear color space
		 * @param Color the sRGB color that needs to be converted into linear space
		 * to get direct conversion use ReinterpretAsLinear
		 */
		FLinearColor(const FColor& color);
	};

	//FColor
	//stores a color with 8 bits of precision per channel
	//note: linear color values should always be converted to gamma space before stored in an FColor,
	//as 8 bits of precision is not enough to store linear space colors!
	//this can be done with FLinearColor::ToFColor(true)

	//note:linear color is float stores, FColor use integer stores

	struct FColor
	{
	public:
		uint8_t R, G, B, A;//r, g, b, a

		//constructors
		FColor(){}

		explicit FColor(EForceInit)
		{
			R = G = B = A = 0;
		}

		constexpr FColor(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint8_t in_a = 255) // 255
			: R(in_r), G(in_g), B(in_b), A(in_a)
		{}

		/*some pre-initialized colors, useful for debug code*/
		static const FColor White;
		static const FColor Black;
		static const FColor Transparent;
		static const FColor Red;
		static const FColor Green;
		static const FColor Blue;
		static const FColor Yellow;
		static const FColor Cyan;//qing se
		static const FColor Magenta;//pin hong
		static const FColor Orange;
		static const FColor Purple;//zi se
		static const FColor Turquoise;//lv song shi, qing se
		static const FColor Silver;//yin se
		static const FColor Emerald;//fei cui
	};
}