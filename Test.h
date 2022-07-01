#pragma once

#include<vector>
#include<random>
#include<chrono>
#include<fstream>
#include<tuple>
#include<iostream>
#include<exception>
#include<type_traits>
#include<functional>


//宏定义
#define DESIGNTOPICCONFIG using namespace LeetCode::DesignTopic;

#define REGISTRMETHOD(TYPE) TYPE::config();

#define MethodType(method) decltype(&method)
#define ClassType(method) LeetCode::DesignTopic::method_traits<std::function<MethodType(method)>>::class_t
#define RetType(method)  LeetCode::DesignTopic::method_ret_t<std::function<MethodType(method)>>
#define ArgType(method,index) LeetCode::DesignTopic::method_traits<std::function<MethodType(method)>>::arg_t<index>
#define ArgSize(method) LeetCode::DesignTopic::method_traits<std::function<MethodType(method)>>::size


namespace LeetCode {
	//生成随机vector测试用例，string类型特化
	template<typename _Ty>
	std::vector<_Ty> generateVecTest(size_t size,int left = 0,int right = 100) {
		std::vector<_Ty> vec_make(size);
		std::default_random_engine engine;
		std::uniform_int_distribution<> generator(left, right);
		for (auto& item : vec_make) {
			item = generator(engine);
		}
		return vec_make;
	}

	//string.size() is left to right
	template<>
	std::vector<std::string> generateVecTest(size_t size, int left, int right) {
		std::vector<std::string> vec_make(size);
		std::default_random_engine engine;
		std::uniform_int_distribution<> generator_char(32, 126); //ascii code
		std::uniform_int_distribution<> generator_length(left, right);
		for (auto& item : vec_make) {
			item.resize(generator_length(engine));
			for (auto& ch : item) {
				ch = generator_char(engine);
			}
		}
		return vec_make;
	}

	//Timer to benchmark
	class Timer {
	private:
		std::chrono::time_point<std::chrono::steady_clock> _start;
		std::chrono::time_point<std::chrono::steady_clock> _end;
	public:
		Timer() = default;
		Timer(Timer&) = delete;

		void Start() {
			_start = std::chrono::steady_clock::now();
		}

		void End() {
			_end = std::chrono::steady_clock::now();
		}

		friend std::ostream& operator<< (std::ostream& out, const Timer& time) {
			out << "Duration:" << std::chrono::duration_cast<std::chrono::microseconds>(time._end - time._start).count() << std::endl;
			return out;
		}

		operator int() {
			return std::chrono::duration_cast<std::chrono::microseconds>(_end - _start).count();
		}
	};

#pragma region Leetcode_DataStruct
	//二叉树
	struct TreeNode {
		int val;
		TreeNode* left;
		TreeNode* right;

		TreeNode() :val(0), left(nullptr), right(nullptr) {}
		TreeNode(int x) :val(x), left(nullptr), right(nullptr) {}
		TreeNode(int x, TreeNode* left, TreeNode* right) :val(x), left(left), right(right) {}
	};

	//链表
	struct ListNode {
		int val;
		ListNode* next;

		ListNode() :val(0), next(nullptr) {}
		ListNode(int x) :val(x), next(nullptr) {}
		ListNode(int x, ListNode* next) :val(x), next(next) {}
	};

	//测试组结构
	template<typename T>
	struct TestGroup {
		using value_type = T;

		//construtor
		TestGroup() = default;
		TestGroup(const std::vector<T>& vec):_Group(vec){}
		
		constexpr inline size_t size() { return _Group.size(); }
		constexpr inline auto begin() { return _Group.begin(); }
		constexpr inline auto end() { return _Group.end(); }

		inline void push_back(const T& data) { _Group.push_back(data); }

		explicit constexpr operator T() { return _Group[0]; }

		std::vector<T> _Group;
	};

	//
#pragma endregion

#pragma region InputParser

	//TODO:返回类型抽象———当文件中只有一行数据的时候，可以直接调用用户自定义函数，通过隐式类型转换转换成返回组唯一元素。如果多行测试用例，则使用迭代器模式迭代测试所用用例.

