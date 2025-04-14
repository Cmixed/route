#include "file_io.hpp"
#include "menu.hpp"

namespace route
{
	/*****************************************************************
	 *
	 *		IO 函数实现
	 *
	 *****************************************************************/
	/**
	 * @brief 从文件中读取图数据
	 * 
	 * 该函数尝试打开指定的文件，并解析其中的内容以构建图的顶点和边。
	 * 文件中的每一行代表一个顶点或一条边，格式如下：
	 * - 顶点：Vertex <名称> <ID> <位置X> <位置Y> [<属性>]
	 * - 边：Edge <源顶点ID> <目标顶点ID> <权重>
	 * @param graph 图
	 * @param filename 文件路径
	 * @return true 如果文件成功打开并解析
	 * @return false 如果文件无法打开或解析过程中出现错误
	 */
	bool read_from_file(WeightedAdjMatrixGraph& graph, const std::string& filename)
	{
	    std::ifstream file(filename);
	    if (!file.is_open()) {
	        std::cerr << "无法打开文件: " << filename << "\n";
	        return false;
	    }

	    std::string line;
	    while (std::getline(file, line)) {
	        // 跳过空行和注释行
	        if (line.empty() || line[0] == '#') {
	            continue;
	        }

	        // 去掉行首的空白字符（如果有的话）
	        size_t start = line.find_first_not_of(" \t");
	        if (start == std::string::npos) {
	            continue;  // 空行或全是空白字符的行
	        }
	        line = line.substr(start);

	        std::istringstream iss(line);
	        std::vector<std::string> tokens;
	        std::string token;
	        while (iss >> token) {
	            tokens.push_back(token);
	        }

	        if (tokens.empty()) {
	            continue;
	        }

	        if (tokens[0] == "[Vertex]") {
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
	            graph.addVertex(vertex);
	        }
	        else if (tokens[0] == "[Edge]") {
	            if (tokens.size() < 4) {
	                std::cerr << "边格式错误: " << line << "\n";
	                continue;
	            }

	            int src = std::stoi(tokens[1]);
	            int dest = std::stoi(tokens[2]);
	            int weight = std::stoi(tokens[3]);
	            graph.addEdge(src, dest, weight);
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
	 * @param graph 图
	 * @param filename 文件路径
	 * @return true 如果文件成功打开并写入
	 * @return false 如果文件无法打开或写入过程中出现错误
	 */
	bool write_to_file(WeightedAdjMatrixGraph& graph, const std::string& filename)
	{
		std::ofstream file(filename);
		if (!file.is_open()) {
			std::cerr << "无法打开文件: " << filename << "\n";
			return false;
		}

		// 写入顶点
		std::println(file, "# 支持 “#” 号 行注释\n\n# [VERTEX] LISTS");
		for (const auto& vertex : graph.m_vertexMap | std::views::values) {
			std::println(file, "[Vertex] {} {} {} {} {}",
				vertex->m_name, vertex->m_id, vertex->m_location.first, vertex->m_location.second,
				static_cast<int>(vertex->m_attr));
		}

		// 写入边
		std::println(file, "\n# [EDGE] LISTS");
		for (int i = 0; i < graph.m_vertices; ++i) {
			for (int j = i + 1; j < graph.m_vertices; ++j) {
				if (graph.m_adjMatrix[i][j] != -1) {
					std::println(file, "[Edge] {} {} {}", i, j, graph.m_adjMatrix[i][j]);
				}
			}
		}

		file.close();
		return true;
	}	
}
