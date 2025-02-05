// Purpose: Define the data module for the route project.
// Author:  Cmixed
module;

#define NAMESPACE_ROUTE_BEGIN namespace route {
#define NAMESPACE_ROUTE_END }

export module data;

import std;

NAMESPACE_ROUTE_BEGIN

// City number


// Define the attribute of the object
export enum class Attribute : int {
	Empty = 1,
	Place = 2,
	Supply = 3,
	Result = 4
};

template <typename T>
class BaseObject
{
public:
	// base information
    std::string Name{};
	Attribute Attr{ Attribute::Empty };
	T Link{};
	T X{};
	T Y{};
};

export using Object = BaseObject<int>;

// 定义图的边结构
struct Edge {
    int To;      // 边的目标顶点
    int Weight;  // 边的权重
    Edge(int t, int w) : To(t), Weight(w) {}
};

// 定义图类
export class Graph {
private:
    int n; // 顶点数量
    std::vector<std::vector<Edge>> adj; // 邻接表表示图

public:
    // 构造函数
    Graph(int vertices) : n(vertices), adj(vertices) {}

    // 添加边
    void addEdge(int from, int to, int weight) {
        adj[from].emplace_back(to, weight);
    }

    // Dijkstra算法
    std::vector<int> Dijkstra(int start) const {
        std::vector<int> dist(n, std::numeric_limits<int>::max()); // 距离数组，初始化为无穷大
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq; // 优先队列，按距离从小到大排序

        dist[start] = 0; // 起始点到自身的距离为0
        pq.push({0, start}); // 将起始点加入优先队列

        while (!pq.empty()) {
            const int u = pq.top().second; // 当前顶点
            const int currentDist = pq.top().first; // 当前顶点的距离
            pq.pop();

            // 如果当前从队列中取出的距离大于已知最短距离，则跳过
            if (currentDist > dist[u]) continue;

            // 遍历当前顶点的所有邻接边
            for (const Edge& edge : adj[u]) {
                int v = edge.To; // 邻接顶点
                const int weight = edge.Weight; // 边的权重

                // 计算从起始点经过当前顶点到邻接顶点的距离
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight; // 更新最短距离
                    pq.push({dist[v], v}); // 将邻接顶点加入优先队列
                }
            }
        }

        return dist; // 返回最短路径距离数组
    }
};


NAMESPACE_ROUTE_END