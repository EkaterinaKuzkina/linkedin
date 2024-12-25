#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

using namespace std;

struct Success {
  Success(string inMsg) : msg(inMsg) {};
  string msg;
};
struct Failure {
  Failure(string inErrMsg) : errMsg(inErrMsg) {};
  string errMsg;
};
struct OrderBookEntry {
  OrderBookEntry(double in_price, int in_quantity, string in_traderName)
      : price(in_price), quantity(in_quantity), traderName(in_traderName) {};
  double price;
  int quantity;
  string traderName;
};

bool cmp(pair<int, OrderBookEntry> &a, pair<int, OrderBookEntry> &b)
{
  return a.second.price < b.second.price;
}

unordered_map<int, OrderBookEntry> sort(unordered_map<int, OrderBookEntry> &M)
{
  vector<pair<int, OrderBookEntry>> A;
  for (auto &it : M) {
    A.push_back(it);
  }

  sort(A.begin(), A.end(), cmp);

  unordered_map<int, OrderBookEntry> sortedMap;
  for (auto &it : A) {
    sortedMap.insert({it.first, it.second});
  }
  return sortedMap;
}

// Dummy function
// We can use boolean or we can use specific types
variant<Success, Failure> execute(const OrderBookEntry &orderBookEntry,
                                  const OrderBookEntry &trade)
{
  return Success("Trade executed with " + orderBookEntry.traderName);
};

variant<Success, Failure> trade(unordered_map<int, OrderBookEntry> &orderBook,
                                const OrderBookEntry &orderToMatch)
{
  for (const auto &[key, order] : orderBook) {
    if (order.price <= orderToMatch.price &&
        order.quantity == orderToMatch.quantity) {
      // execute trade, we use a dummy function here in sake of example
      const auto result = execute(order, orderToMatch);
      return result;
    }
  }
  return Failure("No order to match with");
}

int main()
{
  // create an order book
  unordered_map<int, OrderBookEntry> orderBook;
  orderBook.insert({3, OrderBookEntry(10., 4, "Viktoria")});
  orderBook.insert({1, OrderBookEntry(11., 5, "Max")});
  orderBook.insert({4, OrderBookEntry(10., 5, "Nik")});
  orderBook.insert({2, OrderBookEntry(10.5, 5, "Victor")});

  // sort in ascending order by price as usual in order book for ask
  auto sortedOrderBook = sort(orderBook);

  // trade Alex wants to do
  OrderBookEntry alexOrder{10., 5, "Alex"};

  auto result = trade(sortedOrderBook, alexOrder);
  if (holds_alternative<Success>(result)) {
    cout << "Trade executed successfully. " << get<Success>(result).msg << endl;
  }
  else {
    cout << "Trade failed with error=" << get<Failure>(result).errMsg << endl;
  }

  return 0;
}
