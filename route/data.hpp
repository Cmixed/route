// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#include "pch.hpp"

namespace route {

	using VertexId = std::int_fast32_t;
    using Weight = std::int_fast32_t;
    using Path = std::vector<VertexId>;
    using AdjMatrix = std::vector<std::vector<Weight>>;
    using WGraph = class WeightedAdjMatrixGraph;

    template <typename T> class BaseObject;
    using Object = BaseObject<std::int_fast64_t>;

    /**
     * @brief 结构体，表示图中的边，包含目标顶点和权重。
     */
    struct Edge
    {
        int m_to;
        int m_weight;
        Edge(const int to, const int weight) : m_to(to), m_weight(weight) {}
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

    /**
     * @brief 模板类，表示一个基本物体，包含名称、位置和属性。
     * @tparam T 位置坐标的类型。
     */
    template <typename T>
    class BaseObject
    {
    public:
        std::string m_name{};
        std::pair<T, T> m_location{};
        Attribute m_attr{ Attribute::Empty };

        /**
         * @brief 构造函数，初始化物体的名称、位置和属性。
         * @param name 物体的名称。
         * @param location 物体的位置。
         * @param attr 物体的属性。
         */
        explicit BaseObject(std::string name, std::pair<T, T> location, Attribute attr)
            : m_name(std::move(name)), m_location(std::move(location)), m_attr(attr) {
        }

        /**
         * @brief 构造函数，初始化物体的名称和位置，属性使用默认值。
         * @param name 物体的名称。
         * @param location 物体的位置。
         */
        explicit BaseObject(std::string name, std::pair<T, T> location)
            : m_name(std::move(name)), m_location(std::move(location)) {
        }

        /**
         * @brief 静态工厂函数，创建一个 BaseObject 对象并返回智能指针。
         * @param name 物体的名称。
         * @param location 物体的位置。
         * @param attr 物体的属性。
         * @return 创建的 BaseObject 对象的智能指针。
         */
        static std::shared_ptr<BaseObject<T>> create(std::string name, std::pair<T, T> location, Attribute attr = Attribute::Empty)
        {
            return std::make_shared<BaseObject<T>>(std::move(name), std::move(location), attr);
        }
    };

    /**
     * @brief 计算路径总距离
     * @param path 路径
     * @param adj_matrix 邻接矩阵
     * @return 路径总距离
     */
    inline int calculate_path_distance(const Path& path, const AdjMatrix& adj_matrix) {
        int distance = 0;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            distance += adj_matrix[path[i]][path[i + 1]];
        }
        return distance;
    }

