#pragma once

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
#include<set>
#include<unordered_set>
using namespace std;



//运行时间：0.526
using tab_t = vector<vector<int>>;

struct group {
	//set<int, greater<int>>max_t1_id1;
	//set<int>min_t2_id1;
	//vector<int>max_t1_id1;
	//vector<int>min_t2_id1;
	int max_t1_id1 = -2147483647;
	int min_t2_id1 = 2147483647;
};

//根据t1.id2进行分组
int get_t1_id2(vector<int> & t)
{
	return t[1];
}
//根据t2.id2进行分组
int get_t2_id2(vector<int>& t) {
	return t[3];
}

template< typename Map, typename Iterator, typename F >
void groupByImpl(Map & map, Iterator && current, F && f)
{

	map[f(*current)].max_t1_id1 = max(map[f(*current)].max_t1_id1, (*current)[0]);
	map[f(*current)].min_t2_id1 = min(map[f(*current)].min_t2_id1, (*current)[2]);

}
//模板的参数推断功能
template< typename Map, typename Iterator, typename F, typename... Fs >
void groupByImpl(Map & map, Iterator && current, F && f, Fs &&... fs)
{
	groupByImpl(map[f(*current)], current, std::forward< Fs >(fs)...);
}

template<typename Iterator, typename F, typename... Fs>
map< int, map<int, group>> groupBy(Iterator begin, Iterator const end, F&&f, Fs&&... fs) {

	map<int, map<int, group>>result;
	for (; begin != end; ++begin) {
		groupByImpl(result, begin, std::forward<F>(f), std::forward<Fs>(fs)...);
	}

	return result;
}


//将map< int, map<int, vector<int>>> 转换为诶vector<vector<int>>
vector<vector<int>> mapToVector(map<int, map<int, group>> &result_map) {
	vector<vector<int>> result;
	for (auto it1 = result_map.begin(); it1 != result_map.end(); it1++) {
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {

			result.emplace_back(vector<int>{it1->first, it2->first, (it2->second).max_t1_id1, (it2->second).min_t2_id1});
		}
	}
	return result;
}




//order by函数
//先按照max(t1.id1) = v1[1]排序
//再按照t2.id2 = v1[4]排序
//再按照t1.id2 = v1[2]排序
static	bool comp(const vector<int>&v1, const vector<int>&v2) {
	return ((v1[2] < v2[2]) || ((v1[2] == v2[2]) && (v1[1] < v2[1])) || ((v1[2] == v2[2]) && (v1[1] == v2[1]) && (v1[0] < v2[0])));
}
void order(vector<vector<int>> &v1) {
	sort(v1.begin(), v1.end(), &comp);
}

void printfTwoDimensionalArray(vector<vector<int>>&result) {
	if (result.size() == 0)
		cout << " " << '\n';
	for (int index = 0; index < result.size(); index++) {
		string output;
		for (int j = 0; j < result[index].size(); j++) {
			output += std::to_string(result[index][j]) + ",";
		}
		cout << output.substr(0, output.length() - 1) << '\n';;
	}
}

//哈希连接比嵌套循环内连接快了1倍 从中兴的服务器效率上快了3倍

tab_t  join(int v1[][3], int v2[][3], int &len1, int &len2) { //714ms  659ms 491ms
															  //join函数
	multimap<int, size_t> hashmap;  //使用map比使用hashmap效率更高的原因在于重复值太多 hashmap重复值太多的极限就是链表
									//hash 此处选择较小的表进行hash 


	for (size_t i = 0; i < len1; ++i) {
		hashmap.insert(std::make_pair(v1[i][2], i));
	}
	tab_t join_Result;

	for (size_t i = 0; i < len2; ++i) {
		auto range = hashmap.equal_range(v2[i][2]);
		for (auto it = range.first; it != range.second; ++it) {
			//	cout << v1[it->second][0] << "   " << v1[it->second][1] << "   " << v2[i][0] << "   " << v2[i][1] << endl;;
			join_Result.emplace_back(vector<int>{v1[it->second][0], v1[it->second][1], v2[i][0], v2[i][1]});
		}
	}
	//printfTwoDimensionalArray(join_Result);

	return join_Result;
}



tab_t  join(tab_t & v1, tab_t & v2) { //714ms  659ms 491ms
									  //join函数
	multimap<int, size_t> hashmap;  //使用map比使用hashmap效率更高的原因在于重复值太多 hashmap重复值太多的极限就是链表
									//hash 此处选择较小的表进行hash 

	int len1 = v1.size();
	for (size_t i = 0; i < len1; ++i) {
		hashmap.insert(std::make_pair(v1[i][2], i));
	}
	tab_t join_Result;
	int len2 = v2.size();
	for (size_t i = 0; i < len2; ++i) {
		auto range = hashmap.equal_range(v2[i][2]);
		for (auto it = range.first; it != range.second; ++it) {
			join_Result.emplace_back(vector<int>{v1[it->second][0], v1[it->second][1], v2[i][0], v2[i][1]});
		}
	}
	return join_Result;
}

void do_my_function(tab_t&& join_Result) {

	//group by函数一

	map< int, map<int, group>> result_map
		= groupBy(join_Result.begin(), join_Result.end(), &get_t1_id2, &get_t2_id2); //615ms  map482ms
																					 //将unordered_map< int, map<int, vector<int>>>转换为vector<vector<int>>
	vector<vector<int>> groupResult = mapToVector(result_map); //20ms

	order(groupResult); //314ms

						//打印函数
	for (auto &m : groupResult)
		printf("%d,%d\n", m[2], m[3]);
}



int main() {
	//clock_t startTime = clock();


	char buffer[8192];
	setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
	cout.sync_with_stdio(false);
	cin.sync_with_stdio(false); //节省40ms左右


	FILE *fp1, *fp2;
	//fp1 = fopen("/home/web/ztedatabase/input1.csv", "r");
	//fp2 = fopen("/home/web/ztedatabase/input2.csv", "r");
	fp1 = fopen("Z:\\ZTE_CHALLENGE\\input1.csv", "r");
	fp2 = fopen("Z:\\ZTE_CHALLENGE\\input2.csv", "r");


	/*
	方法一
	tab_t v1, v2; //开一个足够大的空间，然后再减少
	int t1_id1, t1_id2, t1_id3;
	while (3 == fscanf(fp1, "%d,%d,%d", &t1_id1, &t1_id2, &t1_id3)) {
	v1.emplace_back(vector<int>{t1_id1, t1_id2, t1_id3});
	}
	int t2_id1, t2_id2, t2_id3;
	while (3 == fscanf(fp2, "%d,%d,%d", &t2_id1, &t2_id2, &t2_id3)) {
	v2.emplace_back(vector<int>{t2_id1, t2_id2, t2_id3});
	}
	*/

	/*
	int array1[2000][3];
	int array2[2000][3];
	int t1_i = 0;
	int t2_i = 0;
	while (3 == fscanf(fp1, "%d,%d,%d", &array1[t1_i][0], &array1[t1_i][1], &array1[t1_i][2])) {
	t1_i++;
	}
	while (3 == fscanf(fp2, "%d,%d,%d", &array2[t2_i][0], &array2[t2_i][1], &array2[t2_i][2])) {
	t2_i++;
	}
	*/

	fclose(fp1);
	fclose(fp2);


	//方法一：
	//do_my_function(join(array1, array2,t1_i,t2_i));
	//方法二：
	//do_my_function(join(v1, v2));
	//clock_t endTime = clock();
	//cout << "运行时间：" << double(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "毫秒" << endl;

	//system("pause");
	return 0;
}

