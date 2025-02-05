import std;
import data;

using namespace route;

int main() {
    Graph g;
    g.add_edge("A", "B", 6);
    g.add_edge("A", "C", 2);
    g.add_edge("B", "D", 1);
    g.add_edge("C", "B", 3);
    g.add_edge("C", "D", 5);
    g.add_edge("D", "E", 2);
    g.add_edge("E", "B", 4);
    
    try {
        auto result = g.dijkstra("A");
        
        for (const auto& node : {"A", "B", "C", "D", "E"}) {
            if (result.has_path_to(node)) {
                std::print("Distance to {}: {}\nPath: ", node, result.distance_to(node));
                for (const auto& n : result.path_to(node)) {
                    std::print("{} ", n);
                }
				std::print("\n\n");
            } else {
				std::print("{} is unreachable\n\n", node);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}