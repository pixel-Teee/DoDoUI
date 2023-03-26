#pragma once

#include "Core/Core.h"

namespace DoDo {
	enum class EGuidFormats
	{
		Digits,

		DigitsWithHyphens,

		DigitsWithHyphensLower,

		DigitsWithHyphensInBraces,

		DigitsWithHyphensInParentheses,

		HexValuesInBraces,

		UniqueObjectGuid,

		Short,

		Base36Encoded
	};
	/*
	* implements a globally unique identifier
	*/
	class DoDoUtf8String;
	struct FGuid
	{
	public:
		/*default constructor*/
		FGuid()
			: A(0)
			, B(0)
			, C(0)
			, D(0)
		{}

		/*
		* creates and initializes a new GUID from the specified components
		* 
		* @param InA the first component
		* @param InB the second component
		* @param InC the third component
		* @param InD the fourth component
		*/
		explicit FGuid(uint32_t in_a, uint32_t in_b, uint32_t in_c, uint32_t in_d)
			: A(in_a), B(in_b), C(in_c), D(in_d)
		{}

		/*
		* compares two guids for equality
		* 
		* @param X the first GUID to compare
		* @param Y the second GUID to compare
		* 
		* @return true if the GUIDs are equal, false otherwise
		*/
		friend bool operator==(const FGuid& x, const FGuid& y)
		{
			return ((x.A ^ y.A) | (x.B ^ y.B) | (x.C ^ y.C) | (x.D ^ y.D)) == 0;
		}

		friend bool operator!=(const FGuid& x, const FGuid& y)
		{
			return ((x.A ^ y.A) | (x.B ^ y.B) | (x.C ^ y.C) | (x.D ^ y.D)) != 0;
		}

		/*
		* returns a new GUID
		* 
		* @return a new GUID
		*/
		static FGuid new_guid();

		/*
		* converts a string to a GUID
		* 
		* @param GuidString the string to covert
		* @param OutGuid will contain the parsed GUID
		* @return true if the string was converted successfully, false otherwise
		* @see ParseExact, to string
		*/
		static bool Parse(const DoDoUtf8String& guid_str, FGuid& out_guid);

		static bool parse_exact(const DoDoUtf8String& guid_str, EGuidFormats format, FGuid& out_guid);

		/*holds the first component*/
		uint32_t A;

		/*holds the second component*/
		uint32_t B;

		/*holds the third component*/
		uint32_t C;

		/*holds the fourth component*/
		uint32_t D;
	};
}