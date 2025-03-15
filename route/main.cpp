#include "pch.hpp"
#include "data.hpp"

using namespace std;
using namespace route;


int main() {
        // 创建图对象
        route::WeightedAdjMatrixGraph graph(4);

        graph.readFromFile("graph_output.txt");

        // 打印图的邻接矩阵
        graph.printGraph();

        // 测试 Dijkstra 算法
        int startVertex = 0;  // 起始顶点索引（A）
        int endVertex = 3;    // 目标顶点索引（D）

        // 使用 Dijkstra 算法查找最短路径
        auto result = graph.dijkstra(startVertex, endVertex);

        graph.printPath(result.first, result.second);

	// 将图数据写入文件
	    if (graph.writeToFile("graph_output.txt"))
	    {
	        std::cout << "图数据保存成功!" << std::endl;
	    }
	    else
	    {
	        std::cerr << "图数据保存失败!" << std::endl;
	        return 1;
	    }

        return 0;
    }