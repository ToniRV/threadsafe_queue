# Thread Safe Queue
Thread-safe queue with shutdown switch.

## Installation
First, clone this repo:
```
git clone 
```

Then, build the example.
```
cd threadsafe_queue
mkdir build
cd build
cmake ..
make -j8
```

## Run
Provided is an example of a Producer/Consumer using the queue to produce a "Hello World" string and consume it by printing it to the console output.

Go to the build folder, and execute `threadsafeQueue`
```
./threadsafeQueue
```

Expect to see an output similar to this:
```
Got msg: Hello World!
Got msg: Hello World!
Got msg: Hello World!
Got msg: Hello World!
Shutdown queue.
Resume queue.
Got msg: Hello World!
Got msg: Hello World!
Got msg: Hello World!
Got msg: Hello World!
Got msg: Hello World!
Joining threads.
Threads joined.
```



