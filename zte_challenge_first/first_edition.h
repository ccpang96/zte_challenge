#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<fstream>
#include<algorithm>
using namespace std;

//����ʱ�䣺49.835s

class Solution {
public:
	//����һ��ʵ�������ӹ���
	vector<vector<int>> join(vector<vector<int>>&v1, vector<vector<int>>&v2) {
		vector<vector<int>>result;
		for (int i = 0; i < v1.size(); i++) {
			for (int j = 0; j < v2.size(); j++) {
				if (v1[i][2] == v2[j][2]) { //t1.id3 = t2.id3
					vector<int> result_row{ v1[i][2],v1[i][0],v1[i][1],v2[j][0],v2[j][1] };
					result.push_back(result_row);
				}
			}
		}
		return result;
	}

	//���鹦��
	vector<vector<int>> groupAndSelect(vector<vector<int>>&&v1) {
		vector<vector<int>>result;
		vector<int>visit(v1.size(), 0); //�����ж�v1��ĳrow�Ƿ񱻷��ʹ�

										//��group
		for (int i = 0; i < v1.size(); i++) {
			if (visit[i] == 1)
				continue;
			visit[i] = 1;
			int t1_id1_max = v1[i][1]; //t1������id1�����ֵ
			int t2_id1_min = v1[i][3]; //t2������id2����Сֵ
			for (int j = 0; j < v1.size(); j++) {
				if (visit[j] == 1)
					continue;
				if (v1[i][2] == v1[j][2] && v1[i][4] == v1[j][4]) { //group by t1.id2 t2.id2
					visit[j] = 1;
					t1_id1_max = max(t1_id1_max, v1[j][1]);
					t2_id1_min = min(t2_id1_min, v1[j][3]);
				}
			}
			vector<int>result_row{ v1[i][0],t1_id1_max,v1[i][2],t2_id1_min,v1[i][4] };
			result.emplace_back(result_row);
		}
		order(result);
		vector<vector<int>>output = select(result);
		return output;
	}

	//order by����
	//�Ȱ���max(t1.id1) = v1[1]����
	//�ٰ���t2.id2 = v1[4]����
	//�ٰ���t1.id2 = v1[2]����
	static	bool comp(vector<int>v1, vector<int> v2) {
		return ((v1[1] < v2[1]) || ((v1[1] == v2[1]) && (v1[4] < v2[4])) || ((v1[1] == v2[1]) && (v1[4] == v2[4]) && (v1[2] < v2[2])));
	}
	void order(vector<vector<int>> &v1) {
		sort(v1.begin(), v1.end(), &comp);
	}

	//select����
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
	//���뺯��һ�������������ļ��ܷ���ȷ��
	void check_files(ifstream& in_file, string& in_name,
		ifstream& out_file, string& out_name) {
		if (!in_file.is_open()) {
			cerr << "Cannot open input file: " << in_name << endl;
			exit(EXIT_FAILURE);
		}

		if (!out_file.is_open()) { //����һ��boolֵ��ָ����out_file�������ļ��Ƿ�ɹ�������δ�ر�
			cerr << "Cannot open output file: " << out_name << endl;
			exit(EXIT_FAILURE);
		}
	}

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

	string input1 = "/home/web/ztedatabase/input1.csv";
	string input2 = "/home/web/ztedatabase/input2.csv";
	//string input1 = "Z:\\ZTE_CHALLENGE\\zte_challenge_first\\zte_challenge_first\\input1.csv";
	//string input2 = "Z:\\ZTE_CHALLENGE\\zte_challenge_first\\zte_challenge_first\\input2.csv";
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

	vector<vector<int>> result = Solution().do_my_function(v1, v2);
	Output().printfTwoDimensionalArray(result);

	system("pause");
	return 0;
}
