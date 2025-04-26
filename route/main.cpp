#include "pch.hpp"
#include "data.cpp"
#include "menu.hpp"
#include "col_zzj.hpp"
#include "file_io.cpp"
#include "thread_pool.cpp"

using namespace route;
using namespace std::literals;
using namespace zzj;
using namespace zzj::literals;

constexpr int city_num = 20;



int main() {

    // 创建对象
    color_ctrl.default_color = ColorName::WHITE;

    auto graph = WGraph(city_num);
    auto menu = Menu("User");

    menu.statusBarFr();
    menu.ready();

    menu.printMsg(MsgTy::MESSAGE, "继续以读入文件");
    menu.waitEnter();

    if (auto res = menu.readFile(graph, "graph.txt");
        res.has_value()) {
        menu.printMsg(MessageType::SUCCESS, "文件读入成功。");
    } else {
        menu.printMsg(MessageType::ERROR, "文件读入失败！");
    }

    menu.printMsg(MsgTy::MESSAGE, "打印图的邻接矩阵");
    graph.printGraph();
    std::println();

    std::vector const p_v{
		PathEndPoints{0, city_num - 1},
		PathEndPoints{1, city_num - 1},
		PathEndPoints{1, city_num / 2}
    };

    if (auto res = paths_task(graph, p_v);
        res.has_value()) {
        for (const auto& pre_res : res.value()) {
			print_path_result(graph, algo_num, pre_res);
        }
    } else {
        std::println(std::cerr, "error!");
    }

    menu.writeFile(graph, "graph.txt");


    menu.waitEnter();
    menu.statusBarFr();
    

    std::println("额外算法测试");
	route::PathEndPoints endpoints{1,3};
	auto const path_results = route::calculate_path_times(graph, endpoints);

    print_path_result(graph, algo_num, path_results);



    std::println("颜色库");
    auto x{ "111"_log };
    auto y{ "222"_col };

    std::ignore = Color(ColorName::GREEN, "1111");
    {
	    std::ignore = Color(ColorName::RED, "RED");
	    std::println("333");
    }

    {
        auto x = Color(ColorName::RED);
        std::println("333");
    }
    std::println("444");




    Color::displayFrColor(ColorName::GREEN);
    input_test();


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


//   std::vector<std::future<std::vector<PathTimePair>>> f_v_v;
//   f_v_v.reserve(p_v.size() * (algo_num + 1));


//for (const auto& endpoints : p_v) {
//	// 启动异步任务
//	f_v_v.emplace_back(std::async(std::launch::async,
//	[graph, endpoints]() { return route::calculate_path_times(graph, endpoints); }));
//}

//// 等待所有异步任务完成并打印结果
//   for (auto& future : f_v_v) {
//       try {
//           auto path_results = future.get(); // 获取异步任务的结果
//           print_path_result(graph, algo_num, path_results);
//       } catch (const std::exception& e) {
//           std::println(std::cerr, "异步任务出错:{}", e.what());
//       }
//   }