	//输入分析器,数组输入，二叉树输入，链表输入
	template<typename _In>
	class parser {
	public:
		using Return_Type = void;
		Return_Type parseInput(const std::string& input) {
			std::cout << "Non-implementation Type" << std::endl;
		}

		template<typename FSPath>
		Return_Type parseFile(const FSPath& filePath) {
			std::cout << "Non-implementatio Type" << std::endl;
		}
	};

	//二叉树输入分析
	template<>
	class parser<TreeNode> {
	public:
		using Return_Type = TreeNode*;

		Return_Type parseInput(const std::string& input) {
			std::string_view input_view(input.begin() + 1, input.end());
			TreeNode* root = nullptr;
			std::vector<TreeNode*> node;			
			auto substr_begin = input_view.begin();
			auto iter = input_view.begin();
			while (true) {
				if (*iter == ',' || *iter == ']') {
					auto substr = input_view.substr(substr_begin - input_view.begin(), iter - substr_begin);
					substr_begin = iter + 1;
					int value;
					auto res = std::from_chars(substr.data(), substr.data() + substr.size(), value);
					//nullptr node
					if (res.ec == std::errc::invalid_argument) {
						node.push_back(nullptr);
					}
					else {
						node.push_back(new TreeNode(value));
					}
				}
				if (++iter == input_view.end()) break;
			}
			//链接
			for (int i = 0; 2 * i + 1< node.size(); ++i) {
				if (node[i]) {
					node[i]->left = node[2 * i + 1];
					if (2 * i + 2 < node.size()) {
						node[i]->right = node[2 * i + 2];
					}
					else break;
				}
			}
			return node[0];
		}

		template<typename FSPath>
			requires requires(FSPath path) {
			std::cout << path;
		}
		Return_Type parseFile(const FSPath& filePath) {
			std::fstream file(filePath);
			//std::vector<std::vector<int>> ret_group;
			if (!file.is_open()) {
				std::cout << "Error in Opening file:" << filePath << std::endl;
				throw(std::exception("file error"));
			}
			std::string getInput;
			file >> getInput;
			return parseInput(getInput);
		}
	};

	//一维数组输入分析
	template<typename T>
	class parser<std::vector<T>> {
	public:
		using Return_Type = std::vector<T>;
		//using Return_Group = std::vector<std::vector<T>>;

		Return_Type parseInput(const std::string_view& input) {
			std::string_view input_view(input.begin() + 1, input.end());
			std::vector<T> ret;
			auto substr_begin = input_view.begin();
			auto iter = input_view.begin();
			while (true) {
				if (*iter == ',' || *iter == ']') {
					auto substr = input_view.substr(substr_begin - input_view.begin(), iter - substr_begin);
					substr_begin = iter + 1;
					int value;
					std::from_chars(substr.data(), substr.data() + substr.size(), value);
					ret.push_back(value);
				}
				if (++iter == input_view.end()) break;
			}
			return ret;
		}

		template<typename FSPath>
			requires requires(FSPath path) {
			std::cout << path;
		}
		Return_Type parseFile(const FSPath& filePath) {
			std::fstream file(filePath);
			//std::vector<std::vector<int>> ret_group;
			if (!file.is_open()) {
				std::cout << "Error in Opening file:" << filePath << std::endl;
				throw(std::exception("file error"));
			}
			std::string getInput;
			file >> getInput;
			return parseInput(getInput);
		}
	};

	//字符串文件输入
	template<>
	class parser<std::vector<std::string>> {
	public:
		using Return_Type = std::vector<std::string>;

		template<typename FSPath>
			requires requires(FSPath path) {
			std::cout << path;
		}
		Return_Type parseFile(const FSPath& filePath) {
			std::fstream file(filePath);
			if (!file.is_open()) {
				std::cout << "Error in Opening file: " << filePath << std::endl;
				throw(std::exception("file error"));
			}
			std::string getInput;
			std::vector<std::string> ret;
			bool condition = false;
			while (!file.eof()) {
				char temp = file.get();
				switch (temp) {
				case '\"':
					condition = !condition;
					break;
				case ',':
				case ']':
					//getInput.reserve(getInput.size());
					ret.push_back(getInput);
					getInput.clear();
					break;
				default:
					if(condition)
						getInput.push_back(temp);
				}
			}
			return ret;
		}
	};

