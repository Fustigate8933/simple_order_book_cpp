#pragma once

#include "orderbook/matching_engine.hpp"
#include "orderbook/order.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace orderbook {

class OrderManager {
public:
    explicit OrderManager(std::shared_ptr<MatchingEngine> engine);

    OrderId create_limit_order(Side side, Price price, Quantity quantity);
    OrderId create_market_order(Side side, Quantity quantity);
    
    bool cancel_order(OrderId order_id);
    
    std::shared_ptr<Order> get_order(OrderId order_id) const;

    void set_trade_callback(TradeCallback callback);

private:
    std::shared_ptr<MatchingEngine> engine_;
    std::atomic<OrderId> next_order_id_{1};
    
    mutable std::mutex mutex_;
    std::unordered_map<OrderId, std::shared_ptr<Order>> orders_;

    OrderId generate_order_id();
    void store_order(std::shared_ptr<Order> order);
};

}
