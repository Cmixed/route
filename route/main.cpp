#include "pch.hpp"
#include "data.cpp"
#include "menu.hpp"
#include "thread_pool.cpp"


using namespace std;
using namespace route;
using namespace std::chrono;


constexpr int city_num = 20;

PathEndPoints p1(0, city_num - 1);
PathEndPoints p2(1, city_num - 1);
PathEndPoints p3(1, city_num/2);

int main() {

    // 创建图对象
    route::WeightedAdjMatrixGraph graph(city_num);

    if (graph.readFromFile("graph_output.txt")) {
        std::println("*****文件读入成功.*****\n");
    } else {
        std::cerr << "文件读入失败!" << "\n";
        return 1;
    }

    // 打印图的邻接矩阵
    graph.printGraph();
    std::println();

    // 测试 Dijkstra 算法
    int constexpr  startVertex = 0;  // 起始顶点索引（A）
    int constexpr  endVertex = city_num-1;    // 目标顶点索引（D）
     
    // 使用算法查找最短路径

    auto f_res1 = std::async(std::launch::async, sum_path, graph, p1);
    auto f_res2 = std::async(std::launch::async, sum_path, graph, p2);
    auto f_res3 = std::async(std::launch::async, sum_path, graph, p3);

    print_path_result(graph, algo_num, f_res1.get());
    print_path_result(graph, algo_num, f_res2.get());
    print_path_result(graph, algo_num, f_res3.get());

    // 将图数据写入文件
    if (graph.writeToFile("graph_output.txt"))
    {
        std::println("*****图数据保存成功!*****");
    }
    else
    {
        std::cerr << "图数据保存失败!" << "\n";
        return 1;
    }

	route::PathEndPoints endpoints;
	endpoints.startVertex = p1.startVertex;
	endpoints.endVertex = p1.endVertex;

	auto path_results = route::calculate_path_times(graph, endpoints);

    print_path_result(graph, algo_num, path_results);

    return 0;
}