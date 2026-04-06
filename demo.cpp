#include "orderbook/order.hpp"
#include "orderbook/order_book.hpp"
#include "orderbook/matching_engine.hpp"
#include <iostream>
#include <memory>
#include <iomanip>

using namespace orderbook;

void print_separator() {
    std::cout << std::string(60, '=') << "\n";
}

void print_trades(const std::vector<Trade>& trades) {
    if (trades.empty()) {
        std::cout << "  No trades executed\n";
        return;
    }
    
    std::cout << "  Trades executed:\n";
    for (const auto& trade : trades) {
        std::cout << "    " << trade.to_string() << "\n";
    }
}

void print_book_state(const std::shared_ptr<OrderBook>& book) {
    std::cout << "  Book state: "
              << "Best Bid=" << book->best_bid()
              << " Best Ask=" << book->best_ask()
              << " (Bid levels=" << book->bid_depth()
              << ", Ask levels=" << book->ask_depth() << ")\n";
}

int main() {
    std::cout << "\n🚀 Order Book Demo\n";
    print_separator();
    
    auto order_book = std::make_shared<OrderBook>();
    MatchingEngine engine(order_book);
    
    // Set up a trade callback to print trades as they happen
    engine.set_trade_callback([](const Trade& trade) {
        std::cout << "  💰 TRADE: " << trade.to_string() << "\n";
    });
    
    // Test 1: Add resting orders to build the book
    std::cout << "\n📖 Test 1: Building the order book\n";
    print_separator();
    
    Order sell1(1, Side::Sell, OrderType::Limit, 10100, 100);
    Order sell2(2, Side::Sell, OrderType::Limit, 10200, 200);
    Order sell3(3, Side::Sell, OrderType::Limit, 10300, 150);
    
    Order buy1(4, Side::Buy, OrderType::Limit, 9900, 100);
    Order buy2(5, Side::Buy, OrderType::Limit, 9800, 200);
    Order buy3(6, Side::Buy, OrderType::Limit, 9700, 150);
    
    std::cout << "Adding sell orders at 10100, 10200, 10300...\n";
    engine.submit_order(sell1);
    engine.submit_order(sell2);
    engine.submit_order(sell3);
    
    std::cout << "Adding buy orders at 9900, 9800, 9700...\n";
    engine.submit_order(buy1);
    engine.submit_order(buy2);
    engine.submit_order(buy3);
    
    print_book_state(order_book);
    
    // Test 2: Limit order that matches
    std::cout << "\n📊 Test 2: Aggressive limit buy order (crosses spread)\n";
    print_separator();
    
    Order aggressive_buy(7, Side::Buy, OrderType::Limit, 10150, 150);
    std::cout << "Submitting: " << aggressive_buy.to_string() << "\n";
    auto trades = engine.submit_order(aggressive_buy);
    print_trades(trades);
    print_book_state(order_book);
    
    // Test 3: Market order
    std::cout << "\n💨 Test 3: Market sell order\n";
    print_separator();
    
    Order market_sell(8, Side::Sell, OrderType::Market, 250);
    std::cout << "Submitting: " << market_sell.to_string() << "\n";
    trades = engine.submit_order(market_sell);
    print_trades(trades);
    print_book_state(order_book);
    
    // Test 4: Partial fill
    std::cout << "\n📉 Test 4: Large limit order (partial fill)\n";
    print_separator();
    
    Order large_buy(9, Side::Buy, OrderType::Limit, 10300, 500);
    std::cout << "Submitting: " << large_buy.to_string() << "\n";
    trades = engine.submit_order(large_buy);
    print_trades(trades);
    std::cout << "  Order after matching: " << large_buy.to_string() << "\n";
    print_book_state(order_book);
    
    // Test 5: Cancel order
    std::cout << "\n❌ Test 5: Cancel order\n";
    print_separator();
    
    Order to_cancel(10, Side::Buy, OrderType::Limit, 9500, 100);
    std::cout << "Adding order: " << to_cancel.to_string() << "\n";
    engine.submit_order(to_cancel);
    print_book_state(order_book);
    
    std::cout << "Cancelling order ID 10...\n";
    bool cancelled = engine.cancel_order(10);
    std::cout << "  Cancel " << (cancelled ? "SUCCESS" : "FAILED") << "\n";
    print_book_state(order_book);
    
    // Test 6: Market order with no liquidity
    std::cout << "\n⚠️  Test 6: Market order with insufficient liquidity\n";
    print_separator();
    
    Order huge_market_buy(11, Side::Buy, OrderType::Market, 10000);
    std::cout << "Submitting huge market buy for 10000 shares...\n";
    trades = engine.submit_order(huge_market_buy);
    print_trades(trades);
    std::cout << "  Order final state: " << huge_market_buy.to_string() << "\n";
    std::cout << "  (Market orders cancel if not fully filled)\n";
    
    print_separator();
    std::cout << "\n✅ Demo complete!\n\n";
    
    return 0;
}
