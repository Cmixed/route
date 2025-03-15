#include "pch.hpp"
#include "data.hpp"

using namespace std;
using namespace route;

int main() {

        // 创建图对象
        route::WeightedAdjMatrixGraph graph(4);

        if (graph.readFromFile("graph_output.txt")) {
            std::print("文件读入成功.\n");
        } else {
	        std::cerr << "文件读入失败!" << "\n";
	        return 1;
        }

        // 打印图的邻接矩阵
        graph.printGraph();

        // 测试 Dijkstra 算法
        int constexpr  startVertex = 0;  // 起始顶点索引（A）
        int constexpr  endVertex = 3;    // 目标顶点索引（D）

        // 使用算法查找最短路径
        auto const[path, dis] = graph.geneticAlgorithm(startVertex, endVertex);

        graph.printPath(path, dis);

		// 将图数据写入文件
	    if (graph.writeToFile("graph_output.txt"))
	    {
            std::println("图数据保存成功!");
	    }
	    else
	    {
	        std::cerr << "图数据保存失败!" << "\n";
	        return 1;
	    }

        return 0;
    }