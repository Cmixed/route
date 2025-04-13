﻿#pragma once

#include <iostream>
#include <format>
#include <string>
#include <string_view>
#include <source_location>
#include <filesystem>


namespace zzj
{
	/***************************************************************************************** */
	// for control
	/***************************************************************************************** */

	struct ColorCtrlSettings
	{
		bool is_ToDefault{true};
	} inline color_ctrl;

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
		~Color();
		// member functions
		void switchOutFrontColor(ColorName color_name) const;
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
			std::cout << std::format("{}\n", m_outMessage);
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

	inline Color::~Color()
	{
		if (color_ctrl.is_ToDefault) {
			switchOutFrontColor(ColorName::DEFAULT);
		}
	}

	inline void Color::display() const
	{
		Color tempColor{m_colorName};
	}

	inline void Color::switchOutFrontColor(ColorName color_name) const
	{
		using enum ColorName;

		switch (color_name) {
		case DEFAULT: std::cout << "\033[0m";
			break;
		case RED: std::cout << "\033[31m";
			break;
		case GREEN: std::cout << "\033[32m";
			break;
		case YELLOW: std::cout << "\033[33m";
			break;
		case BLUE: std::cout << "\033[34m";
			break;
		case MAGENTA: std::cout << "\033[35m";
			break;
		case CYAN: std::cout << "\033[36m";
			break;
		case WHITE: std::cout << "\033[37m";
			break;
		default: std::cout << "{[Error]ColorName is not valid!}";
			break;
		}
	}
}


namespace zzj
{
	inline namespace literals
	{
		inline zzj::Log operator""_log(char const* str, size_t const len)
		{
			return zzj::Log{std::string(str, len)};
		}
	}
}
