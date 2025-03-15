#include "pch.hpp"
#include "data.hpp"

using namespace std;
using namespace route;

int main()
{
    WeightedAdjMatrixGraph graph(5);

    // 从文件读取图数据
    if (graph.readFromFile("graph.txt"))
    {
        std::cout << "图数据加载成功!" << std::endl;
    }
    else
    {
        std::cerr << "图数据加载失败!" << std::endl;
        return 1;
    }

    // 打印图结构
    graph.printGraph();

    // 计算最短路径
    auto [path, distance] = graph.dijkstra(0, 4);

    // 打印路径
    graph.printPath(path, distance);

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