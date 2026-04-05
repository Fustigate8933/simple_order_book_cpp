#include "orderbook/order_book.hpp"
#include "orderbook/trade.hpp"
#include <mutex>
#include <sstream>

namespace orderbook {

void OrderBook::add_order(const Order& order) {
	std::unique_lock lock{mutex_};
	add_to_book(order);
}

bool OrderBook::cancel_order(OrderId order_id) {
	std::unique_lock locl{mutex_};

	auto it = order_to_price_.find(order_id);
	if (it == order_to_price_.end()) {
		return false; // order not found
	}

	Price price = it->second;
	order_to_price_.erase(it);

	auto bid_it = bids_.find(price);
	if (bid_it != bids_.end()) {
		if (bid_it->second.remove_order(order_id)) {
			if (bid_it->second.empty()) {
				bids_.erase(bid_it);
			}
			return true;
		}
	}

	auto ask_it = asks_.find(price);
	if (ask_it != asks_.end()) {
		if (ask_it->second.remove_order(order_id)) {
			if (ask_it->second.empty()) {
				asks_.erase(ask_it);
			}
			return true;
		}
	}

	return false;
}

std::vector<Trade> OrderBook::match_order(Order& order) {
	std::unique_lock lock{mutex_};
	
	if (order.type() == OrderType::Market) {
		return match_market_order(order);
	} else {
		return match_limit_order(order);
	}
}

std::vector<Trade> OrderBook::match_limit_order(Order& order) {
	std::vector<Trade> trades;
	
	if (order.side() == Side::Buy) {
		while (!asks_.empty() && order.remaining_quantity() > 0) {
			auto& best_level = asks_.begin()->second;

			if (best_level.price() > order.price()) break;

			auto& resting_orders = best_level.orders();
			while (!resting_orders.empty() && order.remaining_quantity() > 0) {
				auto& resting_order = resting_orders.front();
				Quantity trade_qty = std::min(order.remaining_quantity(), resting_order.remaining_quantity());

				trades.push_back(execute_trade(order, resting_order, trade_qty));

				if (resting_order.is_filled()) {
					order_to_price_.erase(resting_order.id());
					resting_orders.pop_front();
				}
			}

			if (best_level.empty()) {
				asks_.erase(asks_.begin());
			}
		}
	} else {
		while (!bids_.empty() && order.remaining_quantity() > 0) {
			auto& best_level = bids_.begin()->second;

			if (best_level.price() < order.price()) break;

			auto& resting_orders = best_level.orders();
			while (!resting_orders.empty() && order.remaining_quantity() > 0) {
				auto& resting_order = resting_orders.front();
				Quantity trade_qty = std::min(resting_order.remaining_quantity(), order.remaining_quantity());

				trades.push_back(execute_trade(order, resting_order, trade_qty));

				if (resting_order.is_filled()) {
					order_to_price_.erase(resting_order.id());
					resting_orders.pop_front();
				}
			}

			if (best_level.empty()) {
				bids_.erase(bids_.begin());
			}
		}
	}

	if (order.remaining_quantity() > 0 && order.is_active()) {
		add_to_book(order);
	}

	return trades;
}

std::vector<Trade> OrderBook::match_market_order(Order& order) {
	std::vector<Trade> trades;
	
	if (order.side() == Side::Buy) {
		while (!asks_.empty() && order.remaining_quantity() > 0) {
			auto& best_level = asks_.begin()->second;

			auto& resting_orders = best_level.orders();
			while (!resting_orders.empty() && order.remaining_quantity() > 0) {
				auto& resting_order = resting_orders.front();
				Quantity trade_qty = std::min(order.remaining_quantity(), resting_order.remaining_quantity());

				trades.push_back(execute_trade(order, resting_order, trade_qty));

				if (resting_order.is_filled()) {
					order_to_price_.erase(resting_order.id());
					resting_orders.pop_front();
				}
			}

			if (best_level.empty()) {
				asks_.erase(asks_.begin());
			}
		}
	} else {
		while (!bids_.empty() && order.remaining_quantity() > 0) {
			auto& best_level = bids_.begin()->second;

			auto& resting_orders = best_level.orders();
			while (!resting_orders.empty() && order.remaining_quantity() > 0) {
				auto& resting_order = resting_orders.front();
				Quantity trade_qty = std::min(resting_order.remaining_quantity(), order.remaining_quantity());

				trades.push_back(execute_trade(order, resting_order, trade_qty));

				if (resting_order.is_filled()) {
					order_to_price_.erase(resting_order.id());
					resting_orders.pop_front();
				}
			}

			if (best_level.empty()) {
				bids_.erase(bids_.begin());
			}
		}
	}

	if (order.remaining_quantity() > 0 && order.is_active()) {
		order.cancel();
	}

	return trades;
}

void OrderBook::add_to_book(const Order& order) {
	if (order.side() == Side::Buy) {
		auto it = bids_.find(order.price());
		if (it == bids_.end()) {
			it = bids_.emplace(order.price(), PriceLevel{order.price()}).first;
		}
		it->second.add_order(order);
		order_to_price_[order.id()] = order.price();
	} else {
		auto it = asks_.find(order.price());
		if (it == asks_.end()) {
			it = asks_.emplace(order.price(), PriceLevel{order.price()}).first;
		}
		it->second.add_order(order);
		order_to_price_[order.id()] = order.price();
	}
}

Trade OrderBook::execute_trade(Order& incoming_order, Order& resting_order, Quantity quantity) {
	Price trade_price = resting_order.price();

	incoming_order.fill(quantity);
	resting_order.fill(quantity);

	OrderId buyer_id = (incoming_order.side() == Side::Buy) ? incoming_order.id() : resting_order.id();
	OrderId seller_id = (incoming_order.side() == Side::Sell) ? incoming_order.id() : resting_order.id();

	return Trade{buyer_id, seller_id, trade_price, quantity, incoming_order.timestamp()};
}

size_t OrderBook::bid_depth() const {
	std::shared_lock lock{mutex_}; // allows simultaneous reads but not writes
	return bids_.size();
}

size_t OrderBook::ask_depth() const {
	std::shared_lock lock{mutex_};
	return asks_.size();
}

Price OrderBook::best_bid() const { // quote
	std::shared_lock lock{mutex_};
	return bids_.empty() ? 0 : bids_.begin()->first;
}

Price OrderBook::best_ask() const { // quote
	std::shared_lock lock{mutex_};
	return asks_.empty() ? 0 : asks_.begin()->first;
}

std::string OrderBook::to_string() const {
	std::shared_lock lock{mutex_};
	std::ostringstream oss;
	oss << "OrderBook[bids=" << bids_.size() << " asks=" << asks_.size() << "]";
	return oss.str();
}

}
