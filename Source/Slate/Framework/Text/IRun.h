#pragma once

namespace DoDo {
	struct FRunInfo
	{
		FRunInfo()
			: m_name()
			, m_meta_data()
		{

		}

		FRunInfo(DoDoUtf8String in_name)
			: m_name(in_name)
			, m_meta_data()
		{

		}

		DoDoUtf8String m_name;

		std::map<DoDoUtf8String, DoDoUtf8String> m_meta_data;
	};
	class IRun
	{
	public:

		virtual ~IRun() {}


	};
}