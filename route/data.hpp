// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#include "pch.hpp"

#include "tool.hpp"

constexpr bool is_debug{false};

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
		static std::shared_ptr<BaseObject<T>>
		create(std::string name, T id, std::pair<T, T> location,
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
		void printGraph() const;

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

		// 路径算法
		[[nodiscard]] auto dijkstra(int const start, int const end) const->std::pair<std::vector<int>, int>;
		[[nodiscard]] auto geneticAlgorithm(int const start, int const end) const->std::pair<std::vector<int>, int>;
		[[nodiscard]] auto localSearchOptimization(int const start, int const end) const->std::pair<std::vector<int>, int>;
		[[nodiscard]] auto geneticLocalSearchOptimization(
			int start, int end, int const populationSize = 50, int const generations = 100) const->std::pair<std::vector<int>, int>;

		// 路径打印
		void printPurePath(const std::vector<int>& path, int const distance);
		void printPath(const std::vector<int>& path, int const distance) const;

		// 文件 IO
		[[nodiscard]] bool readFromFile(const std::string& filename);
		[[nodiscard]] bool writeToFile(const std::string& filename) const;
	};
};
