# Limit Order Book (LOB) Matching Engine

An ultra-low latency, high-throughput Limit Order Book and matching engine written in modern C++. Designed for high-frequency trading (HFT) environments, this engine utilizes highly optimized standard library data structures to achieve purely $O(1)$ order cancellations and nanosecond-scale order matching.

## Performance Benchmarks
Benchmarked on a consumer-grade CPU (Apple Silicon) in Release mode (`-O3`), processing 1,000,000 randomized orders with a tight spread to force constant matching:

* **Throughput:** 5.18+ Million orders / second
* **Average Latency:** ~193 nanoseconds / order

## System Architecture

The matching engine enforces strict **Price-Time Priority** using a combination of three data structures to optimize for the most critical trading path operations:

* **Price Priority (The Trees):** Two `std::map` (Red-Black Trees) instances for the BID and ASK sides. This guarantees $O(\log N)$ lookups for the absolute best market price. The BID map utilizes `std::greater<>` to naturally reverse-sort buyers, keeping the highest bidder at `begin()`.
* **Time Priority (The Queues):** A `std::list` (Doubly Linked List) is maintained at every distinct price level. Orders submitted at the same price are appended to the queue, enforcing strict First-In-First-Out (FIFO) execution.
* **O(1) Cancellations (The Directory):** A `std::unordered_map` serves as the central directory, mapping globally unique `OrderID`s directly to their exact memory pointer (`std::list::iterator`). When a trader cancels a resting order, the engine bypasses tree traversal entirely and snips the order out of the linked list in $O(1)$ time.

## Core Features
* **Maker-Taker Pricing:** strict adherence to resting order price execution.
* **Memory-Aligned Structs:** `Order` and `Trade` structs are designed for cache-friendly memory alignment.
* **Partial Fills:** Flawless handling of resting orders that are larger, smaller, or exactly equal to the incoming aggressive order quantity.
* **Zero-Copy Execution:** The `completeExchange` system utilizes constant references (`const Order&`) to prevent unnecessary memory allocations during the critical matching loop.

## Building and Running
This project uses CMake. To build and run the benchmarking suite in Release mode:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
./limit_order_book