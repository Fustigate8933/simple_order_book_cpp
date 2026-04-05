#include "orderbook/trade.hpp"
#include <sstream>

namespace orderbook {

Trade::Trade(OrderId buyer_id, OrderId seller_id, Price price, Quantity quantity, Timestamp timestamp)
    : buyer_id_(buyer_id)
    , seller_id_(seller_id)
    , price_(price)
    , quantity_(quantity)
    , timestamp_(timestamp) {
}

std::string Trade::to_string() const {
    std::ostringstream oss;
    oss << "Trade[buyer=" << buyer_id_
        << " seller=" << seller_id_
        << " price=" << price_
        << " qty=" << quantity_
        << " ts=" << timestamp_
        << "]";
    return oss.str();
}

}

