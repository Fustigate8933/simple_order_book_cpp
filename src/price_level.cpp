#include "orderbook/price_level.hpp"
#include <algorithm>

namespace orderbook {

PriceLevel::PriceLevel(Price price)
    : price_(price) {
}

void PriceLevel::add_order(const Order& order) {
    orders_.push_back(order);
    auto it = orders_.end();
    --it;
    order_map_[order.id()] = it;
}

bool PriceLevel::remove_order(OrderId order_id) {
    auto map_it = order_map_.find(order_id);
    if (map_it == order_map_.end()) {
        return false;
    }
    
    auto deque_it = map_it->second;
    orders_.erase(deque_it);
    order_map_.erase(map_it);
    
    return true;
}

Quantity PriceLevel::total_quantity() const {
    Quantity total = 0;
    for (const auto& order : orders_) {
        total += order.remaining_quantity();
    }
    return total;
}

}
