/********************************************************************************
 * MIT License
 *
 * Copyright (c) 2019 Toni Rosinol
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*********************************************************************************/

#include <chrono>
#include <string>
#include <iostream>
#include <thread>

#include "ThreadsafeQueue.h"

// To specify time in human-readable format.
using namespace std::chrono_literals;

void consumer(ThreadsafeQueue<std::string>& q, const std::atomic_bool& kill_switch) {
  while (!kill_switch) {
    std::string msg = "No msg!";
    if (q.popBlocking(msg)) {
      std::cout << "Got msg: " << msg << '\n';
    }
  }
  q.shutdown();
}

void producer(ThreadsafeQueue<std::string>& q, const std::atomic_bool& kill_switch) {
  while (!kill_switch) {
    std::this_thread::sleep_for(200ms);
    q.push("Hello World!");
  }
  q.shutdown();
}

int main(int argc, char *argv[]) {
  ThreadsafeQueue<std::string> q;
  std::atomic_bool kill_switch (false);
  std::thread c (consumer, std::ref(q), std::ref(kill_switch));
  std::thread p (producer, std::ref(q), std::ref(kill_switch));

  std::this_thread::sleep_for(1s);

  std::cout << "Shutdown queue.\n";
  q.shutdown();

  std::this_thread::sleep_for(1s);

  std::cout << "Resume queue.\n";
  q.resume();

  std::this_thread::sleep_for(1s);

  std::cout << "Joining threads.\n";
  kill_switch = true;
  c.join();
  p.join();
  std::cout << "Threads joined.\n";

  return EXIT_SUCCESS;
}