    /**
     * @brief 检查路径是否合法
     * @param path 路径
     * @param adj_matrix 邻接矩阵
     * @return 是否合法
     */
    inline bool is_valid_path(const Path& path, const AdjMatrix& adj_matrix) {
        for (size_t i = 0; i < path.size() - 1; ++i) {
            if (adj_matrix[path[i]][path[i + 1]] == -1) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief 初始化种群
     * @param start 起点
     * @param end 终点
     * @param num_vertices 顶点数
     * @param population_size 种群大小
     * @return 初始化的种群
     */
    inline std::vector<Path> initialize_population(VertexId start, VertexId end, VertexId num_vertices, int population_size) {
        std::vector<Path> population;
        std::vector<VertexId> nodes;
        for (VertexId i = 0; i < num_vertices; ++i) {
            if (i != start && i != end) {
                nodes.push_back(i);
            }
        }

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<> dist(0, nodes.size() - 1);

        for (int i = 0; i < population_size; ++i) {
            std::shuffle(nodes.begin(), nodes.end(), rng);
            Path path = {start};
            path.insert(path.end(), nodes.begin(), nodes.end());
            path.push_back(end);
            population.push_back(path);
        }

        return population;
    }

    /**
     * @brief 轮盘赌选择
     * @param population 种群
     * @param adj_matrix 邻接矩阵
     * @param rng 随机数生成器
     * @return 选择的路径
     */
    inline Path select(const std::vector<Path>& population, const AdjMatrix& adj_matrix, std::mt19937& rng) {
        std::vector<double> fitnessScores;
        double totalFitness = 0.0;

        for (const auto& path : population) {
            if (!is_valid_path(path, adj_matrix)) {
                fitnessScores.push_back(0.0);
            } else {
                int const distance = calculate_path_distance(path, adj_matrix);
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

    /**
     * @brief 部分映射交叉（PMX）
     * @param parent1 父代1
     * @param parent2 父代2
     * @param rng 随机数生成器
     * @return 交叉后的子代
     */
    inline Path crossover(const Path& parent1, const Path& parent2, std::mt19937& rng) {
        Path child(parent1.size(), -1);
        std::uniform_int_distribution<> dist(0, static_cast<int>(parent1.size()) - 1);

        // 生成 start 和 end
        int start = dist(rng);
        int end = dist(rng);
        if (start > end) {
            std::swap(start, end);
        }

        // 复制父代1的子段
        for (int i = start; i <= end; ++i) {
            child[i] = parent1[i];
        }

        // 填充父代2的剩余部分
        size_t insertPos{ 0 }; // 记录插入位置
		for (const auto& elem : parent2) {
		    // 检查 elem 是否已经在 child 中
		    if (std::ranges::find(child, elem) == child.end()) {
		        // 找到 child 中第一个值为 -1 的位置
		        while (insertPos < child.size() && child[insertPos] != -1) {
		            ++insertPos;
		        }
		        // 如果找到有效位置，插入 elem
		        if (insertPos < child.size()) {
		            child[insertPos] = elem;
		        }
		    }
		}

        return child;
    }

    /**
     * @brief 变异操作
     * @param path 路径
     * @param rng 随机数生成器
     */
    inline void mutate(Path& path, std::mt19937& rng) {
        if (path.size() < 3) return;

        std::uniform_int_distribution<> dist(1, static_cast<int>(path.size()) - 2);
        int i = dist(rng);
        int j = dist(rng);
        if (i != j) {
            std::swap(path[i], path[j]);
        }
    }

    /**
     * @brief 类，表示一个带权邻接矩阵图。
     */
    class WeightedAdjMatrixGraph
    {
    private:
        int m_vertices;
        int m_edges;
        std::vector<std::shared_ptr<Object>> m_vertexList;
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
            if (vertex != nullptr)
            {
                m_vertexList.push_back(vertex);
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
            if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices && weight > 0)
            {
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
            for (int i = 0; i < m_vertices; ++i)
            {
                for (int j = 0; j < m_vertices; ++j)
                {
                    if (m_adjMatrix[i][j] == -1)
                    {
						std::print("∞ ");
                    }
                    else
                    {
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
            if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices)
            {
                return m_adjMatrix[src][dest];
            }
            return -1;
        }

        /**
         * @brief 获取指定索引的顶点信息。
         * @param index 顶点的索引。
         * @return 顶点信息，如果索引无效则返回nullptr。
         */
        [[nodiscard]] std::shared_ptr<Object> getVertex(int const index) const
        {
            if (index >= 0 && index < m_vertices)
            {
                return m_vertexList[index];
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
            if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices)
            {
                return { {}, -1 };
            }

            std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
            std::vector<int> dist(m_vertices, std::numeric_limits<int>::max());
            std::vector<int> prev(m_vertices, -1);

            dist[start] = 0;
            pq.emplace(0, start);

            while (!pq.empty())
            {
                int const u = pq.top().second;
                pq.pop();

                if (u == end)
                {
                    break;
                }

                for (int v = 0; v < m_vertices; ++v)
                {
                    if (int const weight = m_adjMatrix[u][v];
                        weight != -1)
                    {
                        if (dist[v] > dist[u] + weight)
                        {
                            dist[v] = dist[u] + weight;
                            prev[v] = u;
                            pq.emplace(dist[v], v);
                        }
                    }
                }
            }

            if (dist[end] == std::numeric_limits<int>::max())
            {
                return { {}, -1 };
            }

            std::vector<int> path;
            for (int at = end; at != -1; at = prev[at])
            {
                path.push_back(at);
            }
            std::ranges::reverse(path);

            return { path, dist[end] };
        }

        /**
		 * @brief 使用遗传算法计算最短路径（改进版）
		 * @param start 起点
		 * @param end 终点
		 * @return 最短路径和距离
		 */
		[[nodiscard]] std::pair<std::vector<int>, int> geneticAlgorithm(int const start, int const end) const {
		    if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices) {
		        return { {}, -1 };
		    }

		    std::random_device rd;
		    std::mt19937 rng(rd());

		    constexpr int POPULATION_SIZE = 500;
		    constexpr int MAX_GENERATIONS = 500;
		    constexpr double CROSSOVER_RATE = 0.85;
		    constexpr double MUTATION_RATE = 0.2;
		    constexpr int ELITE_SIZE = 0;

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
		            } else {
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
		        return { {}, -1 };
		    }

		    return { bestPath, bestDistance };
		}


        /**
         * @brief 打印最短路径及其总距离。
         * @param path 最短路径。
         * @param distance 总距离。
         */
        static void printPath(const std::vector<int>& path, int const distance)
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
    };


}