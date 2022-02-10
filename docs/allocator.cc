#include <cstdio>

#include <string>
#include <vector>

class Test {
 public:
  Test(const std::string& name) : name_(name) { printf("construct %s\n", name_.c_str()); }
  ~Test() { printf("destroy %s\n", name_.c_str()); }

 private:
  std::string name_;
};

int main(int argc, char* argv[]) {
  {
    // 代码1
    auto* t = new Test("new_test");
    delete t;
  }
  {
    // 代码2
    void* p = ::operator new(sizeof(Test));
    auto* t = reinterpret_cast<Test*>(p);
    t->Test::Test("allocator_test");
    // auto* t = new (p) Test("allocator_test"); // placement new
    t->~Test();
    ::operator delete(p);
  }
  return 0;
}