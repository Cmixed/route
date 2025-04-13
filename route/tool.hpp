// tool fuc
#pragma once

#include "pch.hpp"

namespace route
{
	/*****************************************************************
	 *
	 *		Tool 函数声明
	 *
	 *****************************************************************/

	/* 打印函数 */
	inline void print_pure_path(const std::vector<int>& path, int const distance);

	/* 工具函数 */
	template <typename Enum, Enum... Values>
	inline constexpr int count_enum_values();

	/* 遗传算法配套函数 */
	inline bool is_valid_path(const std::vector<int>& path, const std::vector<std::vector<int>>& adj_matrix);
	inline auto initialize_population(int const start, int const end, int const vertices,
	                                  int const population_size) -> std::vector<std::vector<int>>;
	inline int calculate_path_distance(const std::vector<int>& path,
	                                   const std::vector<std::vector<int>>& adj_matrix);
	inline auto select(const std::vector<std::vector<int>>& population,
		const std::vector<std::vector<int>>& adj_matrix, std::mt19937& rng) -> std::vector<int>;
	inline auto crossover(const std::vector<int>& parent1, const std::vector<int>& parent2,
		std::mt19937& rng) -> std::vector<int>;
	inline void mutate(std::vector<int>& path, std::mt19937& rng);



	/*****************************************************************
	 *
	 *		Tool 函数实现
	 *
	 *****************************************************************/

	/* 打印函数 */
	/**
	 * @brief 打印最短路径及其总距离。
	 * @param path 最短路径。
	 * @param distance 总距离。
	 */
	inline void print_pure_path(const std::vector<int>& path, int const distance)
	{
		if (path.empty()) {
			std::println("No path found.");
			return;
		}

		std::print("Shortest path: ");
		for (size_t i = 0; i < path.size(); ++i) {
			std::cout << path[i];
			if (i != path.size() - 1) {
				std::print(" -> ");
			}
		}
		std::println("\nTotal distance: {}", distance);
	}

	/* 工具函数 */

	/// 模板元编程：计算枚举项的数量
	template <typename Enum, Enum... Values>
	struct EnumCounter {
	    static constexpr int value = sizeof...(Values);
	};

	/**
	 * @brief 获取枚举类的大小
	 * @tparam Enum 
	 * @return 
	 */
	template <typename Enum, Enum... Values>
	inline constexpr int count_enum_values() {
	    return EnumCounter<Enum, Values...>::value;
	}


	/* 遗传算法配套函数 */
	/**
	 * @brief 检查路径是否有效
	 * 
	 * 该函数检查给定的路径是否在图中有效。路径有效是指路径中每两个相邻节点之间都有边连接。
	 * 
	 * @param path 要检查的路径，表示为节点序列
	 * @param adj_matrix 图的邻接矩阵，adj_matrix[i][j] 为 -1 表示节点 i 和 j 之间没有边
	 * @return true 如果路径有效
	 * @return false 如果路径无效
	 */
	inline bool is_valid_path(const std::vector<int>& path, const std::vector<std::vector<int>>& adj_matrix)
	{
		// 遍历路径中的每两个相邻节点
		for (size_t i = 0; i < path.size() - 1; ++i) {
			int const current_node = path[i];

			// 检查这两个节点之间是否有边
			if (int const next_node = path[i + 1];
				adj_matrix[current_node][next_node] == -1) {
				return false;
			}
		}

		// 所有相邻节点之间都有边，路径有效
		return true;
	}

	/**
	 * @brief 初始化种群
	 * 
	 * 该函数生成一个种群，种群中的每个个体代表一条从起始节点到结束节点的路径。
	 * 路径包含所有中间节点，且中间节点的顺序是随机排列的。
	 * 
	 * @param start 起始节点编号
	 * @param end 结束节点编号
	 * @param vertices 图中总节点数
	 * @param population_size 种群大小（即生成的路径数量）
	 * @return std::vector<std::vector<int>> 生成的种群，每个元素是一个路径
	 */
	inline auto initialize_population(int const start, int const end, int const vertices,
	                                  int const population_size) -> std::vector<std::vector<int>>
	{
		// 创建中间节点列表（排除起始和结束节点）
		std::vector<int> nodes;
		for (int i = 0; i < vertices; ++i) {
			if (i != start && i != end) {
				nodes.push_back(i);
			}
		}

		// 随机数生成器
		std::random_device rd;
		std::mt19937 rng(rd());

		// 创建种群
		std::vector<std::vector<int>> population;
		population.reserve(population_size); // 预分配内存以提高性能

		for (int i = 0; i < population_size; ++i) {
			// 打乱中间节点顺序
			std::ranges::shuffle(nodes, rng);

			// 构建路径：起始节点 + 打乱后的中间节点 + 结束节点
			std::vector<int> path = {start};
			path.insert(path.end(), nodes.begin(), nodes.end());
			path.push_back(end);

			population.push_back(path);
		}

		return population;
	}

