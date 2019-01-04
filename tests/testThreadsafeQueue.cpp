#include "gtest/gtest.h"

#include <chrono>
#include <string>
#include <iostream>
#include <thread>

#include "ThreadsafeQueue.h"

void consumer(ThreadsafeQueue<std::string>& q,
              const std::atomic_bool& kill_switch) {
  while (!kill_switch) {
    std::string msg = "No msg!";
    if (q.popBlocking(msg)) {
      std::cout << "Got msg: " << msg << '\n';
    }
  }
  q.shutdown();
}

void producer(ThreadsafeQueue<std::string>& q,
              const std::atomic_bool& kill_switch) {
  while (!kill_switch) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    q.push("Hello World!");
  }
  q.shutdown();
}

/* ************************************************************************* */
TEST(ThreadsafeQueue, popBlocking_by_reference) {
  ThreadsafeQueue<std::string> q;
  std::thread p ([&] {
    q.push("Hello World!");
    q.push("Hello World 2!");
  });
  std::string s;
  q.popBlocking(s);
  EXPECT_EQ(s, "Hello World!");
  q.popBlocking(s);
  EXPECT_EQ(s, "Hello World 2!");
  q.shutdown();
  EXPECT_EQ(q.popBlocking(s), false);
  EXPECT_EQ(s, "Hello World 2!");

  // Leave some time for p to finish its work.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(p.joinable());
  p.join();
}

TEST(ThreadsafeQueue, popBlocking_by_shared_ptr) {
  ThreadsafeQueue<std::string> q;
  std::thread p ([&] {
    q.push("Hello World!");
    q.push("Hello World 2!");
  });
  std::shared_ptr<std::string> s = q.popBlocking();
  EXPECT_EQ(*s, "Hello World!");
  auto s2 = q.popBlocking();
  EXPECT_EQ(*s2, "Hello World 2!");
  q.shutdown();
  EXPECT_EQ(q.popBlocking(), nullptr);

  // Leave some time for p to finish its work.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(p.joinable());
  p.join();
}

/* ************************************************************************* */
TEST(ThreadsafeQueue, producer_consumer) {
  ThreadsafeQueue<std::string> q;
  std::atomic_bool kill_switch (false);
  std::thread c (consumer, std::ref(q), std::ref(kill_switch));
  std::thread p (producer, std::ref(q), std::ref(kill_switch));

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::cout << "Shutdown queue.\n";
  q.shutdown();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::cout << "Resume queue.\n";
  q.resume();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::cout << "Joining threads.\n";
  kill_switch = true;
  c.join();
  p.join();
  std::cout << "Threads joined.\n";
}