	//二维数组输入分析
	template<typename T>
	class parser<std::vector<std::vector<T>>> {
	public:
		using Return_Type = std::vector<std::vector<T>>;

		Return_Type parseInput(const std::string& input) {
			std::string_view input_view(input.begin() + 1, input.end());
			std::vector<std::vector<T>> ret;
			auto subvec_begin = input_view.begin();
			auto iter = input_view.begin();
			parser<std::vector<T>> subParser;
			while (true) {
				if (*iter == ']') {
					++iter;
					ret.push_back(subParser.parseInput(std::string_view(subvec_begin, iter)));
					subvec_begin = iter + 1;
				}
				if (++iter == input_view.end()) break;
			}
			return ret;
		}

		template<typename FSPath>
			requires requires(FSPath path) {
			std::cout << path;
		}
		Return_Type parseFile(const FSPath& filePath) {
			std::fstream file(filePath);
			//std::vector<std::vector<int>> ret_group;
			if (!file.is_open()) {
				std::cout << "Error in Opening file:" << filePath << std::endl;
				throw(std::exception("file error"));
			}
			std::string getInput;
			file >> getInput;
			return parseInput(getInput);
		}
	};

	//链表输入分析
	template<>
	class parser<ListNode> {
	public:
		using Return_Type = ListNode*;

		Return_Type parseInput(const std::string& input) {
			std::string_view input_view(input.begin() + 1, input.end());
			ListNode* head = nullptr;
			auto substr_begin = input_view.begin();
			auto iter = input_view.begin();
			ListNode* p = nullptr;
			while (true) {
				if (*iter == ',' || *iter == ']') {
					auto substr = input_view.substr(substr_begin - input_view.begin(), iter - substr_begin);
					substr_begin = iter + 1;
					int value;
					std::from_chars(substr.data(), substr.data() + substr.size(), value);
					if (p == nullptr) {
						head = new ListNode(value);
						p = head;
					}
					else {
						p->next = new ListNode(value);
						p = p->next;
					}
				}
				if (++iter == input_view.end()) break;
			}
			return head;
		}

		template<typename FSPath>
			requires requires(FSPath path) {
			std::cout << path;
		}
		Return_Type parseFile(const FSPath& filePath) {
			std::fstream file(filePath);
			//std::vector<std::vector<int>> ret_group;
			if (!file.is_open()) {
				std::cout << "Error in Opening file:" << filePath << std::endl;
				throw(std::exception("file error"));
			}
			std::string getInput;
			file >> getInput;
			return parseInput(getInput);
		}
	};
#pragma endregion

	//设计题测试
	namespace DesignTopic {
		//工具
		enum class Type {
			VOID,
			CHAR,
			UCHAR,
			SHORT,
			USHORT,
			INT,
			UINT,
			LONG,
			ULONG,
			LONGLONG,
			ULONGLONG,
			FLOAT,
			DOUBLE
		};

		template<typename T>
		Type generate_type() { return Type::VOID; }
		template<>
		Type generate_type<char>() { return Type::CHAR; }
		template<>
		Type generate_type<unsigned char>() { return Type::UCHAR; }
		template<>
		Type generate_type<short>() { return Type::SHORT; }
		template<>
		Type generate_type<unsigned short>() { return Type::USHORT; }
		template<>
		Type generate_type<int>() { return Type::INT; }
		template<>
		Type generate_type<unsigned int>() { return Type::UINT; }
		template<>
		Type generate_type<long>() { return Type::LONG; }
		template<>
		Type generate_type<unsigned long>() { return Type::LONG; }
		template<>
		Type generate_type<long long>() { return Type::LONGLONG; }
		template<>
		Type generate_type<unsigned long long>() { return Type::ULONGLONG; }
		template<>
		Type generate_type<float>() { return Type::FLOAT; }
		template<>
		Type generate_type<double>() { return Type::DOUBLE; }

