// data 的具体实现

#include "data.hpp"

namespace route
{
	/*****************************************************************
	 *
	 *		WGraph 类 函数实现
	 *
	 *****************************************************************/

	/* 基础方法 */
	/**
	 * @brief 添加一个顶点到图中。
	 * @param vertex 要添加的顶点。
	 */
	inline void WGraph::addVertex(const std::shared_ptr<Object>& vertex)
	{
		if (vertex != nullptr) {
			// 使用顶点的 id 作为键存储到 map 中
			m_vertexMap[vertex->m_id] = vertex;
		}
	}

	/**
	 * @brief 批量添加顶点到图中。
	 * @param vertices 可变参数模板，用于传递多个顶点。
	 */
	template <typename... Args>
	inline void WGraph::addVertices(Args&&... vertices)
	{
		(addVertex(std::forward<Args>(vertices)), ...);
	}

	/**
	 * @brief 添加一条带权重的边到图中。
	 * @param src 起始顶点。
	 * @param dest 目标顶点。
	 * @param weight 边的权重。
	 */
	inline void WGraph::addEdge(int const src, int const dest, int const weight)
	{
		if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices && weight > 0) {
			m_adjMatrix[src][dest] = weight;
			m_adjMatrix[dest][src] = weight;
			m_edges++;
		}
	}

	/**
	 * @brief 获取两个顶点之间的边的权重。
	 * @param src 起始顶点。
	 * @param dest 目标顶点。
	 * @return 边的权重，如果顶点无效则返回-1。
	 */
	[[nodiscard]] inline int WGraph::getWeight(int const src, int const dest) const
	{
		if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices) {
			return m_adjMatrix[src][dest];
		}
		return -1;
	}

	/**
	 * @brief 获取指定索引的顶点信息。
	 * @param id 顶点的索引。
	 * @return 顶点信息，如果索引无效则返回nullptr。
	 */
	[[nodiscard]] inline auto WGraph::getVertex(int const id) const -> std::shared_ptr<Object>
	{
		if (auto const it = m_vertexMap.find(id);
			it != m_vertexMap.end()) {
			return it->second;
		}
		return nullptr;
	}


	/* 路径算法 */
	/**
	 * @brief 使用 Dijkstra 算法计算两个顶点之间的最短路径。
	 * @param start 起始顶点。
	 * @param end 结束顶点。
	 * @return 一个包含最短路径和总距离的元组。如果找不到路径，距离为-1。
	 */
	[[nodiscard]] inline auto WGraph::dijkstra(int const start, int const end) const -> std::pair<std::vector<int>, int>
	{
		if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices) {
			return {{}, -1};
		}

		std::priority_queue<std::pair<int, int>,
		                    std::vector<std::pair<int, int>>,
		                    std::greater<>> pq;
		std::vector<int> dist(m_vertices, std::numeric_limits<int>::max());
		std::vector<int> prev(m_vertices, -1);

		dist[start] = 0;
		pq.emplace(0, start);

		while (!pq.empty()) {
			int const u = pq.top().second;
			pq.pop();

			if (u == end) {
				break;
			}

			for (int v = 0; v < m_vertices; ++v) {
				if (int const weight = m_adjMatrix[u][v];
					weight != -1) {
					if (dist[v] > dist[u] + weight) {
						dist[v] = dist[u] + weight;
						prev[v] = u;
						pq.emplace(dist[v], v);
					}
				}
			}
		}

		if (dist[end] == std::numeric_limits<int>::max()) {
			return {{}, -1};
		}

		std::vector<int> path;
		for (int at = end; at != -1; at = prev[at]) {
			path.push_back(at);
		}
		std::ranges::reverse(path);

		return {path, dist[end]};
	}

	/**
	 * @brief 使用遗传算法计算最短路径
	 * @param start 起点
	 * @param end 终点
	 * @return 最短路径和距离
	 */
	[[nodiscard]] inline auto WGraph::geneticAlgorithm(int const start,
	                                                   int const end) const -> std::pair<std::vector<int>, int>
	{
		// 检查起点和终点是否合法
		if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices) {
			return {{}, -1};
		}

		std::random_device rd;
		std::mt19937 rng(rd());

		constexpr int POPULATION_SIZE = 100;
		constexpr int MAX_GENERATIONS = 500;
		constexpr double CROSSOVER_RATE = 0.85;
		constexpr double MUTATION_RATE = 0.2;
		constexpr int ELITE_SIZE = 5;

		// 初始化种群
		std::vector<Path> population = initialize_population(start, end, m_vertices, POPULATION_SIZE);

		Path bestPath{};

		for (int generation = 0; generation < MAX_GENERATIONS; ++generation) {
			std::vector<Path> newPopulation;
			std::vector<int> distances(population.size());

			int bestDistanceInGeneration = std::numeric_limits<int>::max();

			// 计算适应度和最佳路径
			for (size_t i = 0; i < population.size(); ++i) {
				const Path& path = population[i];
				if (!is_valid_path(path, m_adjMatrix)) {
					distances[i] = -1;
					continue;
				}

				distances[i] = calculate_path_distance(path, m_adjMatrix);
				if (distances[i] < bestDistanceInGeneration) {
					bestDistanceInGeneration = distances[i];
					Path bestPathInGeneration = path;
					if (is_valid_path(bestPathInGeneration, m_adjMatrix)) {
						bestPath = bestPathInGeneration;
					}
				}
			}

			// 精英保留
			std::vector<std::pair<int, const Path*>> elitePaths;
			for (size_t i = 0; i < population.size(); ++i) {
				if (distances[i] != -1) {
					elitePaths.emplace_back(distances[i], &population[i]);
				}
			}

			if (!elitePaths.empty()) {
				std::ranges::sort(elitePaths, [](const auto& a, const auto& b) { return a.first < b.first; });

				for (int i = 0; i < ELITE_SIZE && i < static_cast<int>(elitePaths.size()); ++i) {
					newPopulation.push_back(*elitePaths[i].second);
				}
			}

			// 选择、交叉和变异
			while (newPopulation.size() < POPULATION_SIZE) {
				Path parent1 = select(population, m_adjMatrix, rng);
				Path parent2 = select(population, m_adjMatrix, rng);

				Path child;
				if (std::uniform_real_distribution<>(0.0, 1.0)(rng) < CROSSOVER_RATE) {
					child = crossover(parent1, parent2, rng);
				}
				else {
					child = parent1;
				}

				if (std::uniform_real_distribution<>(0.0, 1.0)(rng) < MUTATION_RATE) {
					mutate(child, rng);
				}

				newPopulation.push_back(child);
			}

			population = std::move(newPopulation);

			if constexpr (is_debug) {
				std::print("\rGeneration: {} / {}, Best Distance: {}",
				           generation + 1, MAX_GENERATIONS, bestDistanceInGeneration);
			}
		}

		if constexpr (is_debug) {
			std::println();
		}

		// 找到最优路径
		if (bestPath.empty()) {
			return {{}, -1};
		}

		int bestDistance = calculate_path_distance(bestPath, m_adjMatrix);
		return {bestPath, bestDistance};
	}

	/**
	 * @brief 使用局部搜索和模拟退火策略进行路径优化
	 * 
	 * 该函数通过贪心算法初始化路径，然后使用模拟退火策略进行路径优化，以找到从起点到终点的最短路径。
	 * 在优化过程中，随机选择两个城市进行交换，并根据路径长度的变化决定是否接受交换。
	 * 最终返回优化后的路径和总距离。
	 * 
	 * @param start 起点城市编号
	 * @param end 终点城市编号
	 * @return std::pair<std::vector<int>, int> 优化后的路径和总距离
	 * 
	 * @note 如果起点或终点无效，返回空路径和-1
	 */
	[[nodiscard]] inline auto WGraph::localSearchOptimization(int const start,
	                                                          int const end) const -> std::pair<std::vector<int>, int>
	{
		if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices) {
			return {{}, -1};
		}

		// 使用贪心算法初始化路径：从起点开始，每次选择最近的未访问城市
		std::vector<int> currentPath;
		currentPath.reserve(m_vertices);
		currentPath.push_back(start);
		std::vector<bool> visited(m_vertices, false);
		visited[start] = true;

		while (currentPath.size() < static_cast<size_t>(m_vertices)) {
			int const lastCity = currentPath.back();
			int nextCity = -1;
			int minDistance = std::numeric_limits<int>::max();

			for (int city = 0; city < m_vertices; ++city) {
				if (int const distance = m_adjMatrix[lastCity][city];
					!visited[city] && city != lastCity) {
					if (distance < minDistance && distance != -1) {
						// 确保距离有效
						minDistance = distance;
						nextCity = city;
					}
				}
			}

			if (nextCity == -1) break; // 无法继续扩展路径
			currentPath.push_back(nextCity);
			visited[nextCity] = true;
		}

		if (!currentPath.empty() && currentPath.back() != end) {
			currentPath.push_back(end); // 确保路径以终点结束
		}

		// 计算初始路径的总距离
		int currentDistance = 0;
		for (size_t i = 0; i < currentPath.size() - 1; ++i) {
			currentDistance += m_adjMatrix[currentPath[i]][currentPath[i + 1]];
		}

		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> dist(1, m_vertices - 2); // 避免交换起点和终点

		constexpr int MAX_ITERATIONS = 10000;
		constexpr double INITIAL_TEMPERATURE = 1000.0;
		constexpr double COOLING_RATE = 0.995;

		double temperature = INITIAL_TEMPERATURE;

		for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
			// 随机选择两个不同的位置进行交换
			int pos1 = dist(rng);
			int pos2 = dist(rng);
			while (pos1 == pos2) pos2 = dist(rng);

			// 创建候选路径
			std::vector<int> candidatePath = currentPath;
			std::swap(candidatePath[pos1], candidatePath[pos2]);

			// 计算交换前后的路径长度变化
			int delta = 0;

			// 计算交换前涉及的边
			int left1 = pos1 > 0 ? candidatePath[pos1 - 1] : -1;
			int right1 = pos1 < static_cast<int>(candidatePath.size()) - 1 ? candidatePath[pos1 + 1] : -1;
			int left2 = pos2 > 0 ? candidatePath[pos2 - 1] : -1;
			int right2 = pos2 < static_cast<int>(candidatePath.size()) - 1 ? candidatePath[pos2 + 1] : -1;

			// 交换后的路径中，涉及的边可能变化
			int originalSum = 0, newSum = 0;

			// 处理位置pos1的边
			if (left1 != -1) originalSum += m_adjMatrix[left1][candidatePath[pos1]];
			if (right1 != -1) originalSum += m_adjMatrix[candidatePath[pos1]][right1];

			// 处理位置pos2的边
			if (left2 != -1) originalSum += m_adjMatrix[left2][candidatePath[pos2]];
			if (right2 != -1) originalSum += m_adjMatrix[candidatePath[pos2]][right2];

			// 交换后的路径
			std::swap(candidatePath[pos1], candidatePath[pos2]);

			// 计算新路径中涉及的边
			int newLeft1 = pos1 > 0 ? candidatePath[pos1 - 1] : -1;
			int newRight1 = pos1 < static_cast<int>(candidatePath.size()) - 1 ? candidatePath[pos1 + 1] : -1;
			int newLeft2 = pos2 > 0 ? candidatePath[pos2 - 1] : -1;
			int newRight2 = pos2 < static_cast<int>(candidatePath.size()) - 1 ? candidatePath[pos2 + 1] : -1;

			if (newLeft1 != -1) newSum += m_adjMatrix[newLeft1][candidatePath[pos1]];
			if (newRight1 != -1) newSum += m_adjMatrix[candidatePath[pos1]][newRight1];
			if (newLeft2 != -1) newSum += m_adjMatrix[newLeft2][candidatePath[pos2]];
			if (newRight2 != -1) newSum += m_adjMatrix[candidatePath[pos2]][newRight2];

			delta = newSum - originalSum;

			// 如果交换后路径更短，则接受交换
			if (delta < 0) {
				currentPath = std::move(candidatePath);
				currentDistance += delta;
			}
			// 否则以一定概率接受（模拟退火策略）
			else {
				std::uniform_real_distribution<double> d(0.0, 1.0);
				if (d(rng) < std::exp(-delta / temperature)) {
					currentPath = std::move(candidatePath);
					currentDistance += delta;
				}
			}

			// 降低温度
			temperature *= COOLING_RATE;
		}

		// 在最后返回时重新计算一遍完整的路径长度，确保准确性
		int finalDistance = 0;
		for (size_t i = 0; i < currentPath.size() - 1; ++i) {
			finalDistance += m_adjMatrix[currentPath[i]][currentPath[i + 1]];
		}

		return {currentPath, finalDistance};
	}

	/**
    * @brief 使用遗传算法和局部搜索优化路径
    * 
    * @param start 起始点
    * @param end 结束点
    * @param populationSize 种群大小
    * @param generations 迭代次数
    * @return std::pair<std::vector<int>, int> 优化后的路径和总距离
    */
	[[nodiscard]] inline auto WGraph::geneticLocalSearchOptimization(
		int start, int end, int const population_size, int const generations) const -> std::pair<std::vector<int>, int>
	{
		if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices) {
			return {{}, -1};
		}

		// 初始化种群
		std::vector<std::vector<int>> population;
		for (int i = 0; i < population_size; ++i) {
			// 贪心初始化
			std::vector<int> path;
			path.reserve(m_vertices);
			path.push_back(start);
			std::vector<bool> visited(m_vertices, false);
			visited[start] = true;

			while (path.size() < static_cast<size_t>(m_vertices)) {
				int lastCity = path.back();
				int nextCity = -1;
				int minDistance = std::numeric_limits<int>::max();

				for (int city = 0; city < m_vertices; ++city) {
					if (!visited[city] && city != lastCity && m_adjMatrix[lastCity][city] != -1) {
						if (m_adjMatrix[lastCity][city] < minDistance) {
							minDistance = m_adjMatrix[lastCity][city];
							nextCity = city;
						}
					}
				}

				if (nextCity == -1) break;
				path.push_back(nextCity);
				visited[nextCity] = true;
			}

			if (!path.empty() && path.back() != end) {
				path.push_back(end);
			}
			population.push_back(path);
		}

		for (int gen = 0; gen < generations; ++gen) {
			// 交叉操作生成新种群
			std::vector<std::vector<int>> newPopulation;
			while (newPopulation.size() < population_size) {
				int parent1 = std::rand() % population_size;
				int parent2 = std::rand() % population_size;
				while (parent1 == parent2) parent2 = std::rand() % population_size;

				// 交叉操作
				std::vector<int> child;
				child.reserve(population[parent1].size());

				int startIdx = std::rand() % (population[parent1].size() - 1);
				int endIdx = std::rand() % (population[parent1].size() - startIdx) + startIdx;

				std::unordered_set<int> usedCities;
				for (int i = startIdx; i <= endIdx; ++i) {
					child.push_back(population[parent1][i]);
					usedCities.insert(population[parent1][i]);
				}

				for (int city : population[parent2]) {
					if (usedCities.find(city) == usedCities.end() && city != child.front() && city != child.
						back()) {
						child.push_back(city);
						usedCities.insert(city);
					}
				}

				newPopulation.push_back(child);
			}

			// 局部搜索优化
			for (auto& path : newPopulation) {
				// 2-opt局部搜索
				bool improved = true;
				while (improved) {
					improved = false;
					for (size_t i = 1; i < path.size() - 2; ++i) {
						for (size_t j = i + 1; j < path.size(); ++j) {
							if (j - i == 1) continue;

							std::vector<int> newPath = path;
							std::reverse(newPath.begin() + i, newPath.begin() + j);

							int currentDist = 0;
							for (size_t k = 0; k < path.size() - 1; ++k) {
								currentDist += m_adjMatrix[path[k]][path[k + 1]];
							}

							int newDist = 0;
							for (size_t k = 0; k < newPath.size() - 1; ++k) {
								newDist += m_adjMatrix[newPath[k]][newPath[k + 1]];
							}

							if (newDist < currentDist) {
								path = std::move(newPath);
								improved = true;
							}
						}
					}
				}
			}

			// 合并并选择优胜个体
			population.insert(population.end(), newPopulation.begin(), newPopulation.end());
			std::sort(population.begin(), population.end(), [this, start, end](const auto& a, const auto& b)
			{
				int distA = 0, distB = 0;
				for (size_t i = 0; i < a.size() - 1; ++i) distA += m_adjMatrix[a[i]][a[i + 1]];
				for (size_t i = 0; i < b.size() - 1; ++i) distB += m_adjMatrix[b[i]][b[i + 1]];
				return distA < distB;
			});
			population.resize(population_size);
		}

		// 找出最优路径
		auto bestPathIt = std::min_element(population.begin(), population.end(),
		                                   [this, start, end](const auto& a, const auto& b)
		                                   {
			                                   int distA = 0, distB = 0;
			                                   for (size_t i = 0; i < a.size() - 1; ++i)
				                                   distA += m_adjMatrix[a[i]][
					                                   a[i + 1]];
			                                   for (size_t i = 0; i < b.size() - 1; ++i)
				                                   distB += m_adjMatrix[b[i]][
					                                   b[i + 1]];
			                                   return distA < distB;
		                                   });
		std::vector<int> bestPath = *bestPathIt;
		int bestDistance = 0;
		for (size_t i = 0; i < bestPath.size() - 1; ++i) {
			bestDistance += m_adjMatrix[bestPath[i]][bestPath[i + 1]];
		}

		return {bestPath, bestDistance};
	}

	/* 打印 */
	/**
	 * @brief 打印图的结构。
	 */
	inline void WGraph::printGraph() const
	{
		std::cout << "带权重的图的邻接矩阵表示：\n";
		for (int i = 0; i < m_vertices; ++i) {
			for (int j = 0; j < m_vertices; ++j) {
				if (m_adjMatrix[i][j] == -1) {
					std::print("∞ ");
				}
				else {
					std::print("{} ", m_adjMatrix[i][j]);
				}
			}
			std::println();
		}
	}

	/**
	 * @brief 打印最短路径及其总距离。
	 * @param path 最短路径。
	 * @param distance 总距离。
	 */
	inline void WGraph::printPath(const std::vector<int>& path, int const distance) const
	{
		if (path.empty()) {
			std::println("未找到路径.");
			return;
		}

		std::print("最短路径为: ");
		for (size_t i = 0; i < path.size(); ++i) {
			if (auto it = m_vertexMap.find(path[i]);
				it != m_vertexMap.end()) {
				std::print("{}", it->second->m_name);
			}
			else {
				std::print("{}", path[i]);
			}
			if (i != path.size() - 1) {
				std::print(" -> ");
			}
		}
		std::println();
		std::println("总距离: {}", distance);
	}

	/* 文件 IO */
	/**
	 * @brief 从文件中读取图数据
	 * 
	 * 该函数尝试打开指定的文件，并解析其中的内容以构建图的顶点和边。
	 * 文件中的每一行代表一个顶点或一条边，格式如下：
	 * - 顶点：Vertex <名称> <ID> <位置X> <位置Y> [<属性>]
	 * - 边：Edge <源顶点ID> <目标顶点ID> <权重>
	 * 
	 * @param filename 文件路径
	 * @return true 如果文件成功打开并解析
	 * @return false 如果文件无法打开或解析过程中出现错误
	 */
	[[nodiscard]] inline bool WGraph::readFromFile(const std::string& filename)
	{
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "无法打开文件: " << filename << "\n";
			return false;
		}

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::vector<std::string> tokens;
			std::string token;
			while (iss >> token) {
				tokens.push_back(token);
			}

			if (tokens.empty()) {
				continue;
			}

			if (tokens[0] == "Vertex") {
				if (tokens.size() < 5) {
					std::cerr << "顶点格式错误: " << line << "\n";
					continue;
				}

				std::string const& name = tokens[1];
				IntType id = std::stoi(tokens[2]);
				int locationA = std::stoi(tokens[3]);
				int locationB = std::stoi(tokens[4]);
				auto attr = Attribute::Empty;
				if (tokens.size() >= 6) {
					int attrValue = std::stoi(tokens[5]);
					attr = static_cast<Attribute>(attrValue);
				}

				auto vertex = Object::create(name, id, {locationA, locationB}, attr);
				addVertex(vertex);
			}
			else if (tokens[0] == "Edge") {
				if (tokens.size() < 4) {
					std::cerr << "边格式错误: " << line << "\n";
					continue;
				}

				int src = std::stoi(tokens[1]);
				int dest = std::stoi(tokens[2]);
				int weight = std::stoi(tokens[3]);
				addEdge(src, dest, weight);
			}
		}

		file.close();
		return true;
	}

	/**
	 * @brief 将图数据写入文件
	 * 
	 * 该函数将当前图的所有顶点和边信息写入指定的文件。
	 * 文件格式与 readFromFile 函数读取的格式相同，便于后续读取和解析。
	 * 
	 * @param filename 文件路径
	 * @return true 如果文件成功打开并写入
	 * @return false 如果文件无法打开或写入过程中出现错误
	 */
	[[nodiscard]] inline bool WGraph::writeToFile(const std::string& filename) const
	{
		std::ofstream file(filename);
		if (!file.is_open()) {
			std::cerr << "无法打开文件: " << filename << "\n";
			return false;
		}

		// 写入顶点
		for (const auto& vertex : m_vertexMap | std::views::values) {
			file << std::format("Vertex {} {} {} {} {}\n",
			                    vertex->m_name, vertex->m_id, vertex->m_location.first, vertex->m_location.second,
			                    static_cast<int>(vertex->m_attr));
		}
		// 写入边
		for (int i = 0; i < m_vertices; ++i) {
			for (int j = i + 1; j < m_vertices; ++j) {
				if (m_adjMatrix[i][j] != -1) {
					file << std::format("Edge {} {} {}\n", i, j, m_adjMatrix[i][j]);
				}
			}
		}

		file.close();
		return true;
	}
}
