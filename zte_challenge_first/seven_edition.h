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
using namespace std;


//����ʱ�䣺0.891ms 1.764
using tab_t = vector<vector<int>>;


//����t1.id2���з���
int get_t1_id2(const vector<int> & t)
{
	return t[1];
}
//����t2.id2���з���
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
//ģ��Ĳ����ƶϹ���
template< typename Map, typename Iterator, typename F, typename... Fs >
void groupByImpl(Map & map, Iterator && current, F && f, Fs &&... fs)
{
	groupByImpl(map[f(*current)], current, std::forward< Fs >(fs)...);
}

template<typename Iterator, typename F, typename... Fs>
map< int, map<int, vector<int>>> groupBy(Iterator begin, Iterator const end, F&&f, Fs&&... fs) {

	map<int, map<int, vector<int>>>result;
	for (; begin != end; ++begin) {
		groupByImpl(result, begin, std::forward<F>(f), std::forward<Fs>(fs)...);
	}

	return result;
}


//��map< int, map<int, vector<int>>> ת��Ϊ��vector<vector<int>>
vector<vector<int>> mapToVector(map<int, map<int, vector<int>>> &result_map) {
	vector<vector<int>> result;
	for (auto it1 = result_map.begin(); it1 != result_map.end(); it1++) {
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
			result.emplace_back(vector<int>{it1->first, it2->first, it2->second[0], it2->second[1]});
		}
	}
	return result;
}

//order by����
//�Ȱ���max(t1.id1) = v1[1]����
//�ٰ���t2.id2 = v1[4]����
//�ٰ���t1.id2 = v1[2]����
static	bool comp(const vector<int>&v1, const vector<int>&v2) {
	return ((v1[2] < v2[2]) || ((v1[2] == v2[2]) && (v1[1] < v2[1])) || ((v1[2] == v2[2]) && (v1[1] == v2[1]) && (v1[0] < v2[0])));
}
void order(vector<vector<int>> &v1) {
	sort(v1.begin(), v1.end(), &comp);
}


//��ϣ���ӱ�Ƕ��ѭ�������ӿ���1�� �����˵ķ�����Ч���Ͽ���3��
tab_t  join(const tab_t&v1, const tab_t&v2) { //714ms  659ms 491ms



											  //join����
	multimap<int, size_t> hashmap;  //ʹ��map��ʹ��hashmapЧ�ʸ��ߵ�ԭ�������ظ�ֵ̫�� hashmap�ظ�ֵ̫��ļ��޾�������
									//hash �˴�ѡ���С�ı����hash 
	for (size_t i = 0; i < v1.size(); ++i) {
		hashmap.insert(std::make_pair(v1[i][2], i));
	}
	tab_t join_Result;
	for (size_t i = 0; i < v2.size(); ++i) {
		auto range = hashmap.equal_range(v2[i][2]);
		for (auto it = range.first; it != range.second; ++it) {
			join_Result.emplace_back(vector<int>{v1[it->second][0], v1[it->second][1], v2[i][0], v2[i][1]});
		}
	}

	return join_Result;
}

void do_my_function(tab_t&& join_Result) {

	//group by����
	map< int, map<int, vector<int>>> result_map
		= groupBy(join_Result.begin(), join_Result.end(), &get_t1_id2, &get_t2_id2); //615ms  map482ms

																					 //��unordered_map< int, map<int, vector<int>>>ת��Ϊvector<vector<int>>
	vector<vector<int>> groupResult = mapToVector(result_map); //20ms

															   //order by����
	order(groupResult); //314ms

						//��ӡ����
	for (auto &m : groupResult)
		printf("%d,%d\n", m[2], m[3]);

}


int main() {

	clock_t startTime = clock();
	char buffer[8192];

	setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
	//	cout.sync_with_stdio(false);
	cin.sync_with_stdio(false); //��ʡ40ms����

	ifstream in_file1("Z:\\ZTE_CHALLENGE\\input1.csv", ifstream::in); //�Զ���ʽ��
	ifstream in_file2("Z:\\ZTE_CHALLENGE\\input1.csv", ifstream::in); //�Զ���ʽ��
																	  //ifstream in_file1("/home/web/ztedatabase/input1.csv", ifstream::in); //�Զ���ʽ��
																	  //ifstream in_file2("/home/web/ztedatabase/input2.csv", ifstream::in); //�Զ���ʽ��

	tab_t v1;
	for (std::string line, number; std::getline(in_file1, line);)
	{
		vector<int>temp;
		std::istringstream iss(line);
		while (std::getline(iss, number, ','))
		{
			temp.emplace_back(std::stoi(number));
		}
		v1.emplace_back(temp);
	}

	tab_t v2;
	for (std::string line, number; std::getline(in_file2, line);)
	{
		vector<int>temp;
		std::istringstream iss(line);
		while (std::getline(iss, number, ','))
		{
			temp.emplace_back(std::stoi(number));
		}
		v2.emplace_back(temp);
	}

	//���ݶ�ȡ����63ms
	//���ݴ�����1322ms
	//���ݴ�ӡ����300ms


	do_my_function(join(v1, v2));
	clock_t endTime = clock();
	cout << "����ʱ�䣺" << double(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "����" << endl;

	system("pause");
	return 0;
}