		//方法类型萃取，获取返回值类型，根据索引获取参数类型
		template<typename func>
		struct method_traits {};

		template<class Class, typename Return, typename... args>
		struct method_traits<std::function<Return(Class::*)(args...)>> {

			using return_t = Return;

			template<std::size_t index>
			struct arg {
				using type = std::tuple_element<index, std::tuple<args...>>::type;
			};

			static constexpr size_t size = std::tuple_size<std::tuple<args...>>::value;

			template<std::size_t index>
			using arg_t = arg<index>::type;

			using class_t = Class;

			using arg_list = std::tuple<args...>;

		};

		template<typename func>
		using method_ret_t = method_traits<func>::return_t;

		template<typename T>
		T& _get(void* p) {
			return *reinterpret_cast<T*>(p);
		}

		class ReturnPtr;
		std::unordered_map<std::string, std::function<ReturnPtr(void*, std::vector<void*>&)>> f_map; // lambda;

		template<typename T, std::size_t s>
		auto getArg(void* p) {
			/*if constexpr (std::is_same_v<std::tuple_element_t<s,T>, arg>) {
				return a;
			}
			else {
				return std::tuple_element_t<s, T>(a);
			}*/
			using arg_type = std::tuple_element_t<s, T>;
			using arg_type_without_cvr = std::remove_cvref<arg_type>::type;
			return *reinterpret_cast<arg_type_without_cvr*>(p);
		}

		template<auto func, typename Args, std::size_t... s>
		method_ret_t<std::function<decltype(func)>>
			_invoke(void* p, std::vector<void*>& args, std::index_sequence<s...>) {
			using namespace LeetCode::DesignTopic;

			using Class = method_traits<std::function<decltype(func)>>::class_t;

			return (_get<Class>(p).*func)(getArg<Args, s>(args[s])...);
		}

		template<auto func>
		constexpr auto registr(const std::string& name) {
			using namespace LeetCode::DesignTopic;
			using ret = method_ret_t<std::function<decltype(func)>>;
			constexpr auto wrapped_function = [](void* p, std::vector<void*>& arg)->ReturnPtr {
				ret* temp = new ret(_invoke<func, method_traits<std::function<decltype(func)>>::arg_list>(
					p,
					arg,
					std::make_index_sequence<method_traits<std::function<decltype(func)>>::size>{}));
				return ReturnPtr{ temp,generate_type<ret>() };
			};
			f_map.insert(make_pair(name, wrapped_function));
		}

		template<typename T>
		auto get_method(const std::string& name) {
			return f_map[name];
		}

		class ReturnPtr {
		public:
			ReturnPtr() :data(nullptr), type(Type::VOID) {}
			ReturnPtr(void* p, Type t) :data(p), type(t) {}

			constexpr Type& get_type() { return type; }
			constexpr void* get_pointer() { return data; }

			template<typename T>
			operator T() { return *static_cast<T*>(data); }

			friend std::ostream& operator<< (std::ostream& out, ReturnPtr& ret);
		private:
			LeetCode::DesignTopic::Type type;
			void* data;
		};

		std::ostream& operator<< (std::ostream& out, ReturnPtr& ret) {
			switch (ret.get_type()) {
			case Type::CHAR:
				out << char(ret);
				break;
			case Type::UCHAR:
				out << unsigned char(ret);
				break;
			case Type::SHORT:
				out << short(ret);
				break;
			case Type::USHORT:
				out << unsigned short(ret);
				break;
			case Type::INT:
				out << int(ret);
				break;
			case Type::UINT:
				out << unsigned int(ret);
				break;
			case Type::LONG:
				out << long(ret);
				break;
			case Type::ULONG:
				out << unsigned long(ret);
				break;
			case Type::LONGLONG:
				out << long long(ret);
				break;
			case Type::ULONGLONG:
				out << unsigned long long(ret);
				break;
			case Type::FLOAT:
				out << float(ret);
				break;
			case Type::DOUBLE:
				out << double(ret);
			}
			return out;
		}
	}
}