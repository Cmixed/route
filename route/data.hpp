// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#include "pch.hpp"

namespace route
{
	using IntType = std::int_fast32_t;
	using VertexId = IntType;
	using Weight = IntType;
	using Path = std::vector<VertexId>;
	using AdjMatrix = std::vector<std::vector<Weight>>;
	using WGraph = class WeightedAdjMatrixGraph;

	template <typename T>
	class BaseObject;
	using Object = BaseObject<IntType>;

	/**
	 * @brief 枚举类，表示物体的属性。
	 */
	enum class Attribute : std::uint8_t
	{
		Empty = 0, ///< 无特定属性。
		Place, ///< 表示一个位置。
		Supply, ///< 表示一个供给点。
		Occupied, ///< 表示被占用状态, 如被一个目标访问。
	};

	template <typename T>
	class BaseObject
	{
	public:
		std::string m_name{};
		T m_id{};
		std::pair<T, T> m_location{};
		Attribute m_attr{Attribute::Empty};

		/**
		 * @brief 构造函数，初始化物体的名称、id、位置和属性。
		 * @param name 物体的名称。
		 * @param id 物体的唯一标识符。
		 * @param location 物体的位置。
		 * @param attr 物体的属性。
		 */
		explicit BaseObject(std::string name, T id, std::pair<T, T> location, Attribute const attr)
			: m_name(std::move(name)), m_id(id), m_location(std::move(location)), m_attr(attr)
		{
		}

		/**
		 * @brief 构造函数，初始化物体的名称、id和位置，属性使用默认值。
		 * @param name 物体的名称。
		 * @param id 物体的唯一标识符。
		 * @param location 物体的位置。
		 */
		explicit BaseObject(std::string name, T id, std::pair<T, T> location)
			: m_name(std::move(name)), m_id(id), m_location(std::move(location))
		{
		}

		/**
		 * @brief 静态工厂函数，创建一个 BaseObject 对象并返回智能指针。
		 * @param name 物体的名称。
		 * @param id 物体的唯一标识符。
		 * @param location 物体的位置。
		 * @param attr 物体的属性。
		 * @return 创建的 BaseObject 对象的智能指针。
		 */
		static std::shared_ptr<BaseObject<T>> create(std::string name, T id, std::pair<T, T> location,
		                                             Attribute attr = Attribute::Empty)
		{
			return std::make_shared<BaseObject<T>>(std::move(name), id, std::move(location), attr);
		}
	};


	/**
	 * @brief 类，表示一个带权邻接矩阵图。
	 */
	class WeightedAdjMatrixGraph
	{
	private:
		IntType m_vertices;
		IntType m_edges;
		std::map<IntType, std::shared_ptr<Object>> m_vertexMap; // 使用 map 存储顶点，键为顶点的 id
		std::vector<std::vector<IntType>> m_adjMatrix;

	public:
		/**
		 * @brief 构造一个新的 WeightedAdjMatrixGraph 对象。
		 * @param v 图中的顶点数。
		 */
		explicit WeightedAdjMatrixGraph(int const v) : m_vertices(v), m_edges(0)
		{
			m_adjMatrix.resize(v, std::vector<int>(v, -1));
		}

		/**
		 * @brief 添加一个顶点到图中。
		 * @param vertex 要添加的顶点。
		 */
		void addVertex(const std::shared_ptr<Object>& vertex)
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
		void addVertices(Args&&... vertices)
		{
			(addVertex(std::forward<Args>(vertices)), ...);
		}


		/**
		 * @brief 添加一条带权重的边到图中。
		 * @param src 起始顶点。
		 * @param dest 目标顶点。
		 * @param weight 边的权重。
		 */
		void addEdge(int const src, int const dest, int const weight)
		{
			if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices && weight > 0) {
				m_adjMatrix[src][dest] = weight;
				m_adjMatrix[dest][src] = weight;
				m_edges++;
			}
		}

		/**
		 * @brief 打印图的结构。
		 */
		void printGraph() const
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
		 * @brief 获取两个顶点之间的边的权重。
		 * @param src 起始顶点。
		 * @param dest 目标顶点。
		 * @return 边的权重，如果顶点无效则返回-1。
		 */
		[[nodiscard]] int getWeight(int const src, int const dest) const
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
		[[nodiscard]] std::shared_ptr<Object> getVertex(int const id) const
		{
			if (auto const it = m_vertexMap.find(id);
				it != m_vertexMap.end()) {
				return it->second;
			}
			return nullptr;
		}


		/**
		 * @brief 使用 Dijkstra 算法计算两个顶点之间的最短路径。
		 * @param start 起始顶点。
		 * @param end 结束顶点。
		 * @return 一个包含最短路径和总距离的元组。如果找不到路径，距离为-1。
		 */
		[[nodiscard]] std::pair<std::vector<int>, int> dijkstra(int const start, int const end) const
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
		[[nodiscard]] std::pair<std::vector<int>, int> geneticAlgorithm(int const start, int const end) const
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

				std::print("\rGeneration: {} / {}, Best Distance: {}",
				           generation + 1, MAX_GENERATIONS, bestDistanceInGeneration);
			}

			std::println();

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
		[[nodiscard]] std::pair<std::vector<int>, int> localSearchOptimization(int const start, int const end) const
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
		 * @brief 打印最短路径及其总距离。
		 * @param path 最短路径。
		 * @param distance 总距离。
		 */
		static void printPurePath(const std::vector<int>& path, int const distance)
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
			std::println();
			std::println("Total distance: {}", distance);
		}


		/**
		 * @brief 打印最短路径及其总距离。
		 * @param path 最短路径。
		 * @param distance 总距离。
		 */
		void printPath(const std::vector<int>& path, int const distance)
		{
			if (path.empty()) {
				std::println("No path found.");
				return;
			}

			std::print("Shortest path: ");
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
			std::println("Total distance: {}", distance);
		}


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
		[[nodiscard]] bool readFromFile(const std::string& filename)
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
		[[nodiscard]] bool writeToFile(const std::string& filename) const
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

	private:
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
		static std::vector<std::vector<int>> initialize_population(int const start, int const end, int const vertices,
		                                                           int const population_size)
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
		 * @brief 检查路径是否有效
		 * 
		 * 该函数检查给定的路径是否在图中有效。路径有效是指路径中每两个相邻节点之间都有边连接。
		 * 
		 * @param path 要检查的路径，表示为节点序列
		 * @param adj_matrix 图的邻接矩阵，adj_matrix[i][j] 为 -1 表示节点 i 和 j 之间没有边
		 * @return true 如果路径有效
		 * @return false 如果路径无效
		 */
		static bool is_valid_path(const std::vector<int>& path, const std::vector<std::vector<int>>& adj_matrix)
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
		static int calculate_path_distance(const std::vector<int>& path,
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
		static std::vector<int> select(const std::vector<std::vector<int>>& population,
		                               const std::vector<std::vector<int>>& adj_matrix, std::mt19937& rng)
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
		static std::vector<int> crossover(const std::vector<int>& parent1, const std::vector<int>& parent2,
		                                  std::mt19937& rng)
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
		static void mutate(std::vector<int>& path, std::mt19937& rng)
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
	};
};
