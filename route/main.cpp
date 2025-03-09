import std;
import data;

using namespace std;
using namespace route;


int main() {
    // 创建图，4个顶点
    WeightedAdjMatrixGraph graph(4);

    // 创建顶点
    auto vertexA = make_shared<Object>();
    vertexA->m_name = "A";
    vertexA->m_location = {0, 0};
    vertexA->m_attr = Attribute::Empty;

    auto vertexB = make_shared<Object>();
    vertexB->m_name = "B";
    vertexB->m_location = {1, 1};
    vertexB->m_attr = Attribute::Occupied;

    auto vertexC = make_shared<Object>();
    vertexC->m_name = "C";
    vertexC->m_location = {2, 2};
    vertexC->m_attr = Attribute::Place;

    auto vertexD = make_shared<Object>();
    vertexD->m_name = "D";
    vertexD->m_location = {3, 3};
    vertexD->m_attr = Attribute::Supply;

    // 添加顶点到图中
    graph.addVertex(vertexA);
    graph.addVertex(vertexB);
    graph.addVertex(vertexC);
    graph.addVertex(vertexD);

    // 添加带权重的边
    graph.addEdge(0, 1, 5); // A-B，权重5
    graph.addEdge(0, 2, 3); // A-C，权重3
    graph.addEdge(1, 3, 2); // B-D，权重2

    // 打印图的结构
    graph.printGraph();

    // 测试获取权重
    cout << "边A-B的权重: " << graph.getWeight(0, 1) << endl;
    cout << "边A-C的权重: " << graph.getWeight(0, 2) << endl;
    cout << "边B-D的权重: " << graph.getWeight(1, 3) << endl;

    // 测试获取顶点信息
    auto vertex = graph.getVertex(0);
    if (vertex) {
        cout << "顶点 " << vertex->m_name << " 的位置是 (" 
             << vertex->m_location.first << ", " 
             << vertex->m_location.second << "), 属性是 ";
        switch (vertex->m_attr) {
            case Attribute::Empty:
                cout << "Empty" << endl;
                break;
            case Attribute::Occupied:
                cout << "Occupied" << endl;
                break;
        case Attribute::Place:
                cout << "Place" << endl;
                break;
        }
    }

    return 0;
}