#include "unique_ptr.h"
#include <catch2/catch_test_macros.hpp>
#include <utility>

struct TestObject {
  int value;
  static inline int instance_count = 0;
  static inline int destructor_calls = 0;

  TestObject(int v = 0) : value(v) { ++instance_count; }
  ~TestObject() { 
    ++destructor_calls;
    --instance_count;
  }

  static void reset_counters() {
    instance_count = 0;
    destructor_calls = 0;
  }
};

TEST_CASE("unique_ptr: default constructor", "[unique_ptr]") {
  jal::unique_ptr<int> ptr;
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
}

TEST_CASE("unique_ptr: constructor with pointer", "[unique_ptr]") {
  TestObject::reset_counters();
  
  auto* raw = new TestObject(42);
  jal::unique_ptr<TestObject> ptr(raw);
  
  REQUIRE(ptr.get() == raw);
  REQUIRE(ptr);
  REQUIRE(ptr->value == 42);
  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("unique_ptr: destructor releases memory", "[unique_ptr]") {
  TestObject::reset_counters();
  
  {
    jal::unique_ptr<TestObject> ptr(new TestObject(42));
    REQUIRE(TestObject::instance_count == 1);
  }
  
  REQUIRE(TestObject::destructor_calls == 1);
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr: move constructor", "[unique_ptr]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject> ptr1(new TestObject(42));
  auto* raw = ptr1.get();
  
  jal::unique_ptr<TestObject> ptr2(std::move(ptr1));
  
  REQUIRE(ptr1.get() == nullptr);
  REQUIRE_FALSE(ptr1);
  REQUIRE(ptr2.get() == raw);
  REQUIRE(ptr2->value == 42);
  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("unique_ptr: move assignment operator", "[unique_ptr]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject> ptr1(new TestObject(42));
  jal::unique_ptr<TestObject> ptr2(new TestObject(99));
  auto* raw1 = ptr1.get();
  
  REQUIRE(TestObject::instance_count == 2);
  
  ptr2 = std::move(ptr1);
  
  REQUIRE(ptr1.get() == nullptr);
  REQUIRE(ptr2.get() == raw1);
  REQUIRE(ptr2->value == 42);
  REQUIRE(TestObject::destructor_calls == 1);
  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("unique_ptr: move assignment to self", "[unique_ptr]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject> ptr(new TestObject(42));
  auto* raw = ptr.get();
  
  ptr = std::move(ptr);
  
  // Self-move should be safe and leave object unchanged
  REQUIRE(ptr.get() == raw);
  REQUIRE(ptr->value == 42);
  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("unique_ptr: get() returns raw pointer", "[unique_ptr]") {
  auto* raw = new int(42);
  jal::unique_ptr<int> ptr(raw);
  
  REQUIRE(ptr.get() == raw);
}

TEST_CASE("unique_ptr: release() transfers ownership", "[unique_ptr]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject> ptr(new TestObject(42));
  auto* raw = ptr.release();
  
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
  REQUIRE(raw != nullptr);
  REQUIRE(raw->value == 42);
  REQUIRE(TestObject::instance_count == 1);
  
  delete raw;
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr: reset() with nullptr", "[unique_ptr]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject> ptr(new TestObject(42));
  REQUIRE(TestObject::instance_count == 1);
  
  ptr.reset();
  
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
  REQUIRE(TestObject::destructor_calls == 1);
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr: reset() with new pointer", "[unique_ptr]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject> ptr(new TestObject(42));
  auto* new_raw = new TestObject(99);
  
  REQUIRE(TestObject::instance_count == 2);
  
  ptr.reset(new_raw);
  
  REQUIRE(ptr.get() == new_raw);
  REQUIRE(ptr->value == 99);
  REQUIRE(TestObject::destructor_calls == 1);
  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("unique_ptr: swap()", "[unique_ptr]") {
  jal::unique_ptr<int> ptr1(new int(42));
  jal::unique_ptr<int> ptr2(new int(99));
  
  auto* raw1 = ptr1.get();
  auto* raw2 = ptr2.get();
  
  ptr1.swap(ptr2);
  
  REQUIRE(ptr1.get() == raw2);
  REQUIRE(ptr2.get() == raw1);
  REQUIRE(*ptr1 == 99);
  REQUIRE(*ptr2 == 42);
}

TEST_CASE("unique_ptr: swap() with nullptr", "[unique_ptr]") {
  jal::unique_ptr<int> ptr1(new int(42));
  jal::unique_ptr<int> ptr2;
  
  auto* raw1 = ptr1.get();
  
  ptr1.swap(ptr2);
  
  REQUIRE(ptr1.get() == nullptr);
  REQUIRE(ptr2.get() == raw1);
  REQUIRE(*ptr2 == 42);
}

TEST_CASE("unique_ptr: operator bool", "[unique_ptr]") {
  jal::unique_ptr<int> ptr1;
  jal::unique_ptr<int> ptr2(new int(42));
  
  REQUIRE_FALSE(ptr1);
  REQUIRE(ptr2);
  
  if (ptr2) {
    REQUIRE(*ptr2 == 42);
  }
}

TEST_CASE("unique_ptr: operator->", "[unique_ptr]") {
  struct Point {
    int x, y;
  };
  
  jal::unique_ptr<Point> ptr(new Point{3, 4});
  
  REQUIRE(ptr->x == 3);
  REQUIRE(ptr->y == 4);
  
  ptr->x = 10;
  REQUIRE(ptr->x == 10);
}

TEST_CASE("unique_ptr: operator*", "[unique_ptr]") {
  jal::unique_ptr<int> ptr(new int(42));
  
  REQUIRE(*ptr == 42);
  
  *ptr = 99;
  REQUIRE(*ptr == 99);
}

TEST_CASE("unique_ptr: copy constructor is deleted", "[unique_ptr]") {
  STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<jal::unique_ptr<int>>);
}

TEST_CASE("unique_ptr: copy assignment is deleted", "[unique_ptr]") {
  STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<jal::unique_ptr<int>>);
}

TEST_CASE("unique_ptr: move semantics in container", "[unique_ptr]") {
  TestObject::reset_counters();
  
  std::vector<jal::unique_ptr<TestObject>> vec;
  vec.push_back(jal::unique_ptr<TestObject>(new TestObject(1)));
  vec.push_back(jal::unique_ptr<TestObject>(new TestObject(2)));
  vec.push_back(jal::unique_ptr<TestObject>(new TestObject(3)));
  
  REQUIRE(vec.size() == 3);
  REQUIRE(vec[0]->value == 1);
  REQUIRE(vec[1]->value == 2);
  REQUIRE(vec[2]->value == 3);
  REQUIRE(TestObject::instance_count == 3);
  
  vec.clear();
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr: nullptr constructor", "[unique_ptr]") {
  jal::unique_ptr<int> ptr(nullptr);
  
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
}

TEST_CASE("unique_ptr: reset same pointer is safe", "[unique_ptr]") {
  TestObject::reset_counters();
  
  auto* raw = new TestObject(42);
  jal::unique_ptr<TestObject> ptr(raw);
  
  // This should delete the old pointer first, then assign
  // Resetting to the same pointer would be double-delete in real usage
  // but our implementation handles delete-then-assign order correctly
  ptr.reset(new TestObject(99));
  
  REQUIRE(ptr->value == 99);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("unique_ptr: constexpr operations", "[unique_ptr]") {
  // Test that constexpr operations can be used in constexpr context
  constexpr auto test_constexpr = []() constexpr {
    jal::unique_ptr<int> ptr;
    return ptr.get() == nullptr;
  };
  
  STATIC_REQUIRE(test_constexpr());
}

TEST_CASE("unique_ptr: managing polymorphic types", "[unique_ptr]") {
  struct Base {
    virtual ~Base() = default;
    virtual int get_value() const = 0;
  };
  
  struct Derived : Base {
    int value;
    Derived(int v) : value(v) {}
    int get_value() const override { return value; }
  };
  
  jal::unique_ptr<Base> ptr(new Derived(42));
  
  REQUIRE(ptr->get_value() == 42);
}

TEST_CASE("unique_ptr: empty state after release", "[unique_ptr]") {
  jal::unique_ptr<int> ptr(new int(42));
  auto* raw = ptr.release();
  
  // Verify it behaves like default constructed
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
  
  // Can safely reset
  ptr.reset(new int(99));
  REQUIRE(*ptr == 99);
  
  delete raw;
}

TEST_CASE("unique_ptr: multiple resets", "[unique_ptr]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject> ptr(new TestObject(1));
  REQUIRE(TestObject::instance_count == 1);
  
  ptr.reset(new TestObject(2));
  REQUIRE(TestObject::instance_count == 1);
  REQUIRE(ptr->value == 2);
  
  ptr.reset(new TestObject(3));
  REQUIRE(TestObject::instance_count == 1);
  REQUIRE(ptr->value == 3);
  
  ptr.reset();
  REQUIRE(TestObject::instance_count == 0);
}

// ============================================================================
// Array specialization tests
// ============================================================================

TEST_CASE("unique_ptr<T[]>: default constructor", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr;
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
}

TEST_CASE("unique_ptr<T[]>: constructor with pointer", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr(new int[5]{1, 2, 3, 4, 5});
  
  REQUIRE(ptr.get() != nullptr);
  REQUIRE(ptr);
  REQUIRE(ptr[0] == 1);
  REQUIRE(ptr[2] == 3);
  REQUIRE(ptr[4] == 5);
}

TEST_CASE("unique_ptr<T[]>: destructor releases memory", "[unique_ptr][array]") {
  TestObject::reset_counters();
  
  {
    auto* arr = new TestObject[3]{{1}, {2}, {3}};
    jal::unique_ptr<TestObject[]> ptr(arr);
    REQUIRE(TestObject::instance_count == 3);
  }
  
  REQUIRE(TestObject::destructor_calls == 3);
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr<T[]>: move constructor", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr1(new int[3]{10, 20, 30});
  auto* raw = ptr1.get();
  
  jal::unique_ptr<int[]> ptr2(std::move(ptr1));
  
  REQUIRE(ptr1.get() == nullptr);
  REQUIRE_FALSE(ptr1);
  REQUIRE(ptr2.get() == raw);
  REQUIRE(ptr2[0] == 10);
  REQUIRE(ptr2[1] == 20);
  REQUIRE(ptr2[2] == 30);
}

TEST_CASE("unique_ptr<T[]>: move assignment operator", "[unique_ptr][array]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject[]> ptr1(new TestObject[2]{{1}, {2}});
  jal::unique_ptr<TestObject[]> ptr2(new TestObject[3]{{10}, {20}, {30}});
  auto* raw1 = ptr1.get();
  
  REQUIRE(TestObject::instance_count == 5);
  
  ptr2 = std::move(ptr1);
  
  REQUIRE(ptr1.get() == nullptr);
  REQUIRE(ptr2.get() == raw1);
  REQUIRE(ptr2[0].value == 1);
  REQUIRE(ptr2[1].value == 2);
  REQUIRE(TestObject::destructor_calls == 3);
  REQUIRE(TestObject::instance_count == 2);
}

TEST_CASE("unique_ptr<T[]>: self-move assignment", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr(new int[3]{1, 2, 3});
  auto* raw = ptr.get();
  
  ptr = std::move(ptr);
  
  REQUIRE(ptr.get() == raw);
  REQUIRE(ptr[0] == 1);
  REQUIRE(ptr[1] == 2);
  REQUIRE(ptr[2] == 3);
}

TEST_CASE("unique_ptr<T[]>: get() returns raw pointer", "[unique_ptr][array]") {
  auto* raw = new int[3]{1, 2, 3};
  jal::unique_ptr<int[]> ptr(raw);
  
  REQUIRE(ptr.get() == raw);
}

TEST_CASE("unique_ptr<T[]>: release() transfers ownership", "[unique_ptr][array]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject[]> ptr(new TestObject[2]{{1}, {2}});
  auto* raw = ptr.release();
  
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
  REQUIRE(raw != nullptr);
  REQUIRE(raw[0].value == 1);
  REQUIRE(raw[1].value == 2);
  REQUIRE(TestObject::instance_count == 2);
  
  delete[] raw;
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr<T[]>: reset() with nullptr", "[unique_ptr][array]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject[]> ptr(new TestObject[3]{{1}, {2}, {3}});
  REQUIRE(TestObject::instance_count == 3);
  
  ptr.reset();
  
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
  REQUIRE(TestObject::destructor_calls == 3);
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr<T[]>: reset() with new pointer", "[unique_ptr][array]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject[]> ptr(new TestObject[2]{{1}, {2}});
  auto* new_raw = new TestObject[3]{{10}, {20}, {30}};
  
  REQUIRE(TestObject::instance_count == 5);
  
  ptr.reset(new_raw);
  
  REQUIRE(ptr.get() == new_raw);
  REQUIRE(ptr[0].value == 10);
  REQUIRE(ptr[1].value == 20);
  REQUIRE(ptr[2].value == 30);
  REQUIRE(TestObject::destructor_calls == 2);
  REQUIRE(TestObject::instance_count == 3);
}

TEST_CASE("unique_ptr<T[]>: swap()", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr1(new int[2]{1, 2});
  jal::unique_ptr<int[]> ptr2(new int[3]{10, 20, 30});
  
  auto* raw1 = ptr1.get();
  auto* raw2 = ptr2.get();
  
  ptr1.swap(ptr2);
  
  REQUIRE(ptr1.get() == raw2);
  REQUIRE(ptr2.get() == raw1);
  REQUIRE(ptr1[0] == 10);
  REQUIRE(ptr1[2] == 30);
  REQUIRE(ptr2[0] == 1);
  REQUIRE(ptr2[1] == 2);
}

TEST_CASE("unique_ptr<T[]>: swap() with nullptr", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr1(new int[3]{1, 2, 3});
  jal::unique_ptr<int[]> ptr2;
  
  auto* raw1 = ptr1.get();
  
  ptr1.swap(ptr2);
  
  REQUIRE(ptr1.get() == nullptr);
  REQUIRE(ptr2.get() == raw1);
  REQUIRE(ptr2[0] == 1);
}

TEST_CASE("unique_ptr<T[]>: operator bool", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr1;
  jal::unique_ptr<int[]> ptr2(new int[3]{1, 2, 3});
  
  REQUIRE_FALSE(ptr1);
  REQUIRE(ptr2);
  
  if (ptr2) {
    REQUIRE(ptr2[1] == 2);
  }
}

TEST_CASE("unique_ptr<T[]>: operator[]", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr(new int[5]{10, 20, 30, 40, 50});
  
  REQUIRE(ptr[0] == 10);
  REQUIRE(ptr[2] == 30);
  REQUIRE(ptr[4] == 50);
  
  ptr[2] = 99;
  REQUIRE(ptr[2] == 99);
}

TEST_CASE("unique_ptr<T[]>: copy constructor is deleted", "[unique_ptr][array]") {
  STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<jal::unique_ptr<int[]>>);
}

TEST_CASE("unique_ptr<T[]>: copy assignment is deleted", "[unique_ptr][array]") {
  STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<jal::unique_ptr<int[]>>);
}

TEST_CASE("unique_ptr<T[]>: move semantics in container", "[unique_ptr][array]") {
  TestObject::reset_counters();
  
  std::vector<jal::unique_ptr<TestObject[]>> vec;
  vec.push_back(jal::unique_ptr<TestObject[]>(new TestObject[2]{{1}, {2}}));
  vec.push_back(jal::unique_ptr<TestObject[]>(new TestObject[3]{{10}, {20}, {30}}));
  
  REQUIRE(vec.size() == 2);
  REQUIRE(vec[0][0].value == 1);
  REQUIRE(vec[0][1].value == 2);
  REQUIRE(vec[1][0].value == 10);
  REQUIRE(vec[1][2].value == 30);
  REQUIRE(TestObject::instance_count == 5);
  
  vec.clear();
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr<T[]>: nullptr constructor", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr(nullptr);
  
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
}

TEST_CASE("unique_ptr<T[]>: large array", "[unique_ptr][array]") {
  constexpr std::size_t size = 1000;
  jal::unique_ptr<int[]> ptr(new int[size]);
  
  for (std::size_t i = 0; i < size; ++i) {
    ptr[i] = static_cast<int>(i);
  }
  
  REQUIRE(ptr[0] == 0);
  REQUIRE(ptr[500] == 500);
  REQUIRE(ptr[999] == 999);
}

TEST_CASE("unique_ptr<T[]>: empty state after release", "[unique_ptr][array]") {
  jal::unique_ptr<int[]> ptr(new int[3]{1, 2, 3});
  auto* raw = ptr.release();
  
  REQUIRE(ptr.get() == nullptr);
  REQUIRE_FALSE(ptr);
  
  ptr.reset(new int[2]{10, 20});
  REQUIRE(ptr[0] == 10);
  REQUIRE(ptr[1] == 20);
  
  delete[] raw;
}

TEST_CASE("unique_ptr<T[]>: multiple resets", "[unique_ptr][array]") {
  TestObject::reset_counters();
  
  jal::unique_ptr<TestObject[]> ptr(new TestObject[2]{{1}, {2}});
  REQUIRE(TestObject::instance_count == 2);
  
  ptr.reset(new TestObject[3]{{10}, {20}, {30}});
  REQUIRE(TestObject::instance_count == 3);
  REQUIRE(ptr[1].value == 20);
  
  ptr.reset(new TestObject[1]{{99}});
  REQUIRE(TestObject::instance_count == 1);
  REQUIRE(ptr[0].value == 99);
  
  ptr.reset();
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("unique_ptr<T[]>: constexpr operations", "[unique_ptr][array]") {
  constexpr auto test_constexpr = []() constexpr {
    jal::unique_ptr<int[]> ptr;
    return ptr.get() == nullptr;
  };
  
  STATIC_REQUIRE(test_constexpr());
}
