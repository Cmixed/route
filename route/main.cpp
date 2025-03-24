#include "pch.hpp"
#include "data.hpp"
#include "thread_pool.cpp"

using namespace std;
using namespace route;

constexpr int CITY_COUNT = 4;

int main() {

        // 创建图对象
        route::WeightedAdjMatrixGraph graph(CITY_COUNT);

        if (graph.readFromFile("graph_output.txt")) {
            std::print("文件读入成功.\n");
        } else {
	        std::cerr << "文件读入失败!" << "\n";
	        return 1;
        }

        // 打印图的邻接矩阵
        graph.printGraph();
		std::println();

        // 测试 Dijkstra 算法
        int constexpr  startVertex = 0;  // 起始顶点索引（A）
        int constexpr  endVertex = CITY_COUNT-1;    // 目标顶点索引（D）
		 
		PathEndpoints p1(0, CITY_COUNT-1);
		PathEndpoints p2(1, CITY_COUNT-1);
		PathEndpoints p3(0, CITY_COUNT/2);
        // 使用算法查找最短路径

		{
	        std::println("优化算法");
	        auto const[path, dis] = graph.localSearchOptimization(startVertex, endVertex);
	        graph.printPath(path, dis);
        }
        {
	        std::println("Dijkstra");
	        auto const[path, dis] = graph.dijkstra(startVertex, endVertex);
	        graph.printPath(path, dis);
        }
        {
	        std::println("遗传算法:");
	        auto const[path, dis] = graph.geneticAlgorithm(startVertex, endVertex);
	        graph.printPath(path, dis);
        }


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