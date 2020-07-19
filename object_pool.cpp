#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>

using namespace std;

template <class T> class ObjectPool {
public:
  T *Allocate() {
    if (!Freed.empty()) {
      auto t = Freed.front();
      Freed.pop();
      Dedicated.insert(t);
      return t;
    } else {
      T *t = new T;
      Dedicated.insert(t);
      return t;
    }
  }
  T *TryAllocate() {
    if (!Freed.empty()) {
      auto t = Freed.front();
      Freed.pop();
      Dedicated.insert(t);
      return t;
    } else {
      return nullptr;
    }
  }

  void Deallocate(T *object) {
    auto it = Dedicated.find(object);
    if (it == Dedicated.end()) {
      throw invalid_argument("invalid_argument");
    } else {
      Freed.push(*it);
      Dedicated.erase(it);
    }
  }

  ~ObjectPool() {
    for (T *d : Dedicated) {
      delete d;
    }
    while (!Freed.empty()) {
      delete Freed.front();
      Freed.pop();
    }
  }

private:
  set<T *> Dedicated;
  queue<T *> Freed;
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
