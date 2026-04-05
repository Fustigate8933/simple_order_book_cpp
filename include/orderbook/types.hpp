#pragma once

#include <cstdint>
#include <string>

namespace orderbook {

using OrderId = uint64_t;
using Price = int64_t;
using Quantity = uint64_t;
using Timestamp = uint64_t;

enum class Side {
	Buy,
	Sell
};

enum class OrderType {
	Limit,
	Market
};

enum class OrderStatus {
	Pending,
	PartialFill,
	Filled,
	Cancelled,
	Rejected
};

inline std::string to_string(Side side) {
	return side == Side::Buy ? "Buy" : "Sell";
}

inline std::string to_string(OrderType type) {
	return type == OrderType::Limit ? "Limit" : "Market";
}

inline std::string to_string(OrderStatus status) {
	switch (status) {
		case OrderStatus::Pending: return "Pending";
        case OrderStatus::PartialFill: return "PartialFill";
        case OrderStatus::Filled: return "Filled";
        case OrderStatus::Cancelled: return "Cancelled";
        case OrderStatus::Rejected: return "Rejected";
        default: return "Unknown";
	}
}

}
