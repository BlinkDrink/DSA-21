#include<iostream>
#include "MyStore.hpp"
#include <climits>

using std::cout;
using std::cin;

struct MyActionHandler : ActionHandler {
	void onWorkerSend(int minute, ResourceType resource) override {
		cout << "W " << minute << " " << (resource == ResourceType::banana ? "banana" : "schweppes") << '\n';
	}

	void onWorkerBack(int minute, ResourceType resource) override {
		cout << "D " << minute << " " << (resource == ResourceType::banana ? "banana" : "schweppes") << '\n';
	}

	void onClientDepart(int index, int minute, int banana, int schweppes) override {
		cout << index << " " << minute << " " << banana << " " << schweppes << '\n';
	}
};

int main() {
	size_t workersCount, clientsForTheDay;
	MyStore ms;
	MyActionHandler myActionHandler;
	ms.setActionHandler(&myActionHandler);

	cin >> workersCount >> clientsForTheDay;
	ms.init(workersCount, 0, 0);
	Client* clients = new Client[clientsForTheDay];

	for (size_t i = 0; i < clientsForTheDay; i++) {
		cin >> clients[i].arriveMinute >> clients[i].banana >> clients[i].schweppes >> clients[i].maxWaitTime;
		int departTime = clients[i].arriveMinute + clients[i].maxWaitTime;
	}

	ms.addClients(clients, clientsForTheDay);
	ms.advanceTo(INT_MAX);

	delete[] clients;

	return 0;
}
