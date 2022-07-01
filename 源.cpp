#include"Test.h"
#include<iostream>
#include<unordered_map>
#include<stack>
using namespace std;
using namespace LeetCode::DesignTopic;

class Test {
public:
	Test(float a) :_a(a) {}
	float plus(float a, float b) { a = 1; return a + b + _a; }

	static void config() {
		DESIGNTOPICCONFIG
			registr<&Test::plus>("plus");
	}
private:
	float _a;
};

int main() {
	REGISTRMETHOD(Test);
	using namespace LeetCode::DesignTopic;

	Test a(12);
	vector<float> c = {12,12};
	vector<void*> b = { &c[0],&c[1] };
	auto ret = get_method<Test>("plus")(&a, b);
	cout << ret << endl;
	return 0;
}