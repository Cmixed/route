// Purpose: Define the data module for the route project.
// Author:  Cmixed
#pragma once

#define NAMESPACE_ROUTE_BEGIN namespace route {
#define NAMESPACE_ROUTE_END }

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>

using namespace std;

NAMESPACE_ROUTE_BEGIN

// City number

// Define the attribute of the object
enum class Attribute : std::uint8_t {
	Empty = 0,
	Place,
	Supply,
	Occupied,
};

/**
 * @brief ��������  1: ���� 2: λ�� 3: ����
 * @tparam T 
 */
template <typename T>
class BaseObject
{
public:
	// base information
    std::string m_name{};
    std::pair<T, T> m_location{};
	Attribute m_attr{ Attribute::Empty };
};

using Object = BaseObject<int>;

// ����ͼ�ı߽ṹ
struct Edge {
    int m_to;      // �ߵ�Ŀ�궥��
    int m_weight;  // �ߵ�Ȩ��
    Edge(const int to,const int weight) : m_to(to), m_weight(weight) {}
};

// ����ͼ��
class WeightedAdjMatrixGraph {
private:
    int m_vertices; // ������
    int m_edges;    // ����
    vector<shared_ptr<Object>> m_vertexList; // �����б��洢����ָ��
    vector<vector<int>> m_adjMatrix; // �ڽӾ��󣬴洢Ȩ��
public:
    // ���캯��
    WeightedAdjMatrixGraph(int v) : m_vertices(v), m_edges(0) {
        // ��ʼ���ڽӾ��󣬳�ʼֵΪ-1��ʾ�ޱ�
        m_adjMatrix.resize(v, vector<int>(v, -1));
    }

    // ��Ӷ���
    void addVertex(const shared_ptr<Object>& vertex) {
        m_vertexList.push_back(vertex);
    }

    // ��Ӵ�Ȩ�صı�
    void addEdge(const int src,const int dest,const int weight) {
        if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices && weight > 0) {
            m_adjMatrix[src][dest] = weight;
            m_adjMatrix[dest][src] = weight; // ���������ͼ
            m_edges++;
        }
    }

    // ��ӡͼ�Ľṹ
    void printGraph() const {
        cout << "��Ȩ�ص�ͼ���ڽӾ����ʾ��" << endl;
        for (int i = 0; i < m_vertices; i++) {
            for (int j = 0; j < m_vertices; j++) {
                if (m_adjMatrix[i][j] == -1) {
                    cout << "�� "; // �ޱ�ʱ��ʾ�����
                } else {
                    cout << m_adjMatrix[i][j] << " ";
                }
            }
            cout << endl;
        }
    }

    // ��ȡȨ��
    int getWeight(const int src,const int dest) const {
        if (src >= 0 && src < m_vertices && dest >= 0 && dest < m_vertices) {
            return m_adjMatrix[src][dest];
        }
        return -1; // ��Ч����
    }

    // ��ȡ������Ϣ
    shared_ptr<Object> getVertex(const int index) {
        if (index >= 0 && index < m_vertices) {
            return m_vertexList[index];
        }
        return nullptr; // ��Ч����
    }
};

NAMESPACE_ROUTE_END