// Purpose: Define the data module for the route project.
// Author:  Cmixed
module;

#define NAMESPACE_ROUTE_BEGIN namespace route {
#define NAMESPACE_ROUTE_END }

export module data;

import std;

NAMESPACE_ROUTE_BEGIN

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
	T X{};
	T Y{};
};

export using Object = BaseObject<int>;

struct Ege
{
	std::string Source{};
	std::string Target{};
	int Weight{};

    Ege(const std::string_view tar, const int weight)
		: Target(tar), Weight(weight) {}
	Ege(const std::string_view src, const std::string_view tar, const int weight)
		: Source(src), Target(tar), Weight(weight) {}
};

class Graph
{
private:
	std::unordered_map<std::string, std::vector<Ege>> AdjList{};
public:
    void add_edge(const std::string& source, const std::string& target, int weight) {
        AdjList[source].emplace_back(target, weight);
        AdjList.try_emplace(target);
    }

    class DijkstraResult {
        std::unordered_map<std::string, int> distances;
        std::unordered_map<std::string, std::string> predecessors;

    public:
        DijkstraResult(auto&& d, auto&& p)
            : distances(std::forward<decltype(d)>(d)),
              predecessors(std::forward<decltype(p)>(p)) {}

        bool has_path_to(const std::string& node) const {
            return distances.contains(node) && 
                   distances.at(node) != std::numeric_limits<int>::max();
        }

        int distance_to(const std::string& node) const {
            return distances.at(node);
        }

        std::vector<std::string> path_to(const std::string& node) const {
            std::vector<std::string> path;
            if (!has_path_to(node)) return path;

            for (std::string current = node; !current.empty();
                 current = predecessors.at(current)) {
                path.push_back(current);
            }

            std::reverse(path.begin(), path.end());
            return path;
        }
    };

    DijkstraResult dijkstra(const std::string& start) const {
        if (!AdjList.contains(start)) {
            throw std::invalid_argument("Start node not found");
        }

        std::unordered_map<std::string, int> distances;
        std::unordered_map<std::string, std::string> predecessors;

        for (const auto& [node, _] : AdjList) {
            distances[node] = std::numeric_limits<int>::max();
        }
        distances[start] = 0;

        using QueueElement = std::pair<int, std::string>;
        std::priority_queue<QueueElement,
                          std::vector<QueueElement>,
                          std::greater<>> pq;
        pq.emplace(0, start);

        while (!pq.empty()) {
            auto [current_dist, u] = pq.top();
            pq.pop();

            if (current_dist > distances[u]) continue;

            for (const auto& edge : AdjList.at(u)) {
                const int new_dist = current_dist + edge.Weight;
                if (new_dist < distances[edge.Target]) {
                    distances[edge.target] = new_dist;
                    predecessors[edge.target] = u;
                    pq.emplace(new_dist, edge.target);
                }
            }
        }

        return {std::move(distances), std::move(predecessors)};
    }

};




NAMESPACE_ROUTE_END