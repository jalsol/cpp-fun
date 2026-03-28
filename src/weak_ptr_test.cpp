#include "shared_ptr.h"
#include "weak_ptr.h"
#include <catch2/catch_test_macros.hpp>
#include <thread>
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

TEST_CASE("weak_ptr: default constructor", "[weak_ptr]") {
  jal::weak_ptr<int> wptr;
  REQUIRE(wptr.expired());
}

TEST_CASE("weak_ptr: construct from shared_ptr", "[weak_ptr]") {
  TestObject::reset_counters();

  {
    auto sptr = jal::make_shared<TestObject>(42);
    jal::weak_ptr<TestObject> wptr(sptr);

    REQUIRE_FALSE(wptr.expired());
    REQUIRE(TestObject::instance_count == 1);

    auto sptr2 = wptr.lock();
    REQUIRE(sptr2->value == 42);
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("weak_ptr: expired after shared_ptr destroyed", "[weak_ptr]") {
  TestObject::reset_counters();

  jal::weak_ptr<TestObject> wptr;

  {
    auto sptr = jal::make_shared<TestObject>(42);
    wptr = sptr;
    REQUIRE_FALSE(wptr.expired());
  }

  REQUIRE(wptr.expired());
  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(TestObject::destructor_calls == 1);
}

TEST_CASE("weak_ptr: lock returns empty when expired", "[weak_ptr]") {
  TestObject::reset_counters();

  jal::weak_ptr<TestObject> wptr;

  {
    auto sptr = jal::make_shared<TestObject>(42);
    wptr = sptr;
  }

  REQUIRE(wptr.expired());
  auto locked = wptr.lock();
  REQUIRE(locked.operator->() == nullptr);
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("weak_ptr: prevents dangling reference", "[weak_ptr]") {
  TestObject::reset_counters();

  jal::weak_ptr<TestObject> wptr;

  {
    auto sptr = jal::make_shared<TestObject>(42);
    wptr = sptr;
    REQUIRE(TestObject::instance_count == 1);
  }

  REQUIRE(TestObject::instance_count == 0);
  REQUIRE(wptr.expired());

  auto locked = wptr.lock();
  REQUIRE(locked.operator->() == nullptr);
}

TEST_CASE("weak_ptr: copy constructor", "[weak_ptr]") {
  TestObject::reset_counters();

  auto sptr = jal::make_shared<TestObject>(42);
  jal::weak_ptr<TestObject> wptr1(sptr);
  jal::weak_ptr<TestObject> wptr2(wptr1);

  REQUIRE_FALSE(wptr1.expired());
  REQUIRE_FALSE(wptr2.expired());

  auto locked1 = wptr1.lock();
  auto locked2 = wptr2.lock();

  REQUIRE(locked1->value == 42);
  REQUIRE(locked2->value == 42);
  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("weak_ptr: copy assignment", "[weak_ptr]") {
  TestObject::reset_counters();

  auto sptr = jal::make_shared<TestObject>(42);
  jal::weak_ptr<TestObject> wptr1(sptr);
  jal::weak_ptr<TestObject> wptr2;

  wptr2 = wptr1;

  REQUIRE_FALSE(wptr2.expired());
  auto locked = wptr2.lock();
  REQUIRE(locked->value == 42);
}

TEST_CASE("weak_ptr: move constructor", "[weak_ptr]") {
  TestObject::reset_counters();

  auto sptr = jal::make_shared<TestObject>(42);
  jal::weak_ptr<TestObject> wptr1(sptr);
  jal::weak_ptr<TestObject> wptr2(std::move(wptr1));

  REQUIRE(wptr1.expired());
  REQUIRE_FALSE(wptr2.expired());

  auto locked = wptr2.lock();
  REQUIRE(locked->value == 42);
}

TEST_CASE("weak_ptr: move assignment", "[weak_ptr]") {
  TestObject::reset_counters();

  auto sptr = jal::make_shared<TestObject>(42);
  jal::weak_ptr<TestObject> wptr1(sptr);
  jal::weak_ptr<TestObject> wptr2;

  wptr2 = std::move(wptr1);

  REQUIRE(wptr1.expired());
  REQUIRE_FALSE(wptr2.expired());

  auto locked = wptr2.lock();
  REQUIRE(locked->value == 42);
}

TEST_CASE("weak_ptr: reset", "[weak_ptr]") {
  TestObject::reset_counters();

  auto sptr = jal::make_shared<TestObject>(42);
  jal::weak_ptr<TestObject> wptr(sptr);

  REQUIRE_FALSE(wptr.expired());

  wptr.reset();
  REQUIRE(wptr.expired());
  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("weak_ptr: self assignment", "[weak_ptr]") {
  TestObject::reset_counters();

  auto sptr = jal::make_shared<TestObject>(42);
  jal::weak_ptr<TestObject> wptr(sptr);

  wptr = wptr;

  REQUIRE_FALSE(wptr.expired());
  auto locked = wptr.lock();
  REQUIRE(locked->value == 42);
}

TEST_CASE("weak_ptr: multiple weak references", "[weak_ptr]") {
  TestObject::reset_counters();

  jal::weak_ptr<TestObject> wptr1, wptr2, wptr3;

  {
    auto sptr = jal::make_shared<TestObject>(42);
    wptr1 = sptr;
    wptr2 = sptr;
    wptr3 = wptr1;

    REQUIRE_FALSE(wptr1.expired());
    REQUIRE_FALSE(wptr2.expired());
    REQUIRE_FALSE(wptr3.expired());
  }

  REQUIRE(wptr1.expired());
  REQUIRE(wptr2.expired());
  REQUIRE(wptr3.expired());
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("weak_ptr: lock extends lifetime", "[weak_ptr]") {
  TestObject::reset_counters();

  jal::weak_ptr<TestObject> wptr;
  jal::shared_ptr<TestObject> locked;

  {
    auto sptr = jal::make_shared<TestObject>(42);
    wptr = sptr;
    locked = wptr.lock();
  }

  REQUIRE(TestObject::instance_count == 1);
  REQUIRE(locked->value == 42);

  locked = jal::shared_ptr<TestObject>();
  REQUIRE(TestObject::instance_count == 0);
}

TEST_CASE("weak_ptr: thread safety with concurrent lock",
          "[weak_ptr][concurrency]") {
  TestObject::reset_counters();

  constexpr int num_threads = 10;
  constexpr int operations_per_thread = 1000;

  auto sptr = jal::make_shared<TestObject>(42);
  jal::weak_ptr<TestObject> wptr(sptr);
  std::vector<std::thread> threads;

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back([wptr]() {
      for (int j = 0; j < operations_per_thread; ++j) {
        auto locked = wptr.lock();
        if (locked.operator->() != nullptr) {
          REQUIRE(locked->value == 42);
        }
      }
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  REQUIRE(TestObject::instance_count == 1);
}

TEST_CASE("weak_ptr: reassignment from different shared_ptr", "[weak_ptr]") {
  TestObject::reset_counters();

  auto sptr1 = jal::make_shared<TestObject>(1);
  auto sptr2 = jal::make_shared<TestObject>(2);

  jal::weak_ptr<TestObject> wptr(sptr1);

  REQUIRE(wptr.lock()->value == 1);

  wptr = sptr2;

  REQUIRE(wptr.lock()->value == 2);
  REQUIRE(TestObject::instance_count == 2);
}
