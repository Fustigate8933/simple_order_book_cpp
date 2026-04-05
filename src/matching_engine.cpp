#include "orderbook/matching_engine.hpp"

namespace orderbook {

MatchingEngine::MatchingEngine(std::shared_ptr<OrderBook> order_book) : order_book_{order_book} {
}

void MatchingEngine::set_trade_callback(TradeCallback&& callback) {
	trade_callback_ = callback;
}

std::vector<Trade> MatchingEngine::submit_order(Order& order) {
	auto trades = order_book_->match_order(order);
	notify_trades(trades);
	return trades;
}

bool MatchingEngine::cancel_order(OrderId order_id) {
	return order_book_->cancel_order(order_id);
}

void MatchingEngine::notify_trades(const std::vector<Trade>& trades) {
	if (trade_callback_) {
		for (const auto& trade : trades) {
			trade_callback_(trade);
		}
	}
}

}
