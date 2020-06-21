#pragma once
/**
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<map>
#include<ctime>
#include<cstdio>
#include<unordered_map>
using namespace std;



void printfTwoDimensionalArray1(vector<vector<int>>&result) {
	for (auto &m : result)
		printf("%d,%d\n", m[0], m[1]);
}



//运行时间：1.686s
using tab_t = vector<vector<int>>;

//哈希连接比嵌套循环内连接快了1倍 从中兴的服务器效率上快了3倍
tab_t join(const tab_t&v1, const tab_t&v2) { //714ms  659ms

	multimap<int, size_t> hashmap;  //使用map比使用hashmap效率更高的原因在于重复值太多 hashmap重复值太多的极限就是链表
									//hash 此处选择较小的表进行hash 
	for (size_t i = 0; i < v1.size(); ++i) {
		hashmap.insert(std::make_pair(v1[i][2], i));
	}



	tab_t result;
	for (size_t i = 0; i < v2.size(); ++i) {

		auto range = hashmap.equal_range(v2[i][2]);
		for (auto it = range.first; it != range.second; ++it) {
			result.emplace_back(vector<int>{v1[it->second][0], v1[it->second][1], v2[i][0], v2[i][1]});
		}

	}

	printfTwoDimensionalArray1(result);
	return result;
}


//根据t1.id2进行分组
int get_t1_id2(const vector<int> & t)
{
	return t[1];
}
//根据t2.id2进行分组
int get_t2_id2(const vector<int>& t) {
	return t[3];
}


template< typename Map, typename Iterator, typename F >
void groupByImpl(Map & map, Iterator && current, F && f)
{
	if (map[f(*current)].size() > 0) {
		map[f(*current)][0] = max((map[f(*current)][0]), (*current)[0]);
		map[f(*current)][1] = min((map[f(*current)][1]), (*current)[2]);
	}
	else {
		map[f(*current)].push_back((*current)[0]);
		map[f(*current)].push_back((*current)[2]);
	}

}
//模板的参数推断功能
template< typename Map, typename Iterator, typename F, typename... Fs >
void groupByImpl(Map & map, Iterator && current, F && f, Fs &&... fs)
{
	groupByImpl(map[f(*current)], current, std::forward< Fs >(fs)...);
}

template<typename Iterator, typename F, typename... Fs>
unordered_map< int, map<int, vector<int>>> groupBy(Iterator begin, Iterator const end, F&&f, Fs&&... fs) {
	unordered_map<int, map<int, vector<int>>>result;
	for (; begin != end; ++begin) {
		groupByImpl(result, begin, std::forward<F>(f), std::forward<Fs>(fs)...);
	}
	return result;
}


//将map< int, map<int, vector<int>>> 转换为诶vector<vector<int>>
vector<vector<int>> mapToVector(unordered_map< int, map<int, vector<int>>> &result_map) {
	vector<vector<int>> result;
	for (auto it1 = result_map.begin(); it1 != result_map.end(); it1++) {
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
			result.emplace_back(vector<int>{it1->first, it2->first, it2->second[0], it2->second[1]});
		}
	}
	return result;
}


//order by函数
//先按照max(t1.id1) = v1[1]排序
//再按照t2.id2 = v1[4]排序
//再按照t1.id2 = v1[2]排序
static	bool comp(vector<int>v1, vector<int> v2) {
	return ((v1[2] < v2[2]) || ((v1[2] == v2[2]) && (v1[1] < v2[1])) || ((v1[2] == v2[2]) && (v1[1] == v2[1]) && (v1[0] < v2[0])));
}
void order(vector<vector<int>> &v1) {
	sort(v1.begin(), v1.end(), &comp);
}


//select函数
vector<vector<int>> select(vector<vector<int>>&v1) {
	vector<vector<int>>result;
	for (int i = 0; i < v1.size(); i++) {
		vector<int> row = vector<int>{ v1[i][2],v1[i][3] };
		result.push_back(row);
	}
	return result;
}

//分组功能  16033ms  //运行时间663ms
tab_t groupAndSelect(tab_t &&v1) {

	clock_t startTime = clock();

	unordered_map< int, map<int, vector<int>>> result_map
		= groupBy(v1.begin(), v1.end(), &get_t1_id2, &get_t2_id2); //615ms  map482ms

	clock_t endTime = clock();
	cout << "运行时间：" << double(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "毫秒" << endl;

	//将map转换为vector<vector<int>>

	vector<vector<int>> result = mapToVector(result_map); //20ms

	order(result); //314ms


	vector<vector<int>>output = select(result);  //17ms


	return output;
}



vector<vector<int>> do_my_function(vector<vector<int>>&v1, vector<vector<int>>&v2) {
	vector<vector<int>>result;
	result = groupAndSelect(join(v1, v2));
	return result;
}



int main() {


	//clock_t startTime = clock();
	//	cout.sync_with_stdio(false);
	cin.sync_with_stdio(false); //节省40ms左右

	ifstream in_file1("Z:\\ZTE_CHALLENGE\\input1.csv", ifstream::in); //以读方式打开
	ifstream in_file2("Z:\\ZTE_CHALLENGE\\input1.csv", ifstream::in); //以读方式打开
																	  //ifstream in_file1("/home/web/ztedatabase/input1.csv", ifstream::in); //以读方式打开
																	  //ifstream in_file2("/home/web/ztedatabase/input2.csv", ifstream::in); //以读方式打开

	vector<vector<int>>v1;
	for (std::string line, number; std::getline(in_file1, line);)
	{
		vector<int>temp;
		std::istringstream iss(line);
		while (std::getline(iss, number, ','))
		{
			temp.emplace_back(std::move(std::stoi(number)));
		}
		v1.emplace_back(std::move(temp));
	}

	vector<vector<int>>v2;
	for (std::string line, number; std::getline(in_file2, line);)
	{
		vector<int>temp;
		std::istringstream iss(line);
		while (std::getline(iss, number, ','))
		{
			temp.push_back(std::move(std::stoi(number)));
		}
		v2.emplace_back(std::move(temp));
	}

	//数据读取花费62ms
	//数据处理花费2060ms
	//数据打印花费300ms

	vector<vector<int>> result = do_my_function(v1, v2);

	//printfTwoDimensionalArray1(result);
	//clock_t endTime = clock();
	//cout << "运行时间：" << double(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "毫秒" << endl;
	system("pause");
	return 0;
}

*/