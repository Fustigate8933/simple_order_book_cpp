#pragma once

#include "orderbook/types.hpp"
#include <string>

namespace orderbook {

class Trade {
private:
	OrderId buyer_id_;
	OrderId seller_id_;
	Price price_;
	Quantity quantity_;
	Timestamp timestamp_;

public:
	Trade(OrderId buyer_id, OrderId seller_id, Price price, Quantity quantity, Timestamp timestamp);

	OrderId buyer_id() const { return buyer_id_; }
	OrderId seller_id() const { return seller_id_; }
	Price price() const { return price_; }
	Quantity quantity() const { return quantity_; }
	Timestamp timestamp() const { return timestamp_; }

	std::string to_string() const;
};

}
