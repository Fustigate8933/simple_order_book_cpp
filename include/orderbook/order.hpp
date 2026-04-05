#pragma once

#include "orderbook/types.hpp"
#include <string>

namespace orderbook {

class Order {
private:
	OrderId id_;
	Side side_;
	OrderType type_;
	Price price_;
	Quantity quantity_;
	Quantity remaining_quantity_;
	OrderStatus status_;
	Timestamp timestamp_;

	static Timestamp get_current_timestamp();

public:
	Order(OrderId id, Side side, OrderType type, Price price, Quantity quantity); // limit order
	Order(OrderId id, Side side, OrderType type, Quantity quantity); // market order
	
	OrderId id() const { return id_; }
	Side side() const { return side_; }
	OrderType type() const { return type_; }
	Price price() const { return price_; };
	Quantity quantity() const { return quantity_; }
	Quantity remaining_quantity() const { return remaining_quantity_; }
	OrderStatus status() const { return status_; }
	Timestamp timestamp() const { return timestamp_; }

	void fill(Quantity fill_quantity);
	void cancel();

	bool is_filled() const { return remaining_quantity_ == 0; }
	bool is_active() const { return status_ == OrderStatus::Pending || status_ == OrderStatus::PartialFill; }

	std::string to_string() const;
};

}
