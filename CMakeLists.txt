cmake_minimum_required(VERSION 4.0)
project(limit_order_book)

set(CMAKE_CXX_STANDARD 20)

add_executable(limit_order_book main.cpp
        Order.h
        OrderBook.h
)
