#include "pch.hpp"
#include "data.hpp"

using namespace std;
using namespace route;

int main() {
    // 创建图
    route::WeightedAdjMatrixGraph graph(5);

    // 添加顶点
    graph.addVertex(route::Object::create("A", 0, {0, 0}));
    graph.addVertex(route::Object::create("B", 1, {1, 1}));
    graph.addVertex(route::Object::create("C", 2, {2, 2}));
    graph.addVertex(route::Object::create("D", 3, {3, 3}));
    graph.addVertex(route::Object::create("E", 4, {4, 4}));

    // 添加边
    graph.addEdge(0, 1, 10);
    graph.addEdge(0, 2, 15);
    graph.addEdge(0, 3, 20);
    graph.addEdge(1, 2, 35);
    graph.addEdge(1, 4, 25);
    graph.addEdge(2, 3, 30);
    graph.addEdge(2, 4, 10);
    graph.addEdge(3, 4, 20);

    // 打印图
    graph.printGraph();

    // 使用遗传算法计算最短路径
    auto result = graph.dijkstra(0, 4);
    if (!result.first.empty()) {
       graph.printPath(result.first, result.second);
    } else {
        std::cout << "No path found.\n";
    }


    return 0;
}