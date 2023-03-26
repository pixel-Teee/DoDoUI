#include "PreCompileHeader.h"

#include "Guid.h"

#ifdef WIN32
#include "Platform/Window/WindowsPlatformMisc.h"
#endif

#include "Core/String/DoDoString.h"

#include "Parse.h"

namespace DoDo {
	FGuid FGuid::new_guid()
	{
		FGuid result(0, 0, 0, 0);

		//todo:call platform function to generate guid
		FPlatformMisc::create_guid(result);

		return result;
	}
	bool FGuid::Parse(const DoDoUtf8String& guid_str, FGuid& out_guid)
	{
		if (guid_str.get_length() == 32)
		{
			return parse_exact(guid_str, EGuidFormats::Digits, out_guid);
		}

		return false;
	}
	bool FGuid::parse_exact(const DoDoUtf8String& guid_str, EGuidFormats format, FGuid& out_guid)
	{
		DoDoUtf8String normalized_guid_string;

		if (format == EGuidFormats::Digits)
		{
			normalized_guid_string = guid_str;
		}

		std::string str = normalized_guid_string.c_str();

		//todo:get sub str

		out_guid = FGuid(
			FParse::hex_number(str.substr(0, 8).c_str()),
			FParse::hex_number(str.substr(8, 8).c_str()),
			FParse::hex_number(str.substr(16, 8).c_str()),
			FParse::hex_number(str.substr(24, 8).c_str())
		);

		return true;
	}
}