#include "pch.hpp"
#include "data.hpp"
#include <random>

using namespace std;
using namespace route;

// 随机数生成器配置
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0, 1000);

int main() {
    const int CITY_COUNT = 20;
    const int EXTRA_EDGES = 200;
    
    // 创建图
    route::WeightedAdjMatrixGraph graph(CITY_COUNT);

    // 添加顶点（随机坐标）
    vector<pair<double, double>> coordinates;
    for (int i = 0; i < CITY_COUNT; ++i) {
        double x = dis(gen);
        double y = dis(gen);
        coordinates.emplace_back(x, y);
        graph.addVertex(route::Object::create(std::format("C{}", i), i, {x, y}));
    }

    // 构建基础连通路径（链式连接）
    for (int i = 0; i < CITY_COUNT-1; ++i) {
        double dx = coordinates[i].first - coordinates[i+1].first;
        double dy = coordinates[i].second - coordinates[i+1].second;
        int weight = static_cast<int>(sqrt(dx*dx + dy*dy));
        graph.addEdge(i, i+1, weight);
        graph.addEdge(i+1, i, weight); // 无向图双向连接
    }

    // 添加随机边
    uniform_int_distribution<> vertexDist(0, CITY_COUNT-1);
    for (int e = 0; e < EXTRA_EDGES; ) {
        int u = vertexDist(gen);
        int v = vertexDist(gen);
        if (u != v) {
            double dx = coordinates[u].first - coordinates[v].first;
            double dy = coordinates[u].second - coordinates[v].second;
            int weight = static_cast<int>(sqrt(dx*dx + dy*dy));
            graph.addEdge(u, v, weight);
            graph.addEdge(v, u, weight);
            ++e;
        }
    }

    graph.printGraph();

    // 执行算法测试
    auto result = graph.geneticAlgorithm(0, CITY_COUNT-1);
    if (!result.first.empty()) {
        graph.printPath(result.first, result.second);
    }
    else {
        std::cout << "No path found.\n";
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