// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#include "pch.hpp"

namespace route
{
	using IntType = std::int_fast32_t;
	using VertexId = std::int_fast32_t;
	using Weight = std::int_fast32_t;
	using Path = std::vector<VertexId>;
	using AdjMatrix = std::vector<std::vector<Weight>>;
	using WGraph = class WeightedAdjMatrixGraph;

	template <typename T>
	class BaseObject;
	using Object = BaseObject<std::int_fast64_t>;

	/**
	 * @brief 结构体，表示图中的边，包含目标顶点和权重。
	 */
	struct Edge
	{
		int m_to;
		int m_weight;

		Edge(const int to, const int weight) : m_to(to), m_weight(weight)
		{
		}
	};

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
		explicit BaseObject(std::string name, T id, std::pair<T, T> location, Attribute attr)
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
		std::map<int, std::shared_ptr<Object>> m_vertexMap; // 使用 map 存储顶点，键为顶点的 id
		std::vector<std::vector<int>> m_adjMatrix;

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
		 * @brief 批量添加顶点到图中。
		 * @param vertices 可变参数模板，用于传递多个顶点。
		 */
		template <typename... Args>
		void addVertices(Args&&... vertices)
		{
			(addVertex(std::forward<Args>(vertices)), ...);
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
						std::cout << "∞ ";
					}
					else {
						std::cout << m_adjMatrix[i][j] << " ";
					}
				}
				std::cout << std::endl;
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
		 * @param index 顶点的索引。
		 * @return 顶点信息，如果索引无效则返回nullptr。
		 */
		[[nodiscard]] std::shared_ptr<Object> getVertex(int const id) const
		{
			auto it = m_vertexMap.find(id);
			if (it != m_vertexMap.end()) {
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

			std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<
				                    int, int>>> pq;
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
			std::reverse(path.begin(), path.end());

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
			if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices) {
				return {{}, -1};
			}

			std::random_device rd;
			std::mt19937 rng(rd());

			constexpr int POPULATION_SIZE = 500;
			constexpr int MAX_GENERATIONS = 500;
			constexpr double CROSSOVER_RATE = 0.85;
			constexpr double MUTATION_RATE = 0.2;
			constexpr int ELITE_SIZE = 5;

			std::vector<Path> population = initialize_population(start, end, m_vertices, POPULATION_SIZE);

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
					bestDistanceInGeneration = std::min(bestDistanceInGeneration, distances[i]);
				}


				std::print("\rGeneration: {} / {}, Best Distance: {}",
				           generation + 1, MAX_GENERATIONS, bestDistanceInGeneration);

				// 精英保留
				std::vector<std::pair<int, const Path*>> elitePaths;
				for (size_t i = 0; i < population.size(); ++i) {
					if (distances[i] != -1) {
						elitePaths.emplace_back(distances[i], &population[i]);
					}
				}

				std::ranges::sort(elitePaths, [](const auto& a, const auto& b) { return a.first < b.first; });

				for (int i = 0; i < ELITE_SIZE && i < static_cast<int>(elitePaths.size()); ++i) {
					newPopulation.push_back(*elitePaths[i].second);
				}

				// 选择、交叉和变异
				while (newPopulation.size() < POPULATION_SIZE) {
					Path parent1 = select(population, m_adjMatrix, rng);
					Path parent2 = select(population, m_adjMatrix, rng);

					if (std::uniform_real_distribution<>(0.0, 1.0)(rng) < CROSSOVER_RATE) {
						Path child = crossover(parent1, parent2, rng);
						newPopulation.push_back(child);
					}
					else {
						newPopulation.push_back(parent1);
					}

					if (std::uniform_real_distribution<>(0.0, 1.0)(rng) < MUTATION_RATE) {
						mutate(newPopulation.back(), rng);
					}
				}

				population = std::move(newPopulation);
			}

			// 找到最优路径
			Path bestPath;
			int bestDistance = std::numeric_limits<int>::max();

			for (const auto& path : population) {
				if (int const distance = calculate_path_distance(path, m_adjMatrix);
					is_valid_path(path, m_adjMatrix)) {
					if (distance < bestDistance) {
						bestDistance = distance;
						bestPath = path;
					}
				}
			}

			if (bestDistance == std::numeric_limits<int>::max()) {
				return {{}, -1};
			}

			return {bestPath, bestDistance};
		}

		/**
         * @brief 打印最短路径及其总距离。
         * @param path 最短路径。
         * @param distance 总距离。
         */
        static void printPurePath(const std::vector<int>& path, int const distance)
        {
            if (path.empty())
            {
                std::println("No path found.");
                return;
            }

            std::print("Shortest path: ");
            for (size_t i = 0; i < path.size(); ++i)
            {
                std::cout << path[i];
                if (i != path.size() - 1)
                {
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
	        if (path.empty())
	        {
	            std::cout << "No path found." << std::endl;
	            return;
	        }

	        std::cout << "Shortest path: ";
	        for (size_t i = 0; i < path.size(); ++i)
	        {
	            auto it = m_vertexMap.find(path[i]);
	            if (it != m_vertexMap.end())
	            {
	                std::cout << it->second->m_name;
	            }
	            else
	            {
	                std::cout << path[i];
	            }
	            if (i != path.size() - 1)
	            {
	                std::cout << " -> ";
	            }
	        }
	        std::cout << std::endl;
	        std::cout << "Total distance: " << distance << std::endl;
	    }


		// 新增文件读取函数
		bool readFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "无法打开文件: " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (iss >> token)
            {
                tokens.push_back(token);
            }

            if (tokens.empty())
            {
                continue;
            }

            if (tokens[0] == "Vertex")
            {
                if (tokens.size() < 5)
                {
                    std::cerr << "顶点格式错误: " << line << std::endl;
                    continue;
                }

                std::string name = tokens[1];
                int id = std::stoi(tokens[2]);
                int locationA = std::stoi(tokens[3]);
                int locationB = std::stoi(tokens[4]);
                Attribute attr = Attribute::Empty;
                if (tokens.size() >= 6)
                {
                    attr = static_cast<Attribute>(std::stoi(tokens[5]));
                }

                auto vertex = Object::create(name, id, {locationA, locationB}, attr);
                addVertex(vertex);
            }
            else if (tokens[0] == "Edge")
            {
                if (tokens.size() < 4)
                {
                    std::cerr << "边格式错误: " << line << std::endl;
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

		// 新增文件写入函数
		bool writeToFile(const std::string& filename) const
		{
			std::ofstream file(filename);
			if (!file.is_open()) {
				std::cerr << "无法打开文件: " << filename << std::endl;
				return false;
			}

			// 写入顶点
			for (const auto& pair : m_vertexMap) {
				const auto& vertex = pair.second;
				file << "Vertex " << vertex->m_name << " " << vertex->m_id << " "
					<< vertex->m_location.first << " " << vertex->m_location.second << " "
					<< static_cast<int>(vertex->m_attr) << std::endl;
			}

			// 写入边
			for (int i = 0; i < m_vertices; ++i) {
				for (int j = i + 1; j < m_vertices; ++j) {
					if (m_adjMatrix[i][j] != -1) {
						file << "Edge " << i << " " << j << " " << m_adjMatrix[i][j] << std::endl;
					}
				}
			}

			file.close();
			return true;
		}

	private:
		static std::vector<std::vector<int>> initialize_population(int start, int end, int vertices, int populationSize)
		{
			std::vector<std::vector<int>> population;
			std::vector<int> nodes;
			for (int i = 0; i < vertices; ++i) {
				if (i != start && i != end) {
					nodes.push_back(i);
				}
			}

			std::random_device rd;
			std::mt19937 rng(rd());
			std::uniform_int_distribution<> dist(0, nodes.size() - 1);

			for (int i = 0; i < populationSize; ++i) {
				std::shuffle(nodes.begin(), nodes.end(), rng);
				std::vector<int> path = {start};
				path.insert(path.end(), nodes.begin(), nodes.end());
				path.push_back(end);
				population.push_back(path);
			}

			return population;
		}

		static bool is_valid_path(const std::vector<int>& path, const std::vector<std::vector<int>>& adjMatrix)
		{
			for (size_t i = 0; i < path.size() - 1; ++i) {
				if (adjMatrix[path[i]][path[i + 1]] == -1) {
					return false;
				}
			}
			return true;
		}

		static int calculate_path_distance(const std::vector<int>& path, const std::vector<std::vector<int>>& adjMatrix)
		{
			int distance = 0;
			for (size_t i = 0; i < path.size() - 1; ++i) {
				distance += adjMatrix[path[i]][path[i + 1]];
			}
			return distance;
		}

		static std::vector<int> select(const std::vector<std::vector<int>>& population,
		                               const std::vector<std::vector<int>>& adjMatrix, std::mt19937& rng)
		{
			std::vector<double> fitnessScores;
			double totalFitness = 0.0;

			for (const auto& path : population) {
				if (!is_valid_path(path, adjMatrix)) {
					fitnessScores.push_back(0.0);
				}
				else {
					int const distance = calculate_path_distance(path, adjMatrix);
					fitnessScores.push_back(1.0 / (distance + 1));
					totalFitness += 1.0 / (distance + 1);
				}
			}

			double const target = std::uniform_real_distribution<double>(0.0, totalFitness)(rng);
			double cumulative = 0.0;
			for (size_t i = 0; i < population.size(); ++i) {
				cumulative += fitnessScores[i];
				if (cumulative >= target) {
					return population[i];
				}
			}

			return population.back();
		}

		static std::vector<int> crossover(const std::vector<int>& parent1, const std::vector<int>& parent2,
		                                  std::mt19937& rng)
		{
			std::vector<int> child(parent1.size(), -1);
			std::uniform_int_distribution<> dist(0, static_cast<int>(parent1.size()) - 1);

			int start = dist(rng);
			int end = dist(rng);
			if (start > end) {
				std::swap(start, end);
			}

			for (int i = start; i <= end; ++i) {
				child[i] = parent1[i];
			}

			size_t insertPos = 0;
			for (const auto& elem : parent2) {
				if (std::find(child.begin(), child.end(), elem) == child.end()) {
					while (insertPos < child.size() && child[insertPos] != -1) {
						++insertPos;
					}
					if (insertPos < child.size()) {
						child[insertPos] = elem;
					}
				}
			}

			return child;
		}

		static void mutate(std::vector<int>& path, std::mt19937& rng)
		{
			if (path.size() < 3) {
				return;
			}

			std::uniform_int_distribution<> dist(1, static_cast<int>(path.size()) - 2);
			int i = dist(rng);
			int j = dist(rng);
			if (i != j) {
				std::swap(path[i], path[j]);
			}
		}


		





	};
};
