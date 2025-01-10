#include <algorithm>
#include <iostream>
#include <map>
#include <string>
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

int main() {
	OrderBook orderBook;

	// Add some orders
	orderBook.addOrder(Order{1, "AAPL", 150.0, 100, true});
	orderBook.addOrder(Order{2, "AAPL", 150.0, 50, false});
	orderBook.addOrder(Order{3, "GOOGL", 2800.0, 10, true});

	// Print current orders
	cout << "Current Orders:" << endl;
	orderBook.print();

	cout << endl;

	// Print current orders
	orderBook.removeOrder(3);
	cout << "Current Orders:" << endl;
	orderBook.print();

	return 0;
}
