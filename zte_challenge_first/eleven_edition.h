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
//#pragma GCC optimize("O2")
using namespace std;



//�汾�ģ����˷���������ʱ�䣺0.526
using tab_t = vector<vector<int>>;

struct group {
	int max_t1_id1 = -2147483647;
	int min_t2_id1 = 2147483647;
};



//ʵ��group by����
//����t1.id2���з���
int get_t1_id2(vector<int> & t)
{
	return t[1];
}
//����t2.id2���з���
int get_t2_id2(vector<int>& t) {
	return t[3];
}

template< typename Map, typename Iterator, typename F >
void groupByImpl(Map & map, Iterator && current, F && f)
{

	map[f(*current)].max_t1_id1 = max(map[f(*current)].max_t1_id1, (*current)[0]);
	map[f(*current)].min_t2_id1 = min(map[f(*current)].min_t2_id1, (*current)[2]);

}
//�ɱ����ģ��
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


//��map< int, map<int, vector<int>>> ת��Ϊ��vector<vector<int>>
vector<vector<int>> mapToVector(map<int, map<int, group>> &result_map) {
	vector<vector<int>> result;
	for (auto it1 = result_map.begin(); it1 != result_map.end(); it1++) {
		for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {

			result.emplace_back(vector<int>{it1->first, it2->first, (it2->second).max_t1_id1, (it2->second).min_t2_id1});
		}
	}
	return result;
}


//order by����
//�Ȱ���max(t1.id1) = v1[1]����
//�ٰ���t2.id2 = v1[4]����
//�ٰ���t1.id2 = v1[2]����
static	bool comp(const vector<int>&v1, const vector<int>&v2) {
	return ((v1[2] < v2[2]) | ((v1[2] == v2[2]) & (v1[1] < v2[1])) | ((v1[2] == v2[2]) & (v1[1] == v2[1]) & (v1[0] < v2[0])));
}
void order(vector<vector<int>> &v1) {
	sort(v1.begin(), v1.end(), &comp);
}

//��ӡ��ά���飺�൱��
void printfTwoDimensionalArray(vector<vector<int>>&result) {
	if (result.size() == 0)
		cout << " " << '\n';
	for (int index = 0; index != result.size(); index++) {
		string output;
		for (int j = 0; j < result[index].size(); j++) {
			output += std::to_string(result[index][j]) + ",";
		}
		cout << output.substr(0, output.length() - 1) << '\n';;
	}
}


//join����
//��ϣ���ӱ�Ƕ��ѭ�������ӿ���1�� �����˵ķ�����Ч���Ͽ���3��
tab_t  join(tab_t & v1, tab_t & v2) { //714ms  659ms 491ms
									  //join����
	multimap<int, size_t> hashmap;  //ʹ��multimap��ʹ��unordered_multimapЧ�ʸ��ߵ�ԭ�������ظ�ֵ̫�� hashmap�ظ�ֵ̫��ļ��޾�������

									//ѡ��һ����С�ı���й�ϣӳ��
	const int len1 = v1.size();
	for (size_t i = 0; i != len1; ++i) {
		hashmap.insert(std::make_pair(v1[i][2], i));
	}

	//ִ��probe����
	tab_t join_Result;
	const int len2 = v2.size();
	for (size_t i = 0; i != len2; ++i) {
		auto range = hashmap.equal_range(v2[i][2]);
		for (auto it = range.first; it != range.second; ++it) {
			join_Result.emplace_back(vector<int>{v1[it->second][0], v1[it->second][1], v2[i][0], v2[i][1]});
		}
	}
	return join_Result;
}



void do_my_function(tab_t&& join_Result) {

	//��ִ��join

	//��ִ��group by
	map< int, map<int, group>> result_map
		= groupBy(join_Result.begin(), join_Result.end(), &get_t1_id2, &get_t2_id2); //615ms  map482ms

																					 //��unordered_map< int, map<int, vector<int>>>ת��Ϊvector<vector<int>>
	vector<vector<int>> groupResult = mapToVector(result_map); //20ms
	order(groupResult); //314ms

						//��ӡ���� ��
	for (auto &m : groupResult) {
		/*
		write(m[2]);
		putchar(',');
		write(m[3]);
		putchar('\n');
		*/
		printf("%d,%d\n", m[2], m[3]);
	}

}


struct Id1Id2 {
public:
	int id1;
	int id2;
};
int main() {

	clock_t startTime = clock();
	char buffer[8092];
	setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
	cout.sync_with_stdio(false);
	cin.sync_with_stdio(false); //��ʡ40ms����

								//������������
	tab_t v1, v2; //��һ���㹻��Ŀռ䣬Ȼ���ټ���
	FILE *fp1, *fp2;
	//fp1 = fopen("/home/web/ztedatabase/input1.csv", "r");
	//fp2 = fopen("/home/web/ztedatabase/input2.csv", "r");
	fp1 = fopen("Z:\\ZTE_CHALLENGE\\input1.csv", "r");
	fp2 = fopen("Z:\\ZTE_CHALLENGE\\input2.csv", "r");


	multimap<int, Id1Id2>map1;
	Id1Id2 s;
	int id3;
	while (3 == fscanf(fp1, "%d,%d,%d", &s.id1, &s.id2, &id3)) {
		map1.insert(std::make_pair(id3, std::move(s)));
	}
	//ִ��probe����
	tab_t join_Result;

	while (3 == fscanf(fp2, "%d,%d,%d", &s.id1, &s.id2, &id3)) {
		auto range = map1.equal_range(id3); //��map�в��������ֵ
		for (auto it = range.first; it != range.second; ++it) {
			join_Result.emplace_back(vector<int>{(it->second).id1, (it->second).id2, s.id1, s.id2});
		}
	}

	
	//ִ��sql���
	do_my_function(std::move(join_Result));


	clock_t endTime = clock();
	cout << "����ʱ�䣺" << double(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "����" << endl;

	system("pause");
	return 0;
}

