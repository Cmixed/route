#pragma once

#include "pch.hpp"
#include "data.hpp"

namespace route
{
	/*****************************************************************
	 *
	 *		MENU 函数声明
	 *
	 *****************************************************************/

	/* 打印函数 */
	inline void print_path_result(route::WGraph const& graph, int const algorithm_number, 
				std::vector<PathTimePair> const& path_time_results);

	/* 使用路径计算函数 */
	inline auto sum_path(route::WGraph const& graph, PathEndPoints const pep) -> std::vector<PathTimePair>;
	inline auto calculate_path_times(route::WGraph const& graph, PathEndPoints const pep)
		-> std::vector<PathTimePair>;



	/*****************************************************************
	 *
	 *		MENU 函数定义
	 *
	 *****************************************************************/


	/**
	 * @brief 打印路径结果
	 * 
	 * @param graph 路径图结构
	 * @param algorithm_number 算法数量
	 * @param path_time_results 路径和时间结果集
	 */
	inline void print_path_result(route::WGraph const& graph, int const algorithm_number, std::vector<PathTimePair> const& path_time_results)
	{
	    static int order{ 0 };

	    std::println("\n----------第 {} 个路径规划----------\n", order); ++order;
	    for (int i = 0; i < algorithm_number; i++) {
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
	    auto measure_time = [&](auto&& algorithm, auto&& ...args) {
	        auto start = std::chrono::high_resolution_clock::now();
	        auto path = algorithm(std::forward<decltype(args)>(args)...);
	        auto end = std::chrono::high_resolution_clock::now();
	        results.push_back({path, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)});
	    };

	    // 使用 lambda 表达式测量不同算法的执行时间
	    measure_time([&](auto start, auto end) { return graph.localSearchOptimization(start, end); }, pep.startVertex, pep.endVertex);
	    measure_time([&](auto start, auto end) { return graph.geneticAlgorithm(start, end); }, pep.startVertex, pep.endVertex);
	    measure_time([&](auto start, auto end) { return graph.dijkstra(start, end); }, pep.startVertex, pep.endVertex);
	    measure_time([&](auto start, auto end) { return graph.geneticLocalSearchOptimization(start, end); }, pep.startVertex, pep.endVertex);

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
	        std::make_pair("Genetic+Local Search", [](auto& g, auto s, auto e) { return g.geneticLocalSearchOptimization(s, e, 50, 100); })
	    };

	    // 性能测量辅助函数
	    auto measure_performance = [&](const char* name, const AlgorithmFunc& algorithm) {
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
	        futures.emplace_back(std::async(std::launch::async, [name, algorithm, &graph, &pep, &measure_performance]() {
	            return measure_performance(name, algorithm);
	        }));
	    }

	    // 收集所有线程的结果
	    for (auto& future : futures) {
	        results.emplace_back(future.get());
	    }

	    return results;
	}

}
