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

//����ʱ�䣺1.686��
using tab_t = vector<vector<int>>;


//��ϣ���ӱ�Ƕ��ѭ�������ӿ���1�� �����˵ķ�����Ч���Ͽ���3��
tab_t join(const tab_t&v1, const tab_t&v2) {
	unordered_multimap<int, size_t> hashmap;
	//hash �˴�ѡ���С�ı����hash 
	for (size_t i = 0; i < v1.size(); ++i) {
		hashmap.insert(std::make_pair(v1[i][2], i));
	}
	tab_t result;
	for (size_t i = 0; i < v2.size(); ++i) {
		auto range = hashmap.equal_range(v2[i][2]);
		for (auto it = range.first; it != range.second; ++it) {
			tab_t::value_type row;
			//row.insert(row.end(), v2[i][2]);
			row.insert(row.end(), v1[it->second].begin(), v1[it->second].end() - 1);
			row.insert(row.end(), v2[i].begin(), v2[i].end() - 1);
			result.emplace_back(std::move(row));
		}
	}
	return result;
}

//Ƕ�׹�ϣ+�ɱ����ģ��ʵ�ְ����з���,��ԭ����visit����������10��
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
template< typename Map, typename Iterator, typename F, typename... Fs >
void groupByImpl(Map & map, Iterator && current, F && f, Fs &&... fs)
{
	groupByImpl(map[f(*current)], current, std::forward< Fs >(fs)...);
}
template<typename Iterator, typename F, typename... Fs>
unordered_map< int, unordered_map<int, vector<int>>>
groupBy(Iterator begin, Iterator const end, F&&f, Fs&&... fs) {
	unordered_map<int, unordered_map<int, vector<int>>>result;
	for (; begin != end; ++begin) {
		groupByImpl(result, begin, std::forward<F>(f), std::forward<Fs>(fs)...);
	}
	return result;
}

//��unordered_map< int, unordered_map<int, vector<int>>> ת��Ϊ��vector<vector<int>>
vector<vector<int>> mapToVector(unordered_map< int, unordered_map<int, vector<int>>> &result_map) {
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
static	bool comp(vector<int>v1, vector<int> v2) {
	return ((v1[2] < v2[2]) || ((v1[2] == v2[2]) && (v1[1] < v2[1])) || ((v1[2] == v2[2]) && (v1[1] == v2[1]) && (v1[0] < v2[0])));
}
void order(vector<vector<int>> &v1) {
	sort(v1.begin(), v1.end(), &comp);
}

//select����
vector<vector<int>> select(vector<vector<int>>&v1) {
	vector<vector<int>>result;
	for (int i = 0; i < v1.size(); i++) {
		vector<int> row = vector<int>{ v1[i][2],v1[i][3] };
		result.push_back(row);
	}
	return result;
}

//���鹦��  16033ms  //����ʱ��663ms
tab_t groupAndSelect(tab_t &&v1) {

	unordered_map< int, unordered_map<int, vector<int>>> result_map
		= groupBy(v1.begin(), v1.end(), &get_t1_id2, &get_t2_id2);

	//��mapת��Ϊvector<vector<int>>
	vector<vector<int>> result = mapToVector(result_map);


	//�����ѡ��ֻ����343ms
	order(result);
	vector<vector<int>>output = select(result);
	return output;
}



vector<vector<int>> do_my_function(vector<vector<int>>&v1, vector<vector<int>>&v2) {
	vector<vector<int>>result;
	result = groupAndSelect(join(v1, v2));
	return result;
}


class Input {
public:
	//���뺯�������������һ������ת��Ϊvectors
	vector<int> stringToIntegerVector(string &input) {
		vector<int> output;
		stringstream ss(input);
		char delim = ','; //�ָ���
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
	//clock_t startTime = clock();
	string input1 = "/home/web/ztedatabase/input1.csv";
	string input2 = "/home/web/ztedatabase/input2.csv";
	//string input1 = "Z:\\ZTE_CHALLENGE\\input1.csv";
	//string input2 = "Z:\\ZTE_CHALLENGE\\input2.csv";
	ifstream in_file1(input1.c_str(), ifstream::in); //�Զ���ʽ��
	ifstream in_file2(input2.c_str(), ifstream::in); //�Զ���ʽ��
													 //Input().check_files(in_file1, input1, in_file2, input2); //����ļ��ܷ��
	string line;
	//����1�洢����
	vector<vector<int>>v1;
	while (getline(in_file1, line)) {
		vector<int> row = Input().stringToIntegerVector(line);
		v1.emplace_back(row);
	}
	//����2�洢����
	vector<vector<int>>v2;
	while (getline(in_file2, line)) {
		vector<int> row = Input().stringToIntegerVector(line);
		v2.emplace_back(row);
	}

	//���ݶ�ȡ����115ms

	vector<vector<int>> result = do_my_function(v1, v2);
	Output().printfTwoDimensionalArray(result); //��ӡ����726ms
	//clock_t endTime = clock();
	//cout << "����ʱ�䣺" << double(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "����" << endl;

	system("pause");
	return 0;
}


