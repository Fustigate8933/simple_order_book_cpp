#pragma once

#include "orderbook/order.hpp"
#include <deque>
#include <memory>
#include <unordered_map>

namespace orderbook {

class PriceLevel {
private:
	Price price_;
	std::deque<Order> orders_;
	std::unordered_map<OrderId, std::deque<Order>::iterator> order_map_;

public:
	explicit PriceLevel(Price price);

	void add_order(const Order& order);
	bool remove_order(OrderId order_id);

	Price price() const { return price_; }
	size_t size() const { return orders_.size(); }
	bool empty() const { return orders_.empty(); }
	Quantity total_quantity() const;

	std::deque<Order>& orders() { return orders_; }
	const std::deque<Order>& orders() const { return orders_; }
};

}
