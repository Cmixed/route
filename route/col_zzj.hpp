#pragma once

#ifndef COL_ZZJ_HPP
#define COL_ZZJ_HPP

#include <iostream>
#include <print>
#include <format>
#include <string>
#include <string_view>
#include <source_location>
#include <filesystem>

namespace zzj
{
	/*****************************************************************
	 *
	 *		Tool 函数声明
	 *
	 *****************************************************************/

	enum class ColorName : std::uint_fast8_t
	{
		DEFAULT = 0,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE
	};

	struct ColorCtrlSettings
	{
		bool is_ToDefault{true};
		ColorName default_color{ColorName::YELLOW};
	} inline color_ctrl;

	// main LogCtrl
	struct LogCtrlSettings
	{
		bool is_Log{true};
		bool is_ToFile{false};

		// LogTerminalCtrl
		struct LogTerminalCtrlSettings
		{
			std::string g_initialMessage{"[Log]"};
			bool is_logMessage{true};
			bool is_logFilePath{false};
			bool is_logFileName{false};
			bool is_logFunctionName{true};
			bool is_logLineNumber{true};
			bool is_logColumnNumber{true};
		} LogTerminalCtrl;

		// LogFileCtrl
		struct LogFileCtrlSettings
		{
			std::string logFileName{"log.txt"};
			std::filesystem::path logFilePath{std::filesystem::current_path() / logFileName};
		} LogFileCtrl;
	} inline log_ctrl;


	/***************************************************************************************** */
	// for class
	/***************************************************************************************** */
	class Color
	{
	private:
		ColorName m_colorName;

	public:
		// class functions
		Color() = default;
		explicit Color(ColorName const color_name);
		explicit Color(ColorName const color_name, std::string_view const sv);
		~Color();
		// member functions
		void switchOutFrontColor(ColorName const color_name) const;
		void display() const;
	};

	class Log
	{
	private:
		// variables
		std::string m_outMessage;
		std::string_view m_message;
		std::source_location m_location;
		Color m_color;
		// functions
	public:
		Log(std::string_view message, ColorName display_color,
		    std::source_location const& location);
		~Log() = default;
		// member functions
		void display();
	};


	/***************************************************************************************** */
	// for coding
	/***************************************************************************************** */

	inline Log::Log(std::string_view const message, ColorName const display_color = ColorName::YELLOW,
	                std::source_location const& location = std::source_location::current())
		: m_outMessage{log_ctrl.LogTerminalCtrl.g_initialMessage}, m_message{message},
		  m_location{location}, m_color{display_color}
	{
		if (log_ctrl.is_Log) {
			if (log_ctrl.LogTerminalCtrl.is_logMessage) {
				m_outMessage += std::format("[Msg:{}]", m_message);
			}
			if (log_ctrl.LogTerminalCtrl.is_logFileName) {
				m_outMessage += std::format("[File:{}]", m_location.file_name());
			}
			if (log_ctrl.LogTerminalCtrl.is_logFunctionName) {
				m_outMessage += std::format("[Func:{}]", m_location.function_name());
			}
			if (log_ctrl.LogTerminalCtrl.is_logLineNumber) {
				m_outMessage += std::format("[Line:{}]", m_location.line());
			}
			if (log_ctrl.LogTerminalCtrl.is_logColumnNumber) {
				m_outMessage += std::format("[Col:{}]", m_location.column());
			}
			std::println("{}", m_outMessage);
		}
	}

	inline void Log::display()
	{
		m_color.display();
		std::cout << std::format("{}\n", m_outMessage);
	}


	inline Color::Color(ColorName const color_name)
		: m_colorName{color_name}
	{
		switchOutFrontColor(color_name);
	}

	inline Color::Color(ColorName const color_name, std::string_view const sv)
		: m_colorName{color_name}
	{
		switchOutFrontColor(color_name);
		std::println("{}", sv);
	}

	inline Color::~Color()
	{
		if (color_ctrl.is_ToDefault) {
			switchOutFrontColor(color_ctrl.default_color);
		}
	}

	inline void Color::display() const
	{
		Color tempColor{m_colorName};
	}

	inline void Color::switchOutFrontColor(ColorName const color_name) const
	{
		using enum ColorName;

		std::string color{};

		switch (color_name) {
		case DEFAULT:
			color = "\033[0m";
			break;
		case RED:
			color = "\033[31m";
			break;
		case GREEN:
			color = "\033[32m";
			break;
		case YELLOW:
			color = "\033[33m";
			break;
		case BLUE:
			color = "\033[34m";
			break;
		case MAGENTA:
			color = "\033[35m";
			break;
		case CYAN:
			color = "\033[36m";
			break;
		case WHITE:
			color = "\033[37m";
			break;
		default:
			color = "\033[0m{[Error]ColorName is not valid!}";
			break;
		}

		std::print("{}", color);
	}
}


namespace zzj
{
	inline namespace literals
	{
		inline Log operator""_log(char const* str, size_t const len)
		{
			return Log{std::string(str, len)};
		}

		inline Color operator""_col(char const* str, size_t const len)
		{
			return Color{color_ctrl.default_color, std::string(str, len)};
		}
	}
}

#endif