#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<unordered_map>
#include<ctime>
using namespace std;

//运行时间：18.704s
using tab_t = vector<vector<int>>;

class Solution {
public:
	//哈希连接比嵌套循环内连接快了1倍 从中兴的服务器效率上快了3倍
	tab_t join(const tab_t&v1, const tab_t&v2) {
		unordered_multimap<int, size_t> hashmap;
		//hash 此处选择较小的表进行hash 
		for (size_t i = 0; i < v1.size(); ++i) {
			hashmap.insert(std::make_pair(v1[i][2], i));
		}
		tab_t result;
		for (size_t i = 0; i < v2.size(); ++i) {
			auto range = hashmap.equal_range(v2[i][2]);
			for (auto it = range.first; it != range.second; ++it) {
				tab_t::value_type row;
				row.insert(row.end(), v2[i][2]);
				row.insert(row.end(), v1[it->second].begin(), v1[it->second].end() - 1);
				row.insert(row.end(), v2[i].begin(), v2[i].end() - 1);
				result.emplace_back(std::move(row));
			}
		}
		return result;
	}

	//分组功能  16033ms
	tab_t groupAndSelect(tab_t &&v1) {
		clock_t startTime = clock();
		int count = 0;
		vector<vector<int>>result;
		vector<int>visit(v1.size(), 0); //用于判定v1的某row是否被访问过
		cout << "数据规模是: " << v1.size() << endl; //13837次  
		for (int i = 0; i < v1.size(); i++) {
			if (visit[i] == 1)
				continue;
			visit[i] = 1;
			int t1_id1_max = v1[i][1]; //t1各行中id1的最大值
			int t2_id1_min = v1[i][3]; //t2各行中id2的最小值
			for (int j = 0; j < v1.size(); j++) {
				if (visit[j] == 1)
					continue;
				count++; //遍历到此处是600多万次
				if (v1[i][2] == v1[j][2] && v1[i][4] == v1[j][4]) { //group by t1.id2 t2.id2
					visit[j] = 1;
					t1_id1_max = max(t1_id1_max, v1[j][1]);
					t2_id1_min = min(t2_id1_min, v1[j][3]);
				}
			}
			vector<int>result_row{ v1[i][0],t1_id1_max,v1[i][2],t2_id1_min,v1[i][4] };
			result.emplace_back(std::move(result_row));
		}
		cout << "共遍历了" << count << "次" << endl;

		clock_t endTime = clock();
		cout << "运行时间：" << double(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "毫秒" << endl;


		//排序和选择只用了343ms
		order(result);
		vector<vector<int>>output = select(result);

		return output;
	}

	//order by函数
	//先按照max(t1.id1) = v1[1]排序
	//再按照t2.id2 = v1[4]排序
	//再按照t1.id2 = v1[2]排序
	static	bool comp(vector<int>v1, vector<int> v2) {
		return ((v1[1] < v2[1]) || ((v1[1] == v2[1]) && (v1[4] < v2[4])) || ((v1[1] == v2[1]) && (v1[4] == v2[4]) && (v1[2] < v2[2])));
	}
	void order(vector<vector<int>> &v1) {
		sort(v1.begin(), v1.end(), &comp);
	}

	//select函数
	vector<vector<int>> select(vector<vector<int>>&v1) {
		vector<vector<int>>result;
		for (int i = 0; i < v1.size(); i++) {
			vector<int> row = vector<int>{ v1[i][1],v1[i][3] };
			result.push_back(row);
		}
		return result;
	}

	vector<vector<int>> do_my_function(vector<vector<int>>&v1, vector<vector<int>>&v2) {
		vector<vector<int>>result;
		result = groupAndSelect(join(v1, v2));
		return result;
	}
};

class Input {
public:
	//输入函数二：将输入的一行数据转换为vectors
	vector<int> stringToIntegerVector(string &input) {
		vector<int> output;
		stringstream ss(input);
		char delim = ','; //分隔符
		string item;
		while (getline(ss, item, delim)) {
			output.emplace_back(stoi(item));
		}
		return output;
	}
};

class Output {
public:
	void printfTwoDimensionalArray(vector<vector<int>>&result) {
		if (result.size() == 0)
			cout << " " << endl;
		for (int index = 0; index < result.size(); index++) {
			string output;
			for (int j = 0; j < result[index].size(); j++) {
				output += std::to_string(result[index][j]) + ",";
			}
			cout << output.substr(0, output.length() - 1) << endl;;
		}
	}
};

int main() {


	//string input1 = "/home/web/ztedatabase/input1.csv";
	//string input2 = "/home/web/ztedatabase/input2.csv";
	string input1 = "Z:\\ZTE_CHALLENGE\\input1.csv";
	string input2 = "Z:\\ZTE_CHALLENGE\\input2.csv";
	ifstream in_file1(input1.c_str(), ifstream::in); //以读方式打开
	ifstream in_file2(input2.c_str(), ifstream::in); //以读方式打开
													 //Input().check_files(in_file1, input1, in_file2, input2); //检查文件能否打开
	string line;
	//将表1存储起来
	vector<vector<int>>v1;
	while (getline(in_file1, line)) {
		vector<int> row = Input().stringToIntegerVector(line);
		v1.emplace_back(row);
	}
	//将表2存储起来
	vector<vector<int>>v2;
	while (getline(in_file2, line)) {
		vector<int> row = Input().stringToIntegerVector(line);
		v2.emplace_back(row);
	}

	//数据读取花费115ms

	vector<vector<int>> result = Solution().do_my_function(v1, v2);
	//	Output().printfTwoDimensionalArray(result); //打印花费726ms
	system("pause");
	return 0;
}

