#include "shared_ptr.h"
#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <utility>
#include <vector>

struct TestObject {
  int value;
  static inline int instance_count = 0;
  static inline int destructor_calls = 0;

  TestObject(int v = 0)
      : value(v) {
    ++instance_count;
  }
  ~TestObject() {
    ++destructor_calls;
    --instance_count;
  }

  static void reset_counters() {
    instance_count = 0;
    destructor_calls = 0;
  }
};

struct MultiArgObject {
  int a;
  double b;
  std::string c;

  MultiArgObject(int x, double y, std::string z)
      : a(x)
      , b(y)
      , c(std::move(z)) {}
};

TEST_CASE("shared_ptr: default constructor", "[shared_ptr]") {
  jal::shared_ptr<int> ptr;
  REQUIRE(ptr.operator->() == nullptr);
}

TEST_CASE("shared_ptr: constructor with pointer", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr(new TestObject(42));
    REQUIRE(ptr->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: copy constructor", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(42));
    REQUIRE(TestObject::instance_count == 1);

    {
      jal::shared_ptr<TestObject> ptr2(ptr1);
      REQUIRE(ptr2->value == 42);
      REQUIRE(TestObject::instance_count == 1);
      REQUIRE(TestObject::destructor_calls == 0);
    }

    REQUIRE(TestObject::instance_count == 1);
    REQUIRE(TestObject::destructor_calls == 0);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: copy assignment", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(42));
    jal::shared_ptr<TestObject> ptr2(new TestObject(99));

    REQUIRE(TestObject::instance_count == 2);

    ptr2 = ptr1;

    REQUIRE(ptr2->value == 42);
    REQUIRE(TestObject::instance_count == 1);
    REQUIRE(TestObject::destructor_calls == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 2);
}

TEST_CASE("shared_ptr: self assignment", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr(new TestObject(42));
    ptr = ptr;

    REQUIRE(ptr->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: move constructor", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(42));
    jal::shared_ptr<TestObject> ptr2(std::move(ptr1));

    REQUIRE(ptr1.operator->() == nullptr);
    REQUIRE(ptr2->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: move assignment", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(42));
    jal::shared_ptr<TestObject> ptr2(new TestObject(99));

    REQUIRE(TestObject::instance_count == 2);

    ptr2 = std::move(ptr1);

    REQUIRE(ptr1.operator->() == nullptr);
    REQUIRE(ptr2->value == 42);
    REQUIRE(TestObject::instance_count == 1);
    REQUIRE(TestObject::destructor_calls == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 2);
}

TEST_CASE("shared_ptr: self move assignment", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr(new TestObject(42));
    ptr = std::move(ptr);

    REQUIRE(ptr->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: dereference operator", "[shared_ptr]") {
  jal::shared_ptr<int> ptr(new int(42));
  REQUIRE(*ptr == 42);

  *ptr = 99;
  REQUIRE(*ptr == 99);
}

TEST_CASE("shared_ptr: arrow operator", "[shared_ptr]") {
  jal::shared_ptr<TestObject> ptr(new TestObject(42));
  REQUIRE(ptr->value == 42);

  ptr->value = 99;
  REQUIRE(ptr->value == 99);
}

TEST_CASE("shared_ptr: multiple references", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(42));
    jal::shared_ptr<TestObject> ptr2 = ptr1;
    jal::shared_ptr<TestObject> ptr3 = ptr2;
    jal::shared_ptr<TestObject> ptr4 = ptr1;

    REQUIRE(TestObject::instance_count == 1);
    REQUIRE(ptr1->value == 42);
    REQUIRE(ptr2->value == 42);
    REQUIRE(ptr3->value == 42);
    REQUIRE(ptr4->value == 42);

    ptr1->value = 99;
    REQUIRE(ptr2->value == 99);
    REQUIRE(ptr3->value == 99);
    REQUIRE(ptr4->value == 99);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: reassignment chain", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(1));
    jal::shared_ptr<TestObject> ptr2(new TestObject(2));
    jal::shared_ptr<TestObject> ptr3(new TestObject(3));

    REQUIRE(TestObject::instance_count == 3);

    ptr3 = ptr2;
    REQUIRE(TestObject::instance_count == 2);
    REQUIRE(TestObject::destructor_calls == 1);

    ptr2 = ptr1;
    REQUIRE(TestObject::instance_count == 2);
    REQUIRE(TestObject::destructor_calls == 1);

    REQUIRE(ptr1->value == 1);
    REQUIRE(ptr2->value == 1);
    REQUIRE(ptr3->value == 2);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 3);
}

TEST_CASE("make_shared: basic creation", "[make_shared]") {
  TestObject::reset_counters();

  {
    auto ptr = jal::make_shared<TestObject>(42);
    REQUIRE(ptr->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("make_shared: no arguments", "[make_shared]") {
  TestObject::reset_counters();

  {
    auto ptr = jal::make_shared<TestObject>();
    REQUIRE(ptr->value == 0);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("make_shared: multiple arguments", "[make_shared]") {
  auto ptr = jal::make_shared<MultiArgObject>(42, 3.14, "hello");
  REQUIRE(ptr->a == 42);
  REQUIRE(ptr->b == 3.14);
  REQUIRE(ptr->c == "hello");
}

TEST_CASE("make_shared: with copy", "[make_shared]") {
  TestObject::reset_counters();

  {
    auto ptr1 = jal::make_shared<TestObject>(42);
    auto ptr2 = ptr1;

    REQUIRE(TestObject::instance_count == 1);
    REQUIRE(ptr1->value == 42);
    REQUIRE(ptr2->value == 42);

    ptr2->value = 99;
    REQUIRE(ptr1->value == 99);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("make_shared: with move", "[make_shared]") {
  TestObject::reset_counters();

  {
    auto ptr1 = jal::make_shared<TestObject>(42);
    auto ptr2 = std::move(ptr1);

    REQUIRE(ptr1.operator->() == nullptr);
    REQUIRE(ptr2->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: default constructed assignment", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1;
    jal::shared_ptr<TestObject> ptr2(new TestObject(42));

    ptr1 = ptr2;

    REQUIRE(ptr1->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: assign to default constructed", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(42));
    jal::shared_ptr<TestObject> ptr2;

    ptr2 = ptr1;

    REQUIRE(ptr2->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: thread safety stress test",
          "[shared_ptr][concurrency]") {
  TestObject::reset_counters();

  constexpr int num_threads = 10;
  constexpr int operations_per_thread = 1000;

  {
    auto ptr = jal::make_shared<TestObject>(42);
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
      threads.emplace_back([ptr]() {
        for (int j = 0; j < operations_per_thread; ++j) {
          auto copy = ptr;
          REQUIRE(copy->value == 42);
        }
      });
    }

    for (auto &t : threads) {
      t.join();
    }

    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: concurrent copy and destroy",
          "[shared_ptr][concurrency]") {
  TestObject::reset_counters();

  constexpr int num_iterations = 100;

  for (int iter = 0; iter < num_iterations; ++iter) {
    auto ptr = jal::make_shared<TestObject>(42);
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) {
      threads.emplace_back([ptr]() {
        for (int j = 0; j < 100; ++j) {
          auto copy = ptr;
          std::this_thread::yield();
        }
      });
    }

    for (auto &t : threads) {
      t.join();
    }

    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("shared_ptr: move from default constructed", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1;
    jal::shared_ptr<TestObject> ptr2(new TestObject(42));

    ptr1 = std::move(ptr2);

    REQUIRE(ptr1->value == 42);
    REQUIRE(ptr2.operator->() == nullptr);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("shared_ptr: complex ownership transfer", "[shared_ptr]") {
  TestObject::reset_counters();

  {
    jal::shared_ptr<TestObject> ptr1(new TestObject(1));
    jal::shared_ptr<TestObject> ptr2(new TestObject(2));
    jal::shared_ptr<TestObject> ptr3 = ptr1;

    REQUIRE(TestObject::instance_count == 2);

    ptr1 = ptr2;

    REQUIRE(TestObject::instance_count == 2);
    REQUIRE(ptr1->value == 2);
    REQUIRE(ptr2->value == 2);
    REQUIRE(ptr3->value == 1);

    ptr2 = ptr3;

    REQUIRE(TestObject::instance_count == 2);
    REQUIRE(ptr1->value == 2);
    REQUIRE(ptr2->value == 1);
    REQUIRE(ptr3->value == 1);

    ptr3 = ptr1;

    REQUIRE(TestObject::instance_count == 2);
    REQUIRE(TestObject::destructor_calls == 0);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 2);
}
