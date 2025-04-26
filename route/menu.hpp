#pragma once

#ifndef MENU_HPP
#define MENU_HPP


#include "pch.hpp"
#include "data.hpp"
#include "file_io.hpp"
#include "col_zzj.hpp"
#include <cstdlib>

using namespace zzj;
using namespace zzj::literals;
using namespace std::literals;

namespace route
{
	/*****************************************************************
	 *
	 *		MENU 声明
	 *
	 *****************************************************************/

	/* 全局变量 */
	constexpr int option_num{ 10 };

	/* 打印函数 */
	void print_path_result(route::WGraph const& graph, int const algorithm_number,
	                              std::vector<PathTimePair> const& path_time_results);

	/* 使用路径计算函数 */
	auto sum_path(route::WGraph const& graph, PathEndPoints const pep) -> std::vector<PathTimePair>;
	auto calculate_path_times(route::WGraph const& graph, PathEndPoints const pep)
		-> std::vector<PathTimePair>;

	inline std::array<std::string, option_num> menu_option{
		"进行计算",
		"11"
	};

	auto paths_task(WGraph const& g, std::vector<PathEndPoints> const& pep)
		-> std::optional<std::vector<std::vector<PathTimePair>>>;


	enum class MessageType : std::int_fast8_t
	{
		NORMAL,
		NOTE,
		MESSAGE,
		WARNING,
		ERROR
	};

	using MsgTy = MessageType;


	/* 界面类 */
	class Menu
	{
	private:
		using TimePoint = std::chrono::high_resolution_clock::time_point;

		struct StatFlag
		{
			bool is_ready{ false };
			bool is_fresh{ false };
			bool is_wait{ false };
			bool is_readFile{ false };
			bool is_writeFile{ false };
			bool is_prnMsg{ false };
		} statFlag;

		std::string m_systemName{ "路径规划系统" };
		std::string m_userName{ "user" };

		int m_optionId{ 1 };
		int m_option{ 1 };


		TimePoint m_startTime{std::chrono::high_resolution_clock::now()};
		TimePoint m_latestTime{m_startTime};

	public:
		Menu() = default;
		explicit(false) Menu(std::string_view const usr_name)
			: m_userName{ usr_name } {
		}
		explicit(false) Menu(std::string_view const sys_name , std::string_view const usr_name)
			: m_systemName{ sys_name }, m_userName{ usr_name } {
		}
		Menu(Menu&&) = default;
		~Menu() = default;
		Menu(Menu const&) = default;
		Menu& operator=(Menu const&) = default;
		Menu& operator=(Menu&&) = default;

		void printMsg(MsgTy const ty, std::string_view msg);

		void statusBar();
		void statusBarFr();
		void fresh();
		void waitEnter();
		void options();
		auto readFile(WGraph& graph, std::string const& file_name)
		-> std::optional<int>;
		auto writeFile(WGraph& graph, std::string const& file_name)
		-> std::optional<int>;

		void ready();
	};


	/*****************************************************************
	 *
	 *		MENU 定义
	 *
	 *****************************************************************/

	inline void Menu::printMsg(MsgTy const ty, std::string_view msg)
	{
		statFlag.is_prnMsg = true;

		std::string tyMsg;
		auto col = Color(ColorName::DEFAULT);
		switch (ty) {
		case MsgTy::MESSAGE:
			col.change(ColorName::CYAN);
			tyMsg = "MSG";
			break;
		case MsgTy::NOTE:
			col.change(ColorName::YELLOW);
			tyMsg = "NOTE";
			break;
		case MsgTy::WARNING:
			col.change(ColorName::YELLOW);
			tyMsg = "WARNING";
			break;
		case MsgTy::ERROR:
			col.change(ColorName::RED);
			tyMsg = "ERROR";
			break;
		case MsgTy::NORMAL:
			col.change(ColorName::GREEN);
			tyMsg = "NORMAL";
			break;
		default:
			tyMsg = "NULL";
			col.change(ColorName::DEFAULT);
		}
		col.print();
		std::println("[{}][{}]:「{}」", "系统", tyMsg, msg);
	}


	inline void Menu::statusBar()
	{
		auto const latestTime = std::chrono::high_resolution_clock::now();
		auto const dur = latestTime - m_latestTime;

		m_latestTime = latestTime;

		auto col = Color(ColorName::CYAN);

		std::print("[{1:}][{0:}]", m_userName, m_systemName);
		col.changePrn(ColorName::GREEN);
		std::println("[{:%Y-%m-%d %H:%M}][{}{}s]", 
			std::chrono::system_clock::now(), 
			"Times：", std::chrono::duration_cast<std::chrono::seconds>(dur).count());
		col.changePrn(ColorName::MAGENTA);
		if (statFlag.is_readFile) {
			std::print("[文件读取成功]");
		} else {
			std::print("[未读取文件]");
		}
		if (statFlag.is_writeFile) {
			std::print("[文件写入成功]");
		} else {
			std::print("[未写入文件]");
		}
		col.changePrn(ColorName::RED);
		std::println("[End]");

		statFlag.is_fresh = false;
	}

