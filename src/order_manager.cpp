#include "orderbook/order_manager.hpp"

namespace orderbook {

OrderManager::OrderManager(std::shared_ptr<MatchingEngine> engine)
    : engine_(engine) {
}

OrderId OrderManager::create_limit_order(Side side, Price price, Quantity quantity) {
    OrderId id = generate_order_id();
    auto order = std::make_shared<Order>(id, side, OrderType::Limit, price, quantity);
    
    store_order(order);
    engine_->submit_order(*order);
    
    return id;
}

OrderId OrderManager::create_market_order(Side side, Quantity quantity) {
    OrderId id = generate_order_id();
    auto order = std::make_shared<Order>(id, side, OrderType::Market, quantity);
    
    store_order(order);
    engine_->submit_order(*order);
    
    return id;
}

bool OrderManager::cancel_order(OrderId order_id) {
    std::lock_guard lock(mutex_);
    
    auto it = orders_.find(order_id);
    if (it != orders_.end()) {
        it->second->cancel();
        return engine_->cancel_order(order_id);
    }
    
    return false;
}

std::shared_ptr<Order> OrderManager::get_order(OrderId order_id) const {
    std::lock_guard lock(mutex_);
    
    auto it = orders_.find(order_id);
    if (it != orders_.end()) {
        return it->second;
    }
    
    return nullptr;
}

void OrderManager::set_trade_callback(TradeCallback callback) {
    engine_->set_trade_callback(callback);
}

OrderId OrderManager::generate_order_id() {
    return next_order_id_.fetch_add(1, std::memory_order_relaxed);
}

void OrderManager::store_order(std::shared_ptr<Order> order) {
    std::lock_guard lock(mutex_);
    orders_[order->id()] = order;
}

}
