#include "pch.hpp"
#include "data.hpp"
#include "thread_pool.cpp"

using namespace std;
using namespace route;

constexpr int algorithm_number = 4;
constexpr int CITY_COUNT = 20;

PathEndpoints p1(0, CITY_COUNT - 1);
PathEndpoints p2(1, CITY_COUNT - 1);
PathEndpoints p3(1, CITY_COUNT/2);



auto sum_path(route::WGraph const& graph, route::PathEndpoints const pep) -> std::vector<std::pair<std::vector<int>, int>>
{
	auto const path1 = graph.localSearchOptimization(pep.startVertex, pep.endVertex);
	auto const path2 = graph.geneticAlgorithm(pep.startVertex, pep.endVertex);
	auto const path3 = graph.dijkstra(pep.startVertex, pep.endVertex);
	auto const path4 = graph.geneticLocalSearchOptimization(pep.startVertex, pep.endVertex);

	std::vector<std::pair<std::vector<int>, int>> sum;
	sum.push_back(path1);
	sum.push_back(path2);
	sum.push_back(path3);
	sum.push_back(path4);

	return sum;
}

auto print_path_result(route::WGraph const& graph, std::vector<std::pair<std::vector<int>, int>> const& path_sum) -> void
{
	static int order{ 0 };

	std::println("----------第 {} 个路径规划----------", order); ++order;
	for (int i=0; i<algorithm_number; i++) {
		auto const [path, dis] = path_sum[i];
		switch (i) {
		case 0:
			std::println("\n=====退火局部搜索算法\n");
			graph.printPath(path, dis);
			break;
		case 1:
			std::println("\n=====遗传算法:\n");
			graph.printPath(path, dis);
			break;
		case 2:
			std::println("\n=====Dijkstra:\n");
			graph.printPath(path, dis);
			break;
		case 3:
			std::println("\n遗传局部搜索:\n");
			graph.printPath(path, dis);
			break;
		}
	}
}

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
		 
		
        // 使用算法查找最短路径

		ThreadPool tp(3);
		
		tp.submit(sum_path, graph, p1);

		auto f_res1 = std::async(std::launch::async, sum_path, graph, p1);
		auto f_res2 = std::async(std::launch::async, sum_path, graph, p2);
		auto f_res3 = std::async(std::launch::async, sum_path, graph, p3);
		//auto const res = sum_path(graph, p1);


		print_path_result(graph, f_res1.get());
		print_path_result(graph, f_res2.get());
		print_path_result(graph, f_res3.get());


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