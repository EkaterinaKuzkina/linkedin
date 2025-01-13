#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

struct Order {
	int id;	 // unique order id
	string ticker;
	double price;
	int quantity;
	bool isBuy;	 // indicates order side
};

class OrderBook {
   public:
	map<double, vector<Order>, greater<double>> buyOrders;
	map<double, vector<Order>, less<double>> sellOrders;

	void addOrder(Order order);
	void removeOrder(int orderId);
	void print();

   private:
	template <class T>
	void printBookForSide(const map<double, vector<Order>, T> &book);
	template <class T>
	void removeOrderForSide(map<double, vector<Order>, T> &book, Order &order);

	map<int, Order> orders;
};

void OrderBook::addOrder(Order order) {
	if (order.isBuy) {
		buyOrders[order.price].push_back(order);
	} else {
		sellOrders[order.price].push_back(order);
	}
	orders[order.id] = order;
}

void OrderBook::removeOrder(int orderId) {
	// Check that order exists
	auto it = orders.find(orderId);
	if (it == orders.end()) {
		cout << "Order with orderId=" << orderId << " doesn't exist" << endl;
		return;
	}
	// Delete order from Id map
	orders.erase(it);

	// Delete order from the order book
	if (it->second.isBuy) {
		cout << "Removing Buy Order" << endl;
		removeOrderForSide(buyOrders, it->second);
	} else {
		cout << "Removing Sell Order" << endl;
		removeOrderForSide(sellOrders, it->second);
	}
}

template <class T>
void OrderBook::removeOrderForSide(map<double, vector<Order>, T> &book,
								   Order &order) {
	auto sideIt = book.find(order.price);
	if (sideIt != book.end()) {
		auto &ordersForPrice = sideIt->second;

		// Find and remove order from order book
		ordersForPrice.erase(
			remove_if(ordersForPrice.begin(), ordersForPrice.end(),
					  [order](const Order &o) { return o.id == order.id; }));
		// If no orders for the price delete price  entry from the order book
		if (ordersForPrice.empty()) {
			book.erase(sideIt);
			cout << "Price entry " << sideIt->first
				 << " was removed from order book as it became empty" << endl;
		}
		cout << "Order removed orderId=" << order.id << endl;
	}
}

void OrderBook::print() {
	cout << "Buy Order Book" << endl;
	printBookForSide(buyOrders);

	cout << "Sell Order Book" << endl;
	printBookForSide(sellOrders);
}

template <class T>
void OrderBook::printBookForSide(const map<double, vector<Order>, T> &book) {
	for (const auto &[price, orders] : book) {
		for (const auto &order : orders) {
			cout << "Id: " << order.id << " Ticker: " << order.ticker
				 << " Price: " << order.price << " Quantity: " << order.quantity
				 << endl;
		}
	}
}

class OrderMatchingEngine {
   public:
	void matchOrders(OrderBook &orderBook);
};

void OrderMatchingEngine::matchOrders(OrderBook &orderBook) {
	// get max buy, get min sell. match, reduce the quantity and delete from
	// order book if qunantity is 0
	while (!orderBook.buyOrders.empty() && !orderBook.sellOrders.empty()) {
		auto highestBuy = orderBook.buyOrders.begin();
		auto lowestSell = orderBook.sellOrders.begin();
		// compare sell and buy prices
		if (highestBuy->first >= lowestSell->first) {
			auto &buyOrder = highestBuy->second.front();
			auto &sellOrder = lowestSell->second.front();
			int matchedQuantity = min(buyOrder.quantity, sellOrder.quantity);
			cout << "Trade executed with " << matchedQuantity
				 << " units at price " << sellOrder.price << endl;
			buyOrder.quantity -= matchedQuantity;
			sellOrder.quantity -= matchedQuantity;
			if (buyOrder.quantity == 0) {
				orderBook.removeOrder(buyOrder.id);
			}
			if (sellOrder.quantity == 0) {
				orderBook.removeOrder(sellOrder.id);
			}
		} else {
			break;
		}
	}
}

int main() {
	OrderBook orderBook;

	// Add some orders
	orderBook.addOrder(Order{1, "AAPL", 100.0, 10, true});
	orderBook.addOrder(Order{2, "AAPL", 99.5, 5, false});
	orderBook.addOrder(Order{3, "AAPL", 101.0, 20, true});
	orderBook.addOrder(Order{4, "AAPL", 99.0, 5, false});
	orderBook.addOrder(Order{5, "AAPL", 119.0, 10, false});

	// Print current orders
	cout << "Current Order Book" << endl;
	orderBook.print();

	cout << endl;

	// Print current orders
	orderBook.removeOrder(5);
	cout << "\nCurrent Order Book" << endl;
	orderBook.print();

	cout << endl;
	cout << "Orders matching" << endl;
	OrderMatchingEngine orderMatchingEngine;
	orderMatchingEngine.matchOrders(orderBook);
	orderBook.print();

	return 0;
}
