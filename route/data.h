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
	Edge(const int to, const int weight) : m_to(to), m_weight(weight)
	{
	}
};

// 定义图类
class WeightedAdjMatrixGraph
{
private:
	int m_vertices; // 顶点数
	int m_edges; // 边数
	vector<shared_ptr<Object>> m_vertexList; // 顶点列表，存储智能指针
	vector<vector<int>> m_adjMatrix; // 邻接矩阵，存储权重
public:
	// 构造函数
	explicit WeightedAdjMatrixGraph(int v) : m_vertices(v), m_edges(0)
	{
		// 初始化邻接矩阵，初始值为-1表示无边
		m_adjMatrix.resize(v, vector<int>(v, -1));
	}

	// 添加顶点
	void addVertex(const shared_ptr<Object>& vertex)
	{
		m_vertexList.push_back(vertex);
	}

	// 添加带权重的边
	void addEdge(const int src, const int dest, const int weight)
	{
		if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices && weight > 0) {
			m_adjMatrix[src][dest] = weight;
			m_adjMatrix[dest][src] = weight; // 如果是无向图
			m_edges++;
		}
	}

	// 打印图的结构
	void printGraph() const
	{
		cout << "带权重的图的邻接矩阵表示：" << endl;
		for (int i = 0; i < m_vertices; i++) {
			for (int j = 0; j < m_vertices; j++) {
				if (m_adjMatrix[i][j] == -1) {
					cout << "∞ "; // 无边时显示无穷大
				}
				else {
					cout << m_adjMatrix[i][j] << " ";
				}
			}
			cout << '\n';
		}
	}

	// 获取权重
	int getWeight(const int src, const int dest) const
	{
		if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices) {
			return m_adjMatrix[src][dest];
		}
		return -1; // 无效索引
	}

	// 获取顶点信息
	shared_ptr<Object> getVertex(const int index)
	{
		if (index >= 0 && index < m_vertices) {
			return m_vertexList[index];
		}
		return nullptr; // 无效索引
	}
};

NAMESPACE_ROUTE_END
