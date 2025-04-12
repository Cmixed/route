// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#include "pch.hpp"

#include "tool.hpp"

constexpr bool is_debug{false};

namespace route
{
	/// 别名声明
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

	/**
	 * @brief 基础物体类
	 * @tparam T 
	 */
	template <typename T>
	class BaseObject
	{
	public:
		std::string m_name{}; ///< name 物体的名称。
		T m_id{}; ///< id 物体的唯一标识符。
		std::pair<T, T> m_location{}; ///< location 物体的位置。
		Attribute m_attr{Attribute::Empty}; ///< attr 物体的属性。

		/// 构造函数列表
		explicit(true) BaseObject(std::string name, T id, std::pair<T, T> location, Attribute const attr)
			: m_name(std::move(name)), m_id(id), m_location(std::move(location)), m_attr(attr)
		{
		}

		explicit(true) BaseObject(std::string name, T id, std::pair<T, T> location)
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
		static auto create(std::string name, T id, std::pair<T, T> location,
		                   Attribute attr = Attribute::Empty)
			-> std::shared_ptr<BaseObject<T>>
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

		/* 基础方法 */
		void addVertex(const std::shared_ptr<Object>& vertex);
		template <typename... Args>
		void addVertices(Args&&... vertices);
		void addEdge(int const src, int const dest, int const weight);
		[[nodiscard]] int getWeight(int const src, int const dest) const;
		[[nodiscard]] auto getVertex(int const id) const->std::shared_ptr<Object>;

		/* 路径算法 */
		[[nodiscard]] auto dijkstra(int const start, int const end)
		const -> std::pair<std::vector<int>, int>;
		[[nodiscard]] auto geneticAlgorithm(int const start, int const end)
		const -> std::pair<std::vector<int>, int>;
		[[nodiscard]] auto localSearchOptimization(int const start, int const end)
		const -> std::pair<std::vector<int>, int>;
		[[nodiscard]] auto geneticLocalSearchOptimization(int start, int end, int const populationSize = 50,
		                                                  int const generations = 100)
		const -> std::pair<std::vector<int>, int>;

		/* 打印 */
		void printGraph() const;
		void printPath(const std::vector<int>& path, int const distance) const;
		

		/* 文件 IO */
		[[nodiscard]] bool readFromFile(const std::string& filename);
		[[nodiscard]] bool writeToFile(const std::string& filename) const;
	};
};
