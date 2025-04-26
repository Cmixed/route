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
	 *		col_zzj 声明
	 *
	 *****************************************************************/

	struct SystemInfo
	{
		bool is_windows{ true };
		bool is_linux{ false };
		bool is_mac{ false };
	} constexpr system_info;

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

	using CName = ColorName;

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


	/*****************************************************************
	 *
	 *		Tool 类声明
	 *
	 *****************************************************************/
	class Color
	{
	private:
		ColorName m_colorName;
		std::string m_colorFrCode;
	public:
		// class functions
		Color() = delete;
		Color(Color const&) = default;
		Color& operator=(Color const&) = default;
		Color& operator=(Color&&) = default;
		explicit Color(CName const cname);
		explicit Color(CName const cname, std::string_view const msg);
		~Color();

		Color(Color&&) = delete;

		// member functions

		static auto conv2RelFrColor(CName const cname) -> std::string;
		static void displayFrColor(CName const cname);
		void print() const;
		void prnRelFrColor() const;
		void change(CName const cname);
		void changePrn(CName const cname);
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
		Log& operator=(const Log&) = default;
		Log(Log const&) = default;

		Log(Log&&) = delete;
		Log& operator=(Log&&) = delete;
		~Log() = default;
		// member functions
		void display();
	};


	/*****************************************************************
	 *
	 *		Tool 函数
	 *
	 *****************************************************************/

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
		m_color.print();
		std::print("{}\n", m_outMessage);
	}


	inline Color::Color(CName const cname)
		: m_colorName{cname}, m_colorFrCode{conv2RelFrColor(cname)}
	{
		print();
	}

	inline Color::Color(CName const cname, std::string_view const msg)
		: Color(cname)
	{
		std::println("{}", msg);
	}

	inline Color::~Color()
	{
		if (color_ctrl.is_ToDefault) {
			Color::displayFrColor(color_ctrl.default_color);
		}
	}

	inline void Color::prnRelFrColor() const
	{
		std::print("{}", m_colorFrCode);
	}

	inline void Color::print() const
	{
		this->prnRelFrColor();
	}

	inline void Color::change(CName const cname)
	{
		m_colorName = cname;
		m_colorFrCode = conv2RelFrColor(cname);
	}

	inline void Color::changePrn(CName const cname)
	{
		this->change(cname);
		this->print();
	}

	inline void Color::displayFrColor(CName const cname)
	{
		std::print("{}", conv2RelFrColor(cname));
	}

	inline auto Color::conv2RelFrColor(CName const cname)
		-> std::string
	{
		using enum ColorName;

		std::string color;

		switch (cname) {
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

		return color;
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