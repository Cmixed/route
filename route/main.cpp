#include "pch.hpp"
#include "data.hpp"

using namespace std;
using namespace route;

int main()
{
    WeightedAdjMatrixGraph graph(5);

    // 批量添加顶点，使用 Object::create 工厂函数
    graph.addVertices(
        Object::create("A", {0, 0}, Attribute::Place),
        Object::create("B", {1, 0}, Attribute::Place),
        Object::create("C", {2, 0}, Attribute::Place),
        Object::create("D", {3, 0}, Attribute::Place),
        Object::create("E", {4, 0}, Attribute::Place)
    );

    // 添加边
    graph.addEdge(0, 1, 4);
    graph.addEdge(0, 2, 2);
    graph.addEdge(1, 3, 5);
    graph.addEdge(2, 3, 8);
    graph.addEdge(3, 4, 1);

    // 打印图的结构
    graph.printGraph();

    // 计算从顶点0到顶点4的最短路径
    auto const result = graph.dijkstra(0, 4);
    std::vector<int> path = result.first;
    int const distance = result.second;

    // 打印路径和距离
	WGraph::printPath(path, distance);
    //graph.printPath(path, distance);

    return 0;
}