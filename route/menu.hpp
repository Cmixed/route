﻿#pragma once

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
	inline auto sum_path(route::WGraph const& graph, PathEndpoints const pep) -> std::vector<PathTimePair>;
	template<typename... Args>
	inline auto calculate_path_times(route::WGraph const& graph, PathEndpoints const pep)
		-> std::vector<PathTimePair>;

	/*****************************************************************
	 *
	 *		MENU 函数定义
	 *
	 *****************************************************************/
	inline void print_path_result(route::WGraph const& graph, int const algorithm_number,std::vector<PathTimePair> const& path_time_results)
	{
	    static int order{ 0 };

	    std::println("\n----------第 {} 个路径规划----------\n", order); ++order;
	    for (int i = 0; i < algorithm_number; i++) {
	        auto const& [path_result, execution_time] = path_time_results[i];
	        auto const& [path, dis] = path_result;
	        switch (i) {
	        case 0:
	            std::println("\n=====退火局部搜索算法\n");
	            graph.printPath(path, dis);
	            std::println("执行时间: {} 纳秒", execution_time.count());
	            break;
	        case 1:
	            std::println("\n=====遗传算法:\n");
	            graph.printPath(path, dis);
	            std::println("执行时间: {} 纳秒", execution_time.count());
	            break;
	        case 2:
	            std::println("\n=====Dijkstra:\n");
	            graph.printPath(path, dis);
	            std::println("执行时间: {} 纳秒", execution_time.count());
	            break;
	        case 3:
	            std::println("\n=====遗传局部搜索:\n");
	            graph.printPath(path, dis);
	            std::println("执行时间: {} 纳秒", execution_time.count());
	            break;
	        }
	    }
	}



	/**
	 * @brief 计算时间与路径
	 * @param graph 
	 * @param pep 
	 * @return 
	 */
	inline auto sum_path(route::WGraph const& graph, PathEndpoints const pep) -> std::vector<PathTimePair>
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
	 * @brief 计算不同路径算法的时间与结果
	 * 
	 * 该函数对多种路径查找算法进行性能测试，返回包含路径和执行时间的结果集
	 * 
	 * @param graph 路径图结构
	 * @param pep 路径端点信息
	 * @return 包含路径和执行时间的结构体集合
	 */
	template<typename... Args>
	inline auto calculate_path_times(route::WGraph const& graph, PathEndpoints const pep) 
	    -> std::vector<PathTimePair>
	{
	    std::vector<PathTimePair> results;

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

	        results.emplace_back(PathTimePair{
	            std::move(path_result),
	            std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time)
	        });
	    };

	    // 遍历算法映射并执行测量
	    for (const auto& [name, algorithm] : algorithm_map) {
	        measure_performance(name, algorithm);
	    }

	    return results;
	}

}
