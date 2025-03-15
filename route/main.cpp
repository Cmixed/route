#include "pch.hpp"
#include "data.hpp"

using namespace std;
using namespace route;

//int main()
//{
//    WeightedAdjMatrixGraph graph(5);
//
//    // 从文件读取图数据
//    if (graph.readFromFile("graph.txt"))
//    {
//        std::cout << "图数据加载成功!" << std::endl;
//    }
//    else
//    {
//        std::cerr << "图数据加载失败!" << std::endl;
//        return 1;
//    }
//
//    // 打印图结构
//    graph.printGraph();
//
//    // 计算最短路径
//    auto [path, distance] = graph.dijkstra(0, 4);
//
//    // 打印路径
//    WGraph::printPurePath(path, distance);
//    graph.printPath(path, distance);
//
//    // 将图数据写入文件
//    if (graph.writeToFile("graph_output.txt"))
//    {
//        std::cout << "图数据保存成功!" << std::endl;
//    }
//    else
//    {
//        std::cerr << "图数据保存失败!" << std::endl;
//        return 1;
//    }
//
//    return 0;
//}

int main() {
        // 创建图对象
        route::WeightedAdjMatrixGraph graph(4);

        // 创建顶点
        auto vertexA = route::Object::create("A", 0, {0, 0}, route::Attribute::Place);
        auto vertexB = route::Object::create("B", 1, {1, 1}, route::Attribute::Place);
        auto vertexC = route::Object::create("C", 2, {2, 2}, route::Attribute::Place);
        auto vertexD = route::Object::create("D", 3, {3, 3}, route::Attribute::Place);

        // 添加顶点到图中
        graph.addVertex(vertexA);
        graph.addVertex(vertexB);
        graph.addVertex(vertexC);
        graph.addVertex(vertexD);

        // 添加边
        graph.addEdge(0, 1, 5);  // A 到 B 的权重为 5
        graph.addEdge(0, 2, 3);  // A 到 C 的权重为 3
        graph.addEdge(1, 2, 2);  // B 到 C 的权重为 2
        graph.addEdge(1, 3, 1);  // B 到 D 的权重为 1
        graph.addEdge(2, 3, 4);  // C 到 D 的权重为 4

        // 打印图的邻接矩阵
        graph.printGraph();

        // 测试 Dijkstra 算法
        int startVertex = 0;  // 起始顶点索引（A）
        int endVertex = 3;    // 目标顶点索引（D）

        // 使用 Dijkstra 算法查找最短路径
        auto result = graph.dijkstra(startVertex, endVertex);

        // 打印结果
        std::cout << "最短路径: ";
        for (int vertexIndex : result.first) {
            std::cout << graph.getVertex(vertexIndex)->m_name << " ";
        }
        std::cout << "\n总距离: " << result.second << std::endl;

        return 0;
    }