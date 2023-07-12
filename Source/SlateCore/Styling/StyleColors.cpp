#include <PreCompileHeader.h>

#include "StyleColors.h"

#include <filesystem>

#include <string>

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include "Core/Misc/Paths.h"

namespace DoDo {
	USlateThemeManager USlateThemeManager::m_singleton;

	static const DoDoUtf8String theme_sub_dir = "slate/themes";

	void USlateThemeManager::load_themes()
	{
		m_themes.clear();

		//engine theme
		load_themes_from_directory(get_engine_theme_dir());
		//project theme

		//user specific theme

		ensure_valid_current_theme();

		apply_theme(m_current_theme_id);
	}

	USlateThemeManager::USlateThemeManager()
	{
		//todo:initialize default colors
	}
	DoDoUtf8String USlateThemeManager::get_engine_theme_dir() const
	{
		return FPaths::engine_content_dir() / theme_sub_dir;
	}
	void USlateThemeManager::ensure_valid_current_theme()
	{
		FStyleTheme default_dark_theme;

		default_dark_theme.m_display_name = "Dark";

		default_dark_theme.m_id = FGuid(0x13438026, 0x5FBB4A9C, 0xA00A1DC9, 0x770217B8);

		default_dark_theme.m_file_name = FPaths::engine_content_dir() / "Slate/Themes/Dark.json";

		m_themes.push_back(default_dark_theme);

		m_current_theme_id = default_dark_theme.m_id;
	}
	void USlateThemeManager::load_theme_colors(FStyleTheme& theme)
	{
		std::string theme_data;

		std::fstream fstr(theme.m_file_name.c_str());
		//load theme_data
		std::string strline;//one line
		while (!fstr.eof())
		{
			std::getline(fstr, strline);
			theme_data += strline;
		}
		fstr.close();

		rapidjson::Document document;
		if (!document.Parse(theme_data.c_str()).HasParseError())
		{
			if (document.HasMember("Colors"))
			{
				const rapidjson::Value& object = document["Colors"];

				theme.m_loaded_default_colors.resize((uint32_t)EStyleColor::max);
				//todo:get enum string
				for (int32_t color_index = 0; color_index < (int32_t)EStyleColor::max; ++color_index)
				{
					std::string enum_str = style_color_enum_str[color_index];

					if (object.HasMember(enum_str.c_str()))
					{
						theme.m_loaded_default_colors[color_index].init_from_string(object[enum_str.c_str()].GetString());
					}
				}
			}
		}
	}
	void USlateThemeManager::load_themes_from_directory(const DoDoUtf8String& directory)
	{
		std::vector<DoDoUtf8String> theme_files;

		//from directory to find json file to load to theme_files
		for (const auto& entry : std::filesystem::directory_iterator(directory.c_str()))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
			{
				theme_files.push_back(entry.path().string());
			}
		}

		for (const DoDoUtf8String& theme_file : theme_files)
		{
			std::string theme_data;

			std::fstream fstr(theme_file.c_str());

			//load theme_data
			std::string strline;//one line
			while (!fstr.eof())
			{
				std::getline(fstr, strline);
				theme_data += strline;
			}

			fstr.close();

			FStyleTheme theme;
			//use json library to parse this file
			read_theme(theme_data, theme);

			for (size_t i = 0; i < m_themes.size(); ++i)
			{
				if (m_themes[i].m_id == theme.m_id)
				{
					m_themes[i].m_file_name = theme.m_file_name;
				}
				else
				{
					//theme not found, add a new one
					theme.m_file_name = theme_file.c_str();
					m_themes.push_back(std::move(theme));
				}
			}
		}
	}
	bool USlateThemeManager::read_theme(const std::string& theme_data, FStyleTheme& out_theme)
	{
		rapidjson::Document document;
		if (!document.Parse(theme_data.c_str()).HasParseError())
		{
			int32_t version = 0;
			if (document.HasMember("Version"))
			{
				version = document["Version"].GetInt();
			}
			else
			{
				return false;//invalid file
			}

			std::string id_string;
			if (document.HasMember("Id"))
			{
				id_string = document["Id"].GetString();
				if (!FGuid::Parse(id_string, out_theme.m_id))
				{
					return false;
				}
			}
			else
			{
				return false;
			}

			std::string display_str;
			if (document.HasMember("DisplayName"))
			{
				display_str = document["DisplayName"].GetString();
			}
			else
			{
				return false;
			}

			out_theme.m_display_name = std::move(display_str);

			//just check that the theme has colos, we wont load theme unless the theme is used
			if (!document.HasMember("Colors"))
			{
				return false;
			}
		}

		return true;
	}
	void USlateThemeManager::apply_theme(FGuid theme_id)
	{
		//todo:theme id valid check

		FStyleTheme* current_theme = nullptr;
		if (m_current_theme_id != theme_id)
		{
			current_theme = &get_mutable_current_theme();
			//todo:unload existing colors
		}

		current_theme = &get_mutable_current_theme();

		//todo:load theme colors
		load_theme_colors(*current_theme);

		//apply the new colors
		std::memcpy(m_active_colors.m_style_colors, current_theme->m_loaded_default_colors.data(), sizeof(FLinearColor)*current_theme->m_loaded_default_colors.size());
	}
	//note:default construct

	const FSlateColor FStyleColors::Transparent =		EStyleColor::Transparent;
	const FSlateColor FStyleColors::Foreground =		EStyleColor::Foreground;//use table id
	const FSlateColor FStyleColors::ForegroundHover =	EStyleColor::ForegroundHover;
	const FSlateColor FStyleColors::Panel =				EStyleColor::Panel;
	const FSlateColor FStyleColors::Primary =			EStyleColor::Primary;;
	const FSlateColor FStyleColors::Input =				EStyleColor::Input;
	const FSlateColor FStyleColors::Recessed =			EStyleColor::Recessed;
	const FSlateColor FStyleColors::PrimaryHover =		EStyleColor::PrimaryHover;
	const FSlateColor FStyleColors::PrimaryPress =		EStyleColor::PrimaryPress;
	const FSlateColor FStyleColors::DropDown =			EStyleColor::DropDown;
	const FSlateColor FStyleColors::DropDownOutline =   EStyleColor::DropDownOutline;
	const FSlateColor FStyleColors::Background =		EStyleColor::Background;
	const FSlateColor FStyleColors::Secondary =			EStyleColor::Secondary;
	const FSlateColor FStyleColors::Hover =				EStyleColor::Hover;
	const FSlateColor FStyleColors::Hover2 =			EStyleColor::Hover2;
	const FSlateColor FStyleColors::White =				EStyleColor::White;

	const FSlateColor FStyleColors::WindowBorder =		EStyleColor::WindowBorder;
	const FSlateColor FStyleColors::InputOutline =		EStyleColor::InputOutline;
	const FSlateColor FStyleColors::Header =			EStyleColor::Header;

}