	inline void Menu::statusBarFr()
	{
		if (!statFlag.is_fresh) {
			Menu::fresh();
			this->statusBar();
		}
		statFlag.is_fresh = false;
	}

	inline void Menu::fresh()
	{
		static std::mutex mtx;
		{
			std::scoped_lock<std::mutex> lock(mtx);

			if constexpr (system_info.is_windows) {
				std::system("cls");
			}
			else {
				std::system("clear");
			}
			statFlag.is_fresh = { true };
		}
	}

	inline void Menu::waitEnter()
	{
		auto const markedWords{ "按下回车键以继续"s };

		statFlag.is_wait = true;
		{
			auto col = Color{ ColorName::YELLOW };
			this->printMsg(MessageType::NOTE, markedWords);
			std::cin.clear();
			if (std::cin.rdbuf()->in_avail() > 0) {
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		statFlag.is_wait = false;
	}

	inline auto Menu::readFile(WGraph& graph, std::string const& file_name)
	-> std::optional<int>
	{
		statFlag.is_readFile = false;
		if (read_from_file(graph, file_name)) {
			statFlag.is_readFile = true;
			return { 1 };
		} else {
			std::cerr << "文件读入失败!" << "\n";
			return { 0 };
		}
	}

	inline auto Menu::writeFile(WGraph& graph, std::string const& file_name)
	-> std::optional<int>
	{
		statFlag.is_writeFile = false;
	    if (write_to_file(graph, file_name))
	    {
			statFlag.is_writeFile = true;
			return { 1 };
	    }
	    else
	    {
	        std::cerr << "图数据保存失败!" << "\n";
			return { 0 };
	    }
	}


	inline void options()
	{
		std::vector<int> vv;

		for (auto val : std::ranges::istream_view<int>(std::cin)
			| std::views::take_while([](const auto& v) {return v < 5; })
			| std::views::transform([](const auto& v) {return v * 2; })) {
			std::println("> {}", val);
			vv.push_back(val);
		}
		std::println("end input");
	}

	inline void Menu::ready()
	{
		statFlag.is_ready = true;
		this->printMsg(MessageType::MESSAGE, "系统就绪！");
	}

	/**
	 * @brief 打印路径结果
	 * 
	 * @param graph 路径图结构
	 * @param algorithm_number 算法数量
	 * @param path_time_results 路径和时间结果集
	 */
	inline void print_path_result(route::WGraph const& graph, int const algorithm_number,
	                              std::vector<PathTimePair> const& path_time_results)
	{
		static std::uint_fast32_t order{1};

		std::println("\n----------第 {} 个路径规划----------\n", order);
		++order;
		for (int i = 0; i < algorithm_number; ++i) {
			auto const& [path_result, execution_time] = path_time_results[i];
			auto const& [path, dis] = path_result;

			std::string algorithm_name;
			switch (static_cast<Algorithm>(i)) {
			case Algorithm::SimulatedAnnealing:
				algorithm_name = "退火局部搜索算法";
				break;
			case Algorithm::GeneticAlgorithm:
				algorithm_name = "遗传算法";
				break;
			case Algorithm::Dijkstra:
				algorithm_name = "Dijkstra";
				break;
			case Algorithm::GeneticLocalSearch:
				algorithm_name = "遗传局部搜索";
				break;
			}

			std::println("\n===== {}: =====", algorithm_name);
			graph.printPath(path, dis);
			std::println("执行时间: {} 纳秒", execution_time.count());
		}
	}

	/**
	 * @brief 计算时间与路径
	 * @param graph 
	 * @param pep 
	 * @return 
	 */
	inline auto sum_path(route::WGraph const& graph, PathEndPoints const pep) -> std::vector<PathTimePair>
	{
		std::vector<PathTimePair> results;

		// 定义一个 lambda 表达式，用于测量算法执行时间并存储结果
		auto measure_time = [&]<typename Algorithm, typename... Args>(Algorithm&& algorithm, Args&&... args)
		{
			auto const start = std::chrono::high_resolution_clock::now();
			auto path = std::forward<Algorithm>(algorithm)(std::forward<Args>(args)...);
			auto const end = std::chrono::high_resolution_clock::now();
			results.push_back({path, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)});
		};

		// 使用 lambda 表达式测量不同算法的执行时间
		measure_time([&](auto start, auto end) { return graph.localSearchOptimization(start, end); }, pep.startVertex,
		             pep.endVertex);
		measure_time([&](auto start, auto end) { return graph.geneticAlgorithm(start, end); }, pep.startVertex,
		             pep.endVertex);
		measure_time([&](auto start, auto end) { return graph.dijkstra(start, end); }, pep.startVertex, pep.endVertex);
		measure_time([&](auto start, auto end) { return graph.geneticLocalSearchOptimization(start, end); },
		             pep.startVertex, pep.endVertex);

		return results;
	}

	/**
	 * @brief 计算不同路径算法的时间与结果（多线程版本，优化内存分配）
	 * 
	 * 该函数对多种路径查找算法进行性能测试，返回包含路径和执行时间的结果集。
	 * 使用多线程并行执行每个算法的性能测试，提高性能，并预分配内存以避免多次重新分配。
	 * 
	 * @param graph 路径图结构
	 * @param pep 路径端点信息
	 * @return 包含路径和执行时间的结构体集合
	 */
	inline auto calculate_path_times(route::WGraph const& graph, PathEndPoints const pep)
		-> std::vector<PathTimePair>
	{
		// 使用 std::function 统一算法的调用方式
		using AlgorithmFunc = std::function<std::pair<std::vector<int>, int>(route::WGraph const&, int, int)>;

		// 算法名称与对应函数的映射
		std::array<std::pair<const char*, AlgorithmFunc>, 4> algorithm_map = {
			std::make_pair("Local Search", [](auto& g, auto s, auto e) { return g.localSearchOptimization(s, e); }),
			std::make_pair("Genetic Algorithm", [](auto& g, auto s, auto e) { return g.geneticAlgorithm(s, e); }),
			std::make_pair("Dijkstra", [](auto& g, auto s, auto e) { return g.dijkstra(s, e); }),
			std::make_pair("Genetic+Local Search", [](auto& g, auto s, auto e)
			{
				return g.geneticLocalSearchOptimization(s, e, 50, 100);
			})
		};

		// 性能测量辅助函数
		auto measure_performance = [&](const char* name, const AlgorithmFunc& algorithm)
		{
			const auto start_time = std::chrono::high_resolution_clock::now();
			auto path_result = algorithm(graph, pep.startVertex, pep.endVertex);
			const auto end_time = std::chrono::high_resolution_clock::now();

			return PathTimePair{
				std::move(path_result),
				std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time)
			};
		};

		// 预分配 futures 和 results 的容量，避免多次内存重新分配
		std::vector<std::future<PathTimePair>> futures;
		futures.reserve(algorithm_map.size());

		std::vector<PathTimePair> results;
		results.reserve(algorithm_map.size());

		// 使用多线程并行执行每个算法的性能测量
		for (const auto& [name, algorithm] : algorithm_map) {
			futures.emplace_back(std::async(std::launch::async, [name, algorithm, &graph, &pep, &measure_performance]()
			{
				return measure_performance(name, algorithm);
			}));
		}

		// 收集所有线程的结果
		for (auto& future : futures) {
			results.emplace_back(future.get());
		}

		return results;
	}


