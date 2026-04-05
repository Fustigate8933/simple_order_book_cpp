#include "orderbook/order.hpp"
#include <sstream>
#include <chrono>

namespace orderbook {

Order::Order(OrderId id, Side side, OrderType type, Price price, Quantity quantity)
	: id_{id}
	, side_{side}
	, type_{type}
	, price_{price}
	, quantity_{quantity}
	, remaining_quantity_{quantity}
	, status_{OrderStatus::Pending}
	, timestamp_{get_current_timestamp()} {
}

Order::Order(OrderId id, Side side, OrderType type, Quantity quantity)
	: id_(id)
    , side_(side)
    , type_(type)
    , price_(0)
    , quantity_(quantity)
    , remaining_quantity_(quantity)
    , status_(OrderStatus::Pending)
    , timestamp_(get_current_timestamp()) {
}

void Order::fill(Quantity fill_quantity) {
	if (fill_quantity > remaining_quantity_) {
		return;
	}

	remaining_quantity_ -= fill_quantity;

	if (remaining_quantity_ == 0) {
		status_ = OrderStatus::Filled;
	} else {
		status_ = OrderStatus::PartialFill;
	}
}

void Order::cancel() {
	if (is_active()) {
		status_ = OrderStatus::Cancelled;
	}
}

std::string Order::to_string() const {
    std::ostringstream oss;
    oss << "Order[id=" << id_ 
        << " " << orderbook::to_string(side_)
        << " " << orderbook::to_string(type_)
        << " price=" << price_
        << " qty=" << quantity_
        << " remaining=" << remaining_quantity_
        << " status=" << orderbook::to_string(status_)
        << "]";
    return oss.str();
}

Timestamp Order::get_current_timestamp() {
	return std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now().time_since_epoch()
	).count();
}

}
