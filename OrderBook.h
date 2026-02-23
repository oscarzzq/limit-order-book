//
// Created by Oscar Zhang on 2026/2/23.
//

#pragma once
#include <map>
#include <list>
#include "Order.h"

class OrderBook {
    std::map<uint64_t, std::list<Order>> asks;
    std::map<uint64_t, std::list<Order>, std::greater<>> bids;
    std::unordered_map<uint64_t, std::list<Order>::iterator> orders;

public:
    void addOrder(Order order) {
        if (order.side == Side::ASK) {
            while (!bids.empty() && order.quantity > 0 && bids.begin()->first >= order.price) {
                if (bids.begin()->second.front().quantity < order.quantity) {
                    order.quantity -= bids.begin()->second.front().quantity;
                    completeExchange(order.id, bids.begin()->second.front().id);
                    deleteOrder(bids.begin()->second.front().id);
                    if (bids.begin()->second.empty()) {
                        bids.erase(bids.begin());
                    }
                } else if (bids.begin()->second.front().quantity == order.quantity) {
                    completeExchange(order.id, bids.begin()->second.front().id);
                    order.quantity = 0;
                    deleteOrder(bids.begin()->second.front().id);
                    if (bids.begin()->second.empty()) {
                        bids.erase(bids.begin());
                    }
                } else {
                    bids.begin()->second.front().quantity -= order.quantity;
                    completeExchange(order.id, bids.begin()->second.front().id);
                    order.quantity = 0;
                }
            }
            if (order.quantity > 0) {
                auto& orderList = asks[order.price];
                auto it = orderList.insert(orderList.end(), order);
                orders[order.id] = it;
            }
        } else {
            while (!asks.empty() && order.quantity > 0 && asks.begin()->first <= order.price) {
                if (asks.begin()->second.front().quantity < order.quantity) {
                    order.quantity -= asks.begin()->second.front().quantity;
                    completeExchange(asks.begin()->second.front().id, order.id);
                    deleteOrder(asks.begin()->second.front().id);
                    if (asks.begin()->second.empty()) {
                        asks.erase(asks.begin());
                    }
                } else if (asks.begin()->second.front().quantity == order.quantity) {
                    completeExchange(asks.begin()->second.front().id, order.id);
                    order.quantity = 0;
                    deleteOrder(asks.begin()->second.front().id);
                    if (asks.begin()->second.empty()) {
                        asks.erase(asks.begin());
                    }
                } else {
                    asks.begin()->second.front().quantity -= order.quantity;
                    completeExchange(asks.begin()->second.front().id, order.id);
                    order.quantity = 0;
                }
            }
            if (order.quantity > 0) {
                auto& orderList = bids[order.price];
                auto it = orderList.insert(orderList.end(), order);
                orders[order.id] = it;
            }
        }
    }

    void completeExchange(uint64_t orderAsk, uint64_t orderBid) {

    }

    void deleteOrder(uint64_t order) {

    }
};
