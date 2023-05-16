#pragma once

#include "unicode/unistr.h"//icu::UnicodeStrin depends on it

//class icu::UnicodeString;

namespace DoDo {
	class DoDoUtf8String;
	namespace ICUUtilities
	{
		/*
		* implementation of the string converter that can copy DoDoUtf8String to icu::UnicodeString via an ICU Converter since the native string format for this platform is not utf-16 (as used by icu)
		* note: do not use this type directly! use the FStringConverterImpl typedef, which will be set correctly for your platform
		*/
		class FStringConverterImpl_ConvertToUnicodeString
		{
		public:
			FStringConverterImpl_ConvertToUnicodeString();

			~FStringConverterImpl_ConvertToUnicodeString();

			//void convert_string()
			void convert_string(const DoDoUtf8String& source, const int32_t source_start_index, const int32_t source_len, icu::UnicodeString& destination, const bool should_null_terminate);
		private:
			UConverter* m_icu_converter;
		};

		/*work out the best string converter to use based upon our native platform string traits*/
		template<bool is_unicode, size_t TCHARSize>
		struct FStringConverterImpl_PlatformSpecific
		{
			typedef FStringConverterImpl_ConvertToUnicodeString Type;
		};
		template<>
		struct FStringConverterImpl_PlatformSpecific<true, 2> //a unicode encoding with a wchar_t size of 2 bytes is assumed to be UTF-16
		{
			//typedef FStringConverterImpl_NativeUTF16 Type;
		};

		typedef FStringConverterImpl_PlatformSpecific<true, 3>::Type FStringConverterImpl;
		/*
		* an object that can convert between DoDoUtf8String and icu::UnicodeString
		* note:this object is not thread-safe
		*/
		class FStringConverter
		{
		public:
			FStringConverter() = default;
			FStringConverter(const FStringConverter&) = delete;
			FStringConverter& operator=(const FStringConverter&) = delete;

			/*convert DoDoUtf8String -> icu::UniCodeString*/
			void convert_string(const DoDoUtf8String& source, const int32_t source_start_index, const int32_t source_len, icu::UnicodeString& destination, const bool should_null_terminate = true);

		private:
			FStringConverterImpl_ConvertToUnicodeString m_impl;
		};
	}
}