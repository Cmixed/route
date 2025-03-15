// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#include "pch.h"

using namespace std;

namespace route {

	/**
	 * @brief 定义加权邻接矩阵图的类
	 * 
	 * 该类用于表示图的加权邻接矩阵结构
	 */
	class WeightedAdjMatrixGraph;

	/**
	 * @brief 使用别名定义加权邻接矩阵图的类型
	 * 
	 * 通过别名WGraph，可以更方便地引用WeightedAdjMatrixGraph类
	 */
	using WGraph = WeightedAdjMatrixGraph;

	/**
	 * @brief 定义一个模板类，作为对象的基类
	 * 
	 * 该模板类接受一个类型参数T，用于定义对象的数据类型
	 * 
	 * @tparam T 模板参数，表示对象的数据类型
	 */
	template <typename T>
	class BaseObject;

	/**
	 * @brief 使用别名定义一个基于std::int_fast64_t类型的对象类
	 * 
	 * 通过别名Object，可以更方便地引用BaseObject<std::int_fast64_t>类型
	 */
	using Object = BaseObject<std::int_fast64_t>;



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
     * @brief 结构体，表示图中的边，包含目标顶点和权重。
     */
    struct Edge
    {
        int m_to;
        int m_weight;
        Edge(const int to, const int weight) : m_to(to), m_weight(weight) {}
    };

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