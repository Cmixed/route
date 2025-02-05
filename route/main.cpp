import std;
import data;

using namespace std;
using namespace route;

int main() {
    int n, m;
    cout << "请输入顶点数和边数：";
    cin >> n >> m;
    
    Graph g(n);
    
    cout << "请输入边的起点、终点和权重（共" << m << "条边）：" << endl;
    for (int i = 0; i < m; ++i) {
        int u, v, w;
        cin >> u >> v >> w;
        g.addEdge(u, v, w); // 添加边
    }
    
    int start;
    cout << "请输入起始顶点：";
    cin >> start;
    
    vector<int> shortestDistances = g.Dijkstra(start);
    
    cout << "从顶点" << start << "到其他所有顶点的最短距离：" << endl;
    for (int i = 0; i < n; ++i) {
        if (shortestDistances[i] == numeric_limits<int>::max()) {
            cout << "顶点" << i << "：无法到达" << endl;
        } else {
            cout << "顶点" << i << "：距离 = " << shortestDistances[i] << endl;
        }
    }
    
    return 0;
}