#pragma once

#ifndef FILE_IO_HPP
#define FILE_IO_HPP

#include "pch.hpp"

namespace route
{
	/*****************************************************************
	 *
	 *		IO 函数声明
	 *
	 *****************************************************************/

	class WeightedAdjMatrixGraph;

	/* 两个友元函数 */
	bool read_from_file(WeightedAdjMatrixGraph& graph, const std::string& filename);
	bool write_to_file(WeightedAdjMatrixGraph& graph, const std::string& filename);

}

#endif