	/**
	 * @brief 异步计算多个路径的通行时间。
	 *
	 * @param g 有向加权图。
	 * @param pep 路径端点列表。
	 * @return std::optional<std::vector<std::vector<PathTimePair>>> 包含路径通行时间的二维向量，
	 *         如果异步任务正常完成则返回有效值，否则返回空。
	 */
	inline auto paths_task(WGraph const& g, std::vector<PathEndPoints> const& pep)
	    -> std::optional<std::vector<std::vector<PathTimePair>>>
	{
	    std::vector<std::future<std::vector<PathTimePair>>> f_pt;
	    f_pt.reserve(pep.size() * (algo_num + 1));

	    for (const auto& endpoints : pep) {
	        // 启动异步任务，计算每对端点的路径通行时间
	        f_pt.emplace_back(std::async(std::launch::async,
	        [g, endpoints]() { return route::calculate_path_times(g, endpoints); }));
	    }

	    std::vector<std::vector<PathTimePair>> path_results;

	    // 等待所有异步任务完成并收集结果
	    for (auto& future : f_pt) {
	        try {
	            path_results.push_back(future.get());
	        } catch (const std::exception& e) {
	            // 如果某个异步任务抛出异常，记录错误并返回空值
	            std::println(std::cerr, "异步任务出错:{}", e.what());
	            return {};
	        }
	    }

	    // 返回所有路径的通行时间结果
	    return path_results;
	}
}

#endif
