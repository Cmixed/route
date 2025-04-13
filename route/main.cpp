#include "pch.hpp"
#include "data.cpp"
#include "menu.hpp"
#include "col_zzj.hpp"
#include "thread_pool.cpp"


using namespace route;
using namespace std::literals;
using namespace zzj;
using namespace zzj::literals;

constexpr int city_num = 20;

int main() {

    // 创建图对象
    route::WeightedAdjMatrixGraph graph(city_num);

    if (graph.readFromFile("graph_output.txt")) {
        std::println("*****文件读入成功.*****\n");
    } else {
        std::cerr << "文件读入失败!" << "\n";
        return 1;
    }

    // 打印图的邻接矩阵
    graph.printGraph();
    std::println();

    std::vector const p_v{
		PathEndPoints{0, city_num - 1},
		PathEndPoints{1, city_num - 1},
		PathEndPoints{1, city_num / 2}
    };

    std::vector<std::future<std::vector<PathTimePair>>> f_v_v;
    f_v_v.reserve(p_v.size() * (algo_num + 1));

	for (const auto& endpoints : p_v) {
		// 启动异步任务
		f_v_v.emplace_back(std::async(std::launch::async,
		[graph, endpoints]() { return route::calculate_path_times(graph, endpoints); }));
	}

	// 等待所有异步任务完成并打印结果
    for (auto& future : f_v_v) {
        try {
            auto path_results = future.get(); // 获取异步任务的结果
            print_path_result(graph, algo_num, path_results);
        } catch (const std::exception& e) {
            std::cerr << "异步任务出错: " << e.what() << "\n";
        }
    }

    // 将图数据写入文件
    if (graph.writeToFile("graph_output.txt"))
    {
        std::println("*****图数据保存成功!*****");
    }
    else
    {
        std::cerr << "图数据保存失败!" << "\n";
        return 1;
    }


    std::println("额外测试");
	route::PathEndPoints endpoints{1,3};
	auto const path_results = route::calculate_path_times(graph, endpoints);

    print_path_result(graph, algo_num, path_results);

    auto x{ "111"_log };
    auto y{ "222"_col };

    std::ignore = Color(ColorName::GREEN, "1111");
    {
	    std::ignore = Color(ColorName::RED);
	    std::println("333");
    }

    std::println("444");


    return 0;
}

/* backup future */

// 使用算法查找最短路径
// PathEndPoints p1(0, city_num - 1);
//PathEndPoints p2(1, city_num - 1);
//PathEndPoints p3(1, city_num/2);
//auto f_res1 = std::async(std::launch::async, sum_path, graph, p1);
//auto f_res2 = std::async(std::launch::async, sum_path, graph, p2);
//auto f_res3 = std::async(std::launch::async, sum_path, graph, p3);
//print_path_result(graph, algo_num, f_res1.get());
//print_path_result(graph, algo_num, f_res2.get());
//print_path_result(graph, algo_num, f_res3.get());