	/**
	 * @brief 计算路径的总距离
	 * 
	 * 该函数根据图的邻接矩阵计算给定路径的总距离。路径由节点序列组成，函数会遍历路径中的每两个相邻节点，并累加它们之间的边权重。
	 * 如果路径中存在无效的边（即邻接矩阵中对应的值为 -1），函数将抛出异常。
	 * 
	 * @param path 要计算距离的路径，表示为节点序列
	 * @param adj_matrix 图的邻接矩阵，adj_matrix[i][j] 表示节点 i 到 j 的边权重，-1 表示无边
	 * @return int 路径的总距离
	 * @throw std::invalid_argument 如果路径中存在无效的边
	 */
	inline int calculate_path_distance(const std::vector<int>& path,
	                                   const std::vector<std::vector<int>>& adj_matrix)
	{
		int distance = 0;

		// 遍历路径中的每两个相邻节点
		for (size_t i = 0; i < path.size() - 1; ++i) {
			int const current_node = path[i];
			int const next_node = path[i + 1];

			// 获取两个节点之间的边权重
			int const edge_weight = adj_matrix[current_node][next_node];

			// 检查边是否有效
			if (edge_weight == -1) {
				throw std::invalid_argument("路径中存在无效的边");
			}

			// 累加距离
			distance += edge_weight;
		}

		return distance;
	}

	/**
	 * @brief 从种群中选择一个路径
	 * 
	 * 该函数使用轮盘赌选择算法从种群中选择一个路径。路径的适应度分数基于路径的长度计算，
	 * 路径越短，适应度分数越高。函数会根据适应度分数的概率分布随机选择一个路径。
	 * 
	 * @param population 种群，包含多个路径
	 * @param adj_matrix 图的邻接矩阵，用于验证路径有效性和计算路径距离
	 * @param rng 随机数生成器
	 * @return std::vector<int> 被选中的路径
	 * @throw std::invalid_argument 如果种群为空
	 */
	inline auto select(const std::vector<std::vector<int>>& population,
	                               const std::vector<std::vector<int>>& adj_matrix, std::mt19937& rng)-> std::vector<int>
	{
		if (population.empty()) {
			throw std::invalid_argument("种群为空");
		}

		std::vector<double> fitness_scores;
		double total_fitness = 0.0;

		// 计算每个路径的适应度分数和总适应度
		for (const auto& path : population) {
			double fitness = 0.0;
			if (is_valid_path(path, adj_matrix)) {
				int const distance = calculate_path_distance(path, adj_matrix);
				fitness = 1.0 / (distance + 1); // 路径越短，适应度越高
			}
			fitness_scores.push_back(fitness);
			total_fitness += fitness;
		}

		// 如果所有路径都无效，随机选择一个路径
		if (total_fitness == 0.0) {
			std::uniform_int_distribution<int> dist(0, static_cast<int>(population.size()) - 1);
			return population[dist(rng)];
		}

		// 轮盘赌选择
		std::uniform_real_distribution<double> dist(0.0, total_fitness);
		double const target = dist(rng);
		double cumulative = 0.0;

		for (size_t i = 0; i < population.size(); ++i) {
			cumulative += fitness_scores[i];
			if (cumulative >= target) {
				return population[i];
			}
		}

		// 如果循环结束仍未选择，返回最后一个路径（理论上不会执行到这里）
		return population.back();
	}

	/**
	 * @brief 执行交叉操作生成子代路径
	 * 
	 * 该函数通过两个父代路径进行交叉操作，生成一个新的子代路径。
	 * 交叉操作的过程如下：
	 * 1. 随机选择一个区间 [start, end]，将父代1的该区间复制到子代。
	 * 2. 遍历父代2的元素，将未出现在子代中的元素依次填充到子代的空位中。
	 * 
	 * @param parent1 父代路径1
	 * @param parent2 父代路径2
	 * @param rng 随机数生成器
	 * @return std::vector<int> 生成的子代路径
	 */
	inline auto crossover(const std::vector<int>& parent1, const std::vector<int>& parent2,
	                                  std::mt19937& rng) -> std::vector<int>
	{
		std::vector<int> child(parent1.size(), -1);

		// 随机选择交叉的起始和结束位置
		std::uniform_int_distribution<int> dist(0, static_cast<int>(parent1.size()) - 1);
		int start = dist(rng);
		int end = dist(rng);

		// 确保 start <= end
		if (start > end) {
			std::swap(start, end);
		}

		// 将父代1的 [start, end] 区间复制到子代
		for (int i = start; i <= end; ++i) {
			child[i] = parent1[i];
		}

		// 填充子代中剩余的空位，使用父代2中未出现在子代中的元素
		size_t insertPos = 0;
		for (const auto& elem : parent2) {
			// 如果元素不在子代中
			if (std::ranges::find(child, elem) == child.end()) {
				// 找到子代中下一个空位
				while (insertPos < child.size() && child[insertPos] != -1) {
					++insertPos;
				}
				// 如果找到空位，填入元素
				if (insertPos < child.size()) {
					child[insertPos] = elem;
				}
			}
		}

		return child;
	}

	/**
	 * @brief 对路径进行变异操作
	 * 
	 * 该函数对给定的路径执行变异操作。变异过程如下：
	 * 1. 如果路径长度小于3，则不进行变异。
	 * 2. 随机选择两个不同的位置（排除路径的起始和结束节点）。
	 * 3. 交换这两个位置上的节点。
	 * 
	 * @param path 要变异的路径
	 * @param rng 随机数生成器
	 */
	inline void mutate(std::vector<int>& path, std::mt19937& rng)
	{
		if (path.size() < 3) {
			return;
		}

		// 随机选择两个不同的位置（排除路径的起始和结束节点）
		std::uniform_int_distribution<int> dist(1, static_cast<int>(path.size()) - 2);
		int const i = dist(rng);
		int j = dist(rng);

		// 如果两个位置相同，则重新生成 j
		while (i == j) {
			j = dist(rng);
		}

		// 交换两个位置上的节点
		std::swap(path[i], path[j]);
	}

}
