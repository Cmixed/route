// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#define NAMESPACE_ROUTE_BEGIN namespace route {
#define NAMESPACE_ROUTE_END }

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>
#include <queue>
#include <limits>
#include <vector>
#include <ranges>
#include <functional>
#include <print>
#include <format>

using namespace std;

NAMESPACE_ROUTE_BEGIN
// City number


// Define the attribute of the object
enum class Attribute : std::uint8_t
{
    Empty = 0,
    Place,
    Supply,
    Occupied,
};

/**
 * @brief 基本物体  1: 名称 2: 位置 3: 属性
 * @tparam T 
 */
template <typename T>
class BaseObject
{
public:
    // base information
    std::string m_name{};
    std::pair<T, T> m_location{};
    Attribute m_attr{Attribute::Empty};
};

using Object = BaseObject<int>;

// 定义图的边结构
struct Edge
{
    int m_to; // 边的目标顶点
    int m_weight; // 边的权重
    Edge(const int to, const int weight) : m_to(to), m_weight(weight) {}
};

// 定义图类
class WeightedAdjMatrixGraph
{
private:
    int m_vertices; // 顶点数
    int m_edges; // 边数
    std::vector<std::shared_ptr<Object>> m_vertexList; // 顶点列表，存储智能指针
    std::vector<std::vector<int>> m_adjMatrix; // 邻接矩阵，存储权重

public:
    // 构造函数
    explicit WeightedAdjMatrixGraph(int const v) : m_vertices(v), m_edges(0)
    {
        // 初始化邻接矩阵，初始值为-1表示无边
        m_adjMatrix.resize(v, std::vector<int>(v, -1));
    }

    // 添加顶点
    void addVertex(const std::shared_ptr<Object>& vertex)
    {
        if (vertex != nullptr)
        {
            m_vertexList.push_back(vertex);
        }
    }

    // 添加带权重的边
    void addEdge(int const src, int const dest, int const weight)
    {
        if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices && weight > 0)
        {
            m_adjMatrix[src][dest] = weight;
            m_adjMatrix[dest][src] = weight; // 如果是无向图
            m_edges++;
        }
    }

    // 打印图的结构
    void printGraph() const
    {
        std::cout << "带权重的图的邻接矩阵表示：\n";
        for (int i = 0; i < m_vertices; ++i)
        {
            for (int j = 0; j < m_vertices; ++j)
            {
                if (m_adjMatrix[i][j] == -1)
                {
                    std::cout << "∞ ";
                }
                else
                {
                    std::cout << m_adjMatrix[i][j] << " ";
                }
            }
            std::cout << '\n';
        }
    }

    // 获取权重
    int getWeight(int const src, int const dest) const
    {
        if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices)
        {
            return m_adjMatrix[src][dest];
        }
        return -1; // 无效索引
    }

    // 获取顶点信息
    std::shared_ptr<Object> getVertex(int const index) const
    {
        if (index >= 0 && index < m_vertices)
        {
            return m_vertexList[index];
        }
        return nullptr; // 无效索引
    }

    // Dijkstra算法计算最短路径
    [[nodiscard]] std::pair<std::vector<int>, int> dijkstra(int const start, int const end) const
    {
        if (start < 0 || start >= m_vertices || end < 0 || end >= m_vertices)
        {
            return {{}, -1}; // 无效的起始或结束点
        }

        // 使用优先队列（最小堆）来存储当前最短距离的节点
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;
        // 距离数组，初始化为无穷大
        std::vector<int> dist(m_vertices, std::numeric_limits<int>::max());
        // 前驱节点数组，用于记录路径
        std::vector<int> prev(m_vertices, -1);

		dist[start] = 0;
		pq.emplace(0, start);

        while (!pq.empty())
        {
            int const u = pq.top().second;
            pq.pop();

            if (u == end)
            {
                break; // 已经找到终点，可以提前退出
            }

            for (int v = 0; v < m_vertices; ++v)
            {
                if (int const weight = m_adjMatrix[u][v];
                    weight != -1) // 如果有边
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

        // 如果距离仍然为无穷大，说明无法到达
        if (dist[end] == std::numeric_limits<int>::max())
        {
            return {{}, -1};
        }

        // 重建路径
        std::vector<int> path;
        for (int at = end; at != -1; at = prev[at])
        {
            path.push_back(at);
        }
		ranges::reverse(path);

        return {path, dist[end]};
    }

    // 打印路径
    void printPath(const std::vector<int>& path, int distance) const
    {
        if (path.empty())
        {
            std::cout << "No path found.\n";
            return;
        }

        std::cout << "Shortest path: ";
        for (size_t i = 0; i < path.size(); ++i)
        {
            std::cout << path[i];
            if (i != path.size() - 1)
            {
                std::cout << " -> ";
            }
        }
        std::println();
        std::println("Total distance: {}", distance);
    }
};

NAMESPACE_ROUTE_END
