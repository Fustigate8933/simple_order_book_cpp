#pragma once

#include "orderbook/order.hpp"
#include "orderbook/price_level.hpp"
#include "orderbook/trade.hpp"
#include <map>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace orderbook {

class OrderBook {
private:
	std::map<Price, PriceLevel, std::greater<Price>> bids_;
	std::map<Price, PriceLevel, std::less<Price>> asks_;
	std::unordered_map<OrderId, Price> order_to_price_;

	mutable std::shared_mutex mutex_;

	std::vector<Trade> match_limit_order(Order& order);
	std::vector<Trade> match_market_order(Order& order);

	void add_to_book(const Order& order);
	Trade execute_trade(Order& incoming_order, Order& resting_order, Quantity quantity);

public:
	OrderBook() = default;

	void add_order(const Order& order);
	bool cancel_order(OrderId order_id);

	std::vector<Trade> match_order(Order& order);

	size_t bid_depth() const;
	size_t ask_depth() const;

	Price best_bid() const;
	Price best_ask() const;

	std::string to_string() const;
};

}
