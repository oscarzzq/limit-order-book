#include <iomanip>
#include <iostream>
#include <random>

#include "Order.h"
#include "Trade.h"
#include "OrderBook.h"


int main() {
    std::cout << "--- Limit Order Book Benchmark ---" << std::endl;

    OrderBook book;
    const int NUM_ORDERS = 1'000'000;
    std::vector<Order> testOrders;
    testOrders.reserve(NUM_ORDERS);

    std::mt19937 rng(42);
    std::uniform_int_distribution<uint64_t> priceDist(95, 105);
    std::uniform_int_distribution<uint64_t> quantityDist(1, 100);
    std::uniform_int_distribution<int> sideDist(0, 1); // 0 is ASK, 1 is BID

    std::cout << "Pre-generating " << NUM_ORDERS << " random orders." << std::endl;
    for (uint64_t i = 0; i < NUM_ORDERS; ++i) {
        Side side = (sideDist(rng) == 0) ? Side::ASK : Side::BID;
        uint64_t price = priceDist(rng);
        uint64_t quantity = quantityDist(rng);
        uint64_t instrumentId = 1;
        uint64_t traderId = i % 100;

        testOrders.emplace_back(i, quantity, price, instrumentId, traderId, side);
    }

    std::cout << "Starting test" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& order : testOrders) {
        book.addOrder(order);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    double ops_per_sec = (static_cast<double>(NUM_ORDERS) / duration_ms) * 1000.0;
    double avg_latency_ns = static_cast<double>(duration_ns) / NUM_ORDERS;

    std::cout << "\n--- Results ---" << std::endl;
    std::cout << "Total time: " << duration_ms << " ms"<< std::endl;
    std::cout << "Throughput: " << std::fixed << std::setprecision(0) << ops_per_sec << " orders/sec" << std::endl;
    std::cout << "Average latency: " << std::fixed << std::setprecision(2) << avg_latency_ns << " nanoseconds/order" << std::endl;

    return 0;
}