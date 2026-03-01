//
// Created by Oscar Zhang on 2026/2/23.
//

#pragma once
#include <map>
#include <list>
#include "Order.h"
#include "Trade.h"

using std::uint64_t;

class OrderBook {
    std::map<uint64_t, std::list<Order>> asks;
    std::map<uint64_t, std::list<Order>, std::greater<>> bids;
    std::unordered_map<uint64_t, std::list<Order>::iterator> orders;
    std::vector<Trade> trades;
    uint64_t nextTradeId = 0;

public:
    void addOrder(Order order) {
        if (order.side == Side::ASK) {
            while (!bids.empty() && order.quantity > 0 && bids.begin()->first >= order.price) {
                if (bids.begin()->second.front().quantity < order.quantity) {
                    order.quantity -= bids.begin()->second.front().quantity;
                    //completeExchange(order.id, bids.begin()->second.front().id);
                    completeExchange(bids.begin()->second.front(), order, bids.begin()->second.front().quantity);
                    deleteOrder(bids.begin()->second.front().id);
                } else if (bids.begin()->second.front().quantity == order.quantity) {
                    completeExchange(bids.begin()->second.front(), order, order.quantity);
                    order.quantity = 0;
                    deleteOrder(bids.begin()->second.front().id);
                } else {
                    bids.begin()->second.front().quantity -= order.quantity;
                    completeExchange(bids.begin()->second.front(), order, order.quantity);
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
                    //completeExchange(asks.begin()->second.front().id, order.id);
                    completeExchange(asks.begin()->second.front(), order, asks.begin()->second.front().quantity);
                    deleteOrder(asks.begin()->second.front().id);
                } else if (asks.begin()->second.front().quantity == order.quantity) {
                    completeExchange(asks.begin()->second.front(), order, order.quantity);
                    order.quantity = 0;
                    deleteOrder(asks.begin()->second.front().id);
                } else {
                    asks.begin()->second.front().quantity -= order.quantity;
                    completeExchange(asks.begin()->second.front(), order, order.quantity);
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

    void completeExchange(const Order& restingOrder, const Order& incomingOrder, uint64_t quantity) {
        uint64_t sellerId = restingOrder.side == Side::ASK ? restingOrder.id : incomingOrder.id;
        uint64_t buyerId = restingOrder.side == Side::BID ? restingOrder.id : incomingOrder.id;
        trades.push_back(Trade(nextTradeId++, quantity, restingOrder.price, restingOrder.instrumentId, buyerId, sellerId));
    }

    void deleteOrder(uint64_t orderId) {
        auto mapIter = orders.find(orderId);
        if (mapIter == orders.end()) {
            return;
        }
        auto listIter = mapIter->second;
        uint64_t price = listIter->price;
        Side side = listIter->side;
        if (side == Side::ASK) {
            auto& orderList = asks[price];
            orderList.erase(listIter);
            if (orderList.empty()) {
                asks.erase(price);
            }
        } else {
            auto& orderList = bids[price];
            orderList.erase(listIter);
            if (orderList.empty()) {
                bids.erase(price);
            }
        }
        orders.erase(mapIter);
    }
};
