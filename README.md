# LeetCode-Utility
一个能够Leetcode 做题的工具库，主要负责解析Leetcode的字符串输入，和辅助设计题测试.

所有内容为了方便都放在了单个头文件中，你只需要将他包含进你需要debug的题目中即可。



### parser

---

leetcode中所有的输入都有相同的格式——[1,2,3,4,.......]，他们都是以[]括起来的一组输入。

你可以使用`parser<...>::parseInput(string)`来解析一组输入,通过模板参数获取你想要的输入类型，比如我想将[1,2,3,nullptr,5,6,7]输入为`TreeNode*`，那么——`parser<TreeNode>().parseInput("[1,2,3,nullptr,5,6,7]")`。

对于超大的字符串测试用例。使用文件输入。

使用`parser<...>::parseFile(path)`。



### DesignTopic

---

设计题利用了小小的反射知识。

使用方式如下

```
class Solution{
public:
	solution() = defaut;
	
	float add(float a, float b){
		return a+b;
	}
	
	//需要config来注册反射信息
	static void config(){
		DESIGNTOPICCONFIG
			registr<&Solution::add>("add");
	}
};

int main(){
	REGISTRMETHOD(Solution); //程序开始时将信息注册
	
	Solution sltion;
	Arguments args{12.f,22.f};//参数包
	auto res = get_method<Solution>("add")(&sltion,args);
	std::cout<< res <<std::endl;
	
	return 0;
}
```

