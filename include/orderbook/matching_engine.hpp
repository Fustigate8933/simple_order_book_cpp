#pragma once

#include "orderbook/order.hpp"
#include "orderbook/order_book.hpp"
#include "orderbook/trade.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace orderbook {

using TradeCallback = std::function<void(const Trade&)>;

class MatchingEngine {
public:
    explicit MatchingEngine(std::shared_ptr<OrderBook> order_book);

    void set_trade_callback(TradeCallback&& callback);
    
    std::vector<Trade> submit_order(Order& order);
    bool cancel_order(OrderId order_id);

    std::shared_ptr<OrderBook> order_book() const { return order_book_; }

private:
    std::shared_ptr<OrderBook> order_book_;
    TradeCallback trade_callback_;

    void notify_trades(const std::vector<Trade>& trades);
};

}

