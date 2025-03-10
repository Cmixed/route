#pragma once

#define NAMESPACE_ROUTE_BEGIN namespace route {
#define NAMESPACE_ROUTE_END }

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <utility>
#include <algorithm>

//export class MapMessage
//{
//	int Vertices{}, Edges{}; // �������ͱ���
//
//    
//};
//
//
//// �Զ��˼�¼��ʽ
//export struct PacRecord
//{
//    std::string m_time_s{ 0 };
//    unsigned long long score{ 0 };
//};
//
//export
//{
//	optional<int> fileRead(PacConfigure &configure); // �ļ�����
//    optional<int> fileWrite(const PacRecord& record); // �ļ�д��
//}
//
///**
// * @brief �������ļ��� configure.ini �ж���
// * @param record 
// * @return normal 1 error 0
// */
//optional<int> fileRead(PacConfigure &configure)
//{
//	ifstream fin("configure.ini", ios_base::in | ios_base::binary);
//
//    std::string place;
//
//    fin >> place;
//	fin >> place >> configure.m_fps;
//    fin >> place >> configure.m_playerSpeed;
//    fin >> place >> configure.m_ghostSpeed;
//
//    fin.close();
//
//    //cout << "ReadFile Successfully��\n";
//    
//    return optional<int>{1};
//}
//
///**
// * @brief ����¼д�� data.txt
// * @param record 
// * @return normal 1 error 0
// */
//optional<int> fileWrite(const PacRecord& record)
//{
//    ofstream fout("data.txt", ios_base::out | ios_base::app);
//
//    const auto now = std::chrono::system_clock::now();
//    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
//    std::tm nowTm;
//
//    try {
//	    if (const auto err = localtime_s(&nowTm, &nowTime) != 0) {
//            throw runtime_error("Time convert error!");
//	    }
//    }
//    catch (exception &e) {
//        std::cerr << e.what();
//        return optional<int>{0};
//    }
//
//    // ��ӡ��Ϣ
//    {
//        // ��һ������
//		static int lineNumber = 1;
//	    if (lineNumber == 1) {
//	        fout << "\nGame Start Time(Local): " << std::put_time(&nowTm, "%Y-%m-%d %H:%M:%S") << '\n';
//	    }
//
//	    auto [time_cost, player_score] = record;
//
//	    fout << format("{: >4} : ", lineNumber) << std::put_time(&nowTm, "%Y-%m-%d %H:%M:%S")
//    		<< format(" : Time cost: {}s, Score: {}\n", time_cost, player_score);
//
//        // ��������
//		++lineNumber;
//    }
//
//    fout.close();
//
//    //std::cout << "FileIO over!\n";
//
//    /* ��ʵ��
//     * ios_base::fmtflags old = fout.setf(ios::left, ios::adjustfield);
//     * fout.setf(old, ios::adjustfield);
//     */
//
//    return optional<int>{1};
//}