# 一、题目
采用C/C++开发能模拟数据库连接(JOIN)、分组(GROUP BY)、排序(ORDER BY)功能的算法程序，正确计算出下面语句的结果
```
select max(t1.id1),min(t2.id1) from t1 join t2 on t1.id3 = t2.id3
group by t1.id2,t2.id2 order by max(t1.id1),t2.id2,t1.id2;

```

# 二、执行语句分析
```
SELECT (第七步)
    max(t1.id1),min(t2.id1) (第六步) //选t1.id1中最大的，t2.id2中最小的
FROM (第一步) 
    t1 
JOIN (第三步) //只针对外连接，比如left right
    t2 
ON (第二步)
    t1.id3 = t2.id3
GROUP BY (第五步)
    t1.id2,t2.id2 
ORDER BY (第八步)
    max(t1.id1),t2.id2,t1.id2;
```
# 三、hash join的实现
虽然是hashjoin，但是使用的是底层为红黑树的multimap。

这是因为一开始使用hashtable为底层的unordered_map作为存储键值id3的容器时，在结果上对比发现效果不如以红黑树为底层的multimap好，原因在于id3列重复值较多时，equal_range的查找是在链表上进行的，时间复杂度正比于O(n),n为重复元素的长度。而multimap是二分查找完成的，效率高一些。


 代码如下所示：
 - 先选择一个表t1插入map1，map的key为id3值，value是表的行号；
 - 然后执行probe过程，遍历表t2的id3列的每个值，在map1查找与之相同的id3值，然后t1.id1,t1.id2,t2.id1,t2.id2存入vector中。
```
	//join的过程  使用的multimap而不是unordered_map
	multimap<int, Id1Id2>map1;
	Id1Id2 s;
	int id3;

	//选择较小的表进行哈希插入
	while (3 == fscanf(fp1, "%d,%d,%d", &s.id1, &s.id2, &id3)) {
		map1.insert(std::make_pair(id3, std::move(s)));
	}

	//执行probe过程
	tab_t join_Result;
	
	while (3 == fscanf(fp2, "%d,%d,%d", &s.id1, &s.id2, &id3)) {
		auto range = map1.equal_range(id3); //在map中查找这个键值
		for (auto it = range.first; it != range.second; ++it) {
			join_Result.emplace_back(vector<int>{(it->second).id1, (it->second).id2, s.id1, s.id2});
		}
	}
```



# 四、group by的实现
group by通过使用map< int, map<int, group>>结构实现,其中外层map的key值存放的是t1.id2,内层map的存放的是t2.id2.然后遍历上述得到的join_Result，使用可变参数模板遍历整个join_Result。在传入单个参数的groupByImpl函数中，取得t1.id1的最大值，和t2.id1的最小值。
```

//实现group by函数
//根据t1.id2进行分组
inline int get_t1_id2(vector<int> & t)
{
	return t[1];
}
//根据t2.id2进行分组
inline int get_t2_id2(vector<int>& t) {
	return t[3];
}

template< typename Map, typename Iterator, typename F >
void groupByImpl(Map & map, Iterator && current, F && f)
{

	if ((*current)[0]  > map[f(*current)].max_t1_id1)
		map[f(*current)].max_t1_id1 = (*current)[0];

	if ((*current)[2] < map[f(*current)].min_t2_id1)
		map[f(*current)].min_t2_id1 = (*current)[2];
}
//可变参数模板
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
```


> 为了接下来执行order by函数，需要先将map<int,map<int,group>>结构转换为成vector<vector<int>>
```
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

```

# 五、order by的实现

order by的实现是调用STL的sort函数，然后传入一个仿函数作为第三个参数。

```

//order by函数
//先按照max(t1.id1) = v1[1]排序,再按照t2.id2 = v1[4]排序,再按照t1.id2 = v1[2]排序

static	bool comp(const vector<int>&v1, const vector<int>&v2) {
	return ((v1[2] < v2[2]) | ((v1[2] == v2[2]) & (v1[1] < v2[1])) | ((v1[2] == v2[2]) & (v1[1] == v2[1]) & (v1[0] < v2[0])));
}
void order(vector<vector<int>> &v1) {
	sort(v1.begin(), v1.end(), &comp);
}

```

# 六、得到排序后的结果打印
```
	for (auto &m : groupResult)
		printf("%d,%d\n", m[2], m[3]);
```

# 七、sql语句执行过程
# mysql的执行顺序
```
(8) SELECT 
(9) DISTINCT <select_list>
(1) FROM <left_table>
(3) <join_type> JOIN <right_table>
(2) ON <join_condition>
(4) WHERE<where_condition>
(5) GROUP BY<group_by_list
(6) WITH  {CUBE|ROLLUP}
(7) HAVING <having_condition>
(10) ORDER BY <code_by_list>
(11) LIMIT <limit_number>
```

# 各个关键字所作的内容

- FORM: 对FROM的左边的表和右边的表计算笛卡尔积。产生虚表VT1
- ON: 对虚表VT1进行ON筛选，只有那些符合<join-condition>的行才会被记录在虚表VT2中。
- JOIN： 如果指定了OUTER JOIN（比如left join、 right join），那么保留表中未匹配的行就会作为外部行添加到虚拟表VT2中，产生虚拟表VT3,  from子句中包含两个以上的表的话，那么就会对上一个join连接产生的结果VT3和下一个表重复执行步骤1~3这三个步骤，一直到处理完所有的表为止。
- WHERE： 对虚拟表VT3进行WHERE条件过滤。只有符合<where-condition>的记录才会被插入到虚拟表VT4中。
- GROUP BY: 根据group by子句中的列，对VT4中的记录进行分组操作，产生VT5. （开始使用SELECT中的别名,后面的语句都可以使用）
- avg，sum,max,min等聚合函数
- CUBE | ROLLUP: 对表VT5进行cube或者rollup操作，产生表VT6.
- HAVING： 对虚拟表VT6应用having过滤，只有符合<having-condition>的记录才会被 插入到虚拟表VT7中。
- SELECT： 执行select操作，选择指定的列，插入到虚拟表VT8中。
- DISTINCT： 对VT8中的记录进行去重。产生虚拟表VT9.
- ORDER BY: 将虚拟表VT9中的记录按照<order_by_list>进行排序操作，产生虚拟表VT10.
- LIMIT：取出指定行的记录，产生虚拟表VT11, 并将结果返回。
