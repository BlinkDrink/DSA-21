#define CATCH_CONFIG_MAIN

#include "interface.h"
#include "catch.hpp"

#include <vector>

const int RESTOCK_TIME = 60;
const int RESTOCK_AMOUNT = 100;

typedef std::vector<Client> ClientList;

struct StoreEvent {
	enum Type {
		WorkerSend, WorkerBack, ClientDepart
	};

	Type type;
	int minute;

	struct Worker {
		ResourceType resource;
	} worker;

	struct Client {
		int index;
		int banana;
		int schweppes;
	} client;
};

struct TestStore : ActionHandler {
	Store* impl = nullptr;
	std::vector<StoreEvent> log;

	TestStore()
		: impl(createStore()) {
		impl->setActionHandler(this);
	}

	~TestStore() {
		delete impl;
	}

	TestStore(const TestStore&) = delete;
	TestStore& operator=(const TestStore&) = delete;

	void init(int workerCount, int startBanana, int startSchweppes) {
		impl->init(workerCount, startBanana, startSchweppes);
	}

	void advanceTo(int minute) {
		impl->advanceTo(minute);
	}

	int getBanana() const {
		return impl->getBanana();
	}

	int getSchweppes() const {
		return impl->getSchweppes();
	}

	void addClients(const ClientList& clients) {
		impl->addClients(clients.data(), clients.size());
	}

	void addClients(const Client& single) {
		impl->addClients(&single, 1);
	}

	void onWorkerSend(int minute, ResourceType resource) override {
		StoreEvent ev;
		ev.type = StoreEvent::WorkerSend;
		ev.minute = minute;
		ev.worker.resource = resource;
		log.push_back(ev);
	}

	void onWorkerBack(int minute, ResourceType resource) override {
		StoreEvent ev;
		ev.type = StoreEvent::WorkerBack;
		ev.minute = minute;
		ev.worker.resource = resource;
		log.push_back(ev);
	}

	void onClientDepart(int index, int minute, int banana, int schweppes) override {
		StoreEvent ev;
		ev.type = StoreEvent::ClientDepart;
		ev.minute = minute;
		ev.client.banana = banana;
		ev.client.schweppes = schweppes;
		ev.client.index = index;
		log.push_back(ev);
	}
};

#define LastEvent() (store.log.back())

TEST_CASE("No workers, empty store, up to one client") {
	TestStore store;
	store.init(0, 0, 0);

	SECTION("No events") {
		INFO("Without clients, no events should be generated");
		REQUIRE(store.log.size() == 0);
		store.advanceTo(0);
		REQUIRE(store.log.size() == 0);
		store.advanceTo(1);
		REQUIRE(store.log.size() == 0);

		store.advanceTo(100);
		REQUIRE(store.log.size() == 0);
	}

	SECTION("Advance before depart time") {
		store.addClients(Client{ 0, 1, 1, 1 });
		store.advanceTo(0);
		INFO("Must not generate event before time is advanced to its time");
		REQUIRE(store.log.size() == 0);
	}

	SECTION("Depart time") {
		INFO("maxWaitTime == 1 means, client will wait 1 minute (arrive at 0, depart at 1)");
		INFO("advanceTo(<time>), should generate all events that happen up to and including <time>");
		store.addClients(Client{ 0, 1, 1, 1 });
		store.advanceTo(1);
		REQUIRE(store.log.size() == 1);
	}

	SECTION("client without request") {
		INFO("Client with wait time 0, must generate event at the time of arrival");
		store.addClients(Client{ 0, 0, 0, 0 });
		store.advanceTo(0);
		REQUIRE(store.log.size() == 1);

		INFO("Client without any request should depart empty - (0 banana, 0 schweppes)");
		REQUIRE(LastEvent().minute == 0);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 0);
		REQUIRE(LastEvent().client.schweppes == 0);
	}

	SECTION("advance before client departs") {
		store.addClients(Client{ 0, 1, 1, 10 });

		store.advanceTo(3);
		INFO("client must not depart before its max wait time");
		REQUIRE(store.log.size() == 0);
	}
}

TEST_CASE("No workers, full store") {
	TestStore store;
	store.init(0, 1000, 1000);
	const ClientList three = {
		Client{0, 0, 10, 10},
		Client{0, 10, 0, 3},
		Client{0, 10, 10, 5}
	};

	SECTION("Non waiting client") {
		store.addClients(Client{ 0, 10, 0, 0 });
		store.advanceTo(0);
		INFO("Client must depart same at the time of arrival when store has enough resources");
		REQUIRE(store.log.size() == 1);
		REQUIRE(store.log[0].type == StoreEvent::ClientDepart);
	}

	SECTION("Client with wait time") {
		store.addClients(Client{ 0, 10, 0, 10 });
		store.advanceTo(0);
		INFO("Client must depart same at the time of arrival when store has enough resources");
		REQUIRE(store.log.size() == 1);
		REQUIRE(store.log[0].type == StoreEvent::ClientDepart);
	}

	SECTION("Multiple clients") {
		store.addClients(three);
		store.advanceTo(0);
		INFO("Client must depart same at the time of arrival when store has enough resources");
		REQUIRE(store.log.size() == 3);
		REQUIRE(store.log[0].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[1].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[2].type == StoreEvent::ClientDepart);
	}

	SECTION("Client indices") {
		store.addClients(three);
		store.advanceTo(0);
		INFO("Indices must be correct");
		REQUIRE(store.log[0].client.index == 0);
		REQUIRE(store.log[1].client.index == 1);
		REQUIRE(store.log[2].client.index == 2);
	}
}

TEST_CASE("Multiple stores") {
	TestStore bananaStore;
	bananaStore.init(0, 100, 0);
	TestStore schweppesStore;
	schweppesStore.init(0, 0, 100);

	SECTION("Two stores") {
		INFO("Multiple stores must be ebable to exist at the same time");

		REQUIRE(bananaStore.getBanana() == 100);
		REQUIRE(bananaStore.getSchweppes() == 0);

		REQUIRE(schweppesStore.getBanana() == 0);
		REQUIRE(schweppesStore.getSchweppes() == 100);
	}

	SECTION("Clients to one of the stores") {
		bananaStore.addClients(Client{ 0, 10, 0, 10 });
		bananaStore.advanceTo(0);
		schweppesStore.advanceTo(0);

		INFO("New client to one store must not generate event in the other store");
		REQUIRE(bananaStore.log.size() == 1);
		REQUIRE(schweppesStore.log.size() == 0);
	}

	SECTION("Clients to both stores") {
		bananaStore.addClients(Client{ 0, 5, 0, 0 });
		schweppesStore.addClients(Client{ 0, 0, 10, 0 });

		bananaStore.advanceTo(0);
		schweppesStore.advanceTo(0);

		INFO("Both stores should generate valid evenets");
		REQUIRE(bananaStore.log.back().type == StoreEvent::ClientDepart);
		REQUIRE(bananaStore.log.back().client.banana == 5);

		REQUIRE(schweppesStore.log.back().type == StoreEvent::ClientDepart);
		REQUIRE(schweppesStore.log.back().client.schweppes == 10);
	}
}

TEST_CASE("Example") {
	TestStore store;
	store.init(5, 0, 0);

	store.addClients({
		Client{0, 10, 0, 10},
		Client{45, 35, 0, 30},
		Client{46, 30, 20, 100},
		Client{200, 10, 10, 1}
		});

	SECTION("First client") {
		store.advanceTo(0);
		INFO("First client will trigger 1 worker");
		REQUIRE(store.log.size() == 1);
		REQUIRE(LastEvent().type == StoreEvent::WorkerSend);
	}

	SECTION("First client") {
		store.advanceTo(10);
		INFO("First client must depart without anything");
		REQUIRE(store.log.size() == 2);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 0);
	}

	SECTION("Last client") {
		store.advanceTo(200);
		INFO("Last client departs same time as arrival");
		REQUIRE(store.log.size() == 8);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 10);
		REQUIRE(LastEvent().client.schweppes == 10);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 0;
		int schweppes = 0;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount - client depart amounth must be what is left in the store");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}

TEST_CASE("Workers can be sent out only when needed") {
	TestStore store;
	store.init(1, 100, 0);

	store.addClients({
		Client{0, 100, 0, 1}, // this client will get everything
		Client{10, 1, 0, 1}
		});


	SECTION("Before second client") {
		store.advanceTo(9);

		INFO("Worker must not be sent before client arrives");
		REQUIRE(store.log.size() == 1);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(store.getBanana() == 0);
	}

	SECTION("After second client") {
		store.advanceTo(10);

		INFO("Worker must be sent the same minute second clients arrives");
		REQUIRE(store.log.size() == 2);
		REQUIRE(LastEvent().type == StoreEvent::WorkerSend);
		REQUIRE(LastEvent().minute == 10);
	}
}

TEST_CASE("Workers must not be sent if resource will be available before client departs") {
	TestStore store;
	store.init(2, 0, 0);

	store.addClients({
		Client{0, 10, 0, 200},
		Client{10, 10, 0, 200}
		});

	SECTION("First client must trigger a worker") {
		store.advanceTo(5);
		REQUIRE(store.log.size() == 1);
	}

	SECTION("Second client must not trigger a worker") {
		store.advanceTo(15);
		REQUIRE(store.log.size() == 1);
	}
}

TEST_CASE("Clients depart and take what they can") {
	TestStore store;
	store.init(5, 10, 0);

	store.addClients({
		Client{0, 0, 10, 0}, // Trigger worker sent for schweppes
		Client{1, 20, 0, 5}
		});

	SECTION("Sent out workers") {
		store.advanceTo(3);
		INFO("Store must send 2 workers, and 1 client has departed");
		REQUIRE(store.log.size() == 3);
	}

	SECTION("Client departs with only part of requirement") {
		store.advanceTo(1 + 5);

		INFO("Client must take whatever is available");
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 10);
		REQUIRE(LastEvent().client.schweppes == 0);
	}
}

TEST_CASE("Clients arrive/depart in mixed order") {
	TestStore store;
	store.init(2, 10, 0);

	store.addClients({
		Client{0, 10, 10, 20},
		Client{10, 10, 0, 0}
		});

	SECTION("One worker must be sent") {
		store.advanceTo(0);

		INFO("First client must trigger a worker to restock schweppes");
		REQUIRE(store.log.size() == 1);
		REQUIRE(LastEvent().type == StoreEvent::WorkerSend);
		REQUIRE(LastEvent().worker.resource == ResourceType::schweppes);
	}

	SECTION("First client") {
		store.advanceTo(20);

		INFO("First client departs nothing after second client");
		REQUIRE(store.log.size() == 3);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 0);
		REQUIRE(LastEvent().client.index == 0);
	}

	SECTION("Second client") {
		store.advanceTo(10);

		INFO("Second client comes at min 10, but waits 0, departs at min 10");
		REQUIRE(store.log.size() == 2);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 10);
		REQUIRE(LastEvent().client.index == 1);
	}

}

/// Custom tests 

TEST_CASE("Different example #1") {
	TestStore store;
	store.init(4, 0, 0);

	store.addClients({
		Client{0, 100, 100, 70},
		Client{5, 100, 100, 65},
		});

	SECTION("First client") {
		store.advanceTo(0);
		INFO("First client will trigger 2 workers");
		REQUIRE(store.log.size() == 2);
		REQUIRE(store.log[0].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[0].worker.resource == ResourceType::banana);
		REQUIRE(store.log[1].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[1].worker.resource == ResourceType::schweppes);
	}

	SECTION("Workers") {
		store.advanceTo(60);
		INFO("Two workers should return at this time, one client must depart");
		REQUIRE(store.log.size() == 5);
		REQUIRE(store.log[2].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[2].worker.resource == ResourceType::banana);
		REQUIRE(store.log[3].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[3].worker.resource == ResourceType::schweppes);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 100);
		REQUIRE(LastEvent().client.schweppes == 100);
		REQUIRE(LastEvent().client.index == 0);
	}

	SECTION("Last Client") {
		store.advanceTo(70);
		INFO("Last client should depart the store carrying no items and at minute 70");
		REQUIRE(store.log.size() == 6);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 0);
		REQUIRE(LastEvent().client.schweppes == 0);
		REQUIRE(LastEvent().client.index == 1);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 0;
		int schweppes = 0;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}

TEST_CASE("Different example #2") {
	TestStore store;
	store.init(6, 0, 0);

	store.addClients({
		Client{0 , 100, 100, 70},
		Client{0 , 100, 100, 70},
		Client{5 , 0,   100, 70},
		Client{5 ,   100, 0, 70},
		});

	SECTION("First client") {
		store.advanceTo(0);
		INFO("First client will trigger 2 workers");
		REQUIRE(store.log.size() == 2);
		REQUIRE(store.log[0].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[0].worker.resource == ResourceType::banana);
		REQUIRE(LastEvent().type == StoreEvent::WorkerSend);
		REQUIRE(LastEvent().worker.resource == ResourceType::schweppes);
	}

	SECTION("Workers") {
		store.advanceTo(60);
		INFO("Two workers should return at this time, client with index 0 must depart");
		REQUIRE(store.log.size() == 5);
		REQUIRE(store.log[2].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[2].worker.resource == ResourceType::banana);
		REQUIRE(store.log[3].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[3].worker.resource == ResourceType::schweppes);
		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.index == 0);
		REQUIRE(LastEvent().client.banana == 100);
		REQUIRE(LastEvent().client.schweppes == 100);
	}

	SECTION("Last 3 clients") {
		store.advanceTo(75);
		INFO("Last 3 clients should depart the store carrying no items and at minute 75");
		REQUIRE(store.log.size() == 8);

		REQUIRE(store.log[5].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[5].client.banana == 0);
		REQUIRE(store.log[5].client.schweppes == 0);
		REQUIRE(store.log[5].client.index == 1);

		REQUIRE(store.log[6].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[6].client.banana == 0);
		REQUIRE(store.log[6].client.schweppes == 0);
		REQUIRE(store.log[6].client.index == 2);

		REQUIRE(store.log[7].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[7].client.banana == 0);
		REQUIRE(store.log[7].client.schweppes == 0);
		REQUIRE(store.log[7].client.index == 3);

		REQUIRE(LastEvent().type == StoreEvent::ClientDepart);
		REQUIRE(LastEvent().client.banana == 0);
		REQUIRE(LastEvent().client.schweppes == 0);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 0;
		int schweppes = 0;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}

TEST_CASE("Different example #3") {
	TestStore store;
	store.init(2, 20, 20);

	store.addClients({
		Client{0,50,0,70},
		Client{5,60,0,5},
		});

	SECTION("First client") {
		store.advanceTo(0);
		INFO("First client will trigger 1 worker");
		REQUIRE(store.log.size() == 1);

		REQUIRE(LastEvent().type == StoreEvent::WorkerSend);
		REQUIRE(LastEvent().worker.resource == ResourceType::banana);
	}

	SECTION("Second Client") {
		store.advanceTo(5);
		INFO("Second should not trigger any workers");
		REQUIRE(store.log.size() == 1);
		REQUIRE(LastEvent().type == StoreEvent::WorkerSend);
		REQUIRE(LastEvent().worker.resource == ResourceType::banana);
	}

	SECTION("Advancing to given time") {
		store.advanceTo(7);
		INFO("Should not trigger events");
		REQUIRE(store.log.size() == 1);
		store.advanceTo(9);
		REQUIRE(store.log.size() == 1);
	}

	SECTION("Last 3 events") {
		store.advanceTo(60);
		INFO("Second Client departs first");
		REQUIRE(store.log.size() == 4);

		INFO("Client with index 1 departs before client with index 0");
		REQUIRE(store.log[1].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[1].client.banana == 20);
		REQUIRE(store.log[1].client.schweppes == 0);
		REQUIRE(store.log[1].client.index == 1);

		INFO("3rd event must be worker back, carrying bananas");
		REQUIRE(store.log[2].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[2].worker.resource == ResourceType::banana);

		INFO("Client with index 0 departs last from the store");
		REQUIRE(store.log[3].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[3].client.banana == 50);
		REQUIRE(store.log[3].client.schweppes == 0);
		REQUIRE(store.log[3].client.index == 0);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 20;
		int schweppes = 20;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}

TEST_CASE("Different example #4") {
	TestStore store;
	store.init(3, 0, 0);

	store.addClients({
		Client{0,210,50,60},
		});

	SECTION("First client") {
		store.advanceTo(0);
		INFO("First client will trigger 3 workers");
		REQUIRE(store.log.size() == 3);

		INFO("First Worker");
		REQUIRE(store.log[0].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[0].worker.resource == ResourceType::banana);

		INFO("Second Worker");
		REQUIRE(store.log[1].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[1].worker.resource == ResourceType::banana);

		INFO("Third Worker");
		REQUIRE(store.log[2].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[2].worker.resource == ResourceType::schweppes);
	}

	SECTION("Workers should return simultaneously at min 60, client must depart") {
		store.advanceTo(60);
		INFO("We must have 6 events, because at min 60 client also departs");
		REQUIRE(store.log.size() == 7);

		INFO("First Worker");
		REQUIRE(store.log[3].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[3].worker.resource == ResourceType::banana);

		INFO("Second Worker");
		REQUIRE(store.log[4].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[4].worker.resource == ResourceType::banana);

		INFO("Third Worker");
		REQUIRE(store.log[5].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[5].worker.resource == ResourceType::schweppes);

		INFO("Client with index 0 departs at min 60");
		REQUIRE(store.log[6].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[6].client.banana == 200);
		REQUIRE(store.log[6].client.schweppes == 50);
		REQUIRE(store.log[6].client.index == 0);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 0;
		int schweppes = 0;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}

TEST_CASE("Different example #5") {
	TestStore store;
	store.init(5, 0, 0);

	store.addClients({
		Client{1,30,30,15},
		Client{45,30,30,80},
		Client{60,50,50,100},
		Client{61,10,0,10},
		Client{70,20,30,50},
		});

	SECTION("First client") {
		store.advanceTo(1);
		INFO("First client will trigger 2 workers");
		REQUIRE(store.log.size() == 2);

		INFO("First Worker");
		REQUIRE(store.log[0].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[0].worker.resource == ResourceType::banana);

		INFO("Second Worker");
		REQUIRE(store.log[1].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[1].worker.resource == ResourceType::schweppes);
	}

	SECTION("First client") {
		store.advanceTo(16);
		INFO("First client departs at min 16, taking nothing");
		REQUIRE(store.log.size() == 3);

		INFO("Client with index 0 departs at min 16");
		REQUIRE(store.log[2].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[2].client.banana == 0);
		REQUIRE(store.log[2].client.schweppes == 0);
		REQUIRE(store.log[2].client.index == 0);
	}

	SECTION("Advancing to given time") {
		store.advanceTo(30);
		INFO("Should not trigger events");
		REQUIRE(store.log.size() == 3);
		store.advanceTo(60);
		REQUIRE(store.log.size() == 3);
	}

	SECTION("Advancing to minute 61 should trigger 5 more events") {
		store.advanceTo(61);
		INFO("8 events total");
		REQUIRE(store.log.size() == 8);

		INFO("First Worker arrives at min 61");
		REQUIRE(store.log[3].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[3].worker.resource == ResourceType::banana);

		INFO("Second Worker arrives at min 61 after First Worker");
		REQUIRE(store.log[4].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[4].worker.resource == ResourceType::schweppes);

		INFO("Client with index 1 departs");
		REQUIRE(store.log[5].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[5].client.banana == 30);
		REQUIRE(store.log[5].client.schweppes == 30);
		REQUIRE(store.log[5].client.index == 1);

		INFO("Client with index 2 departs");
		REQUIRE(store.log[6].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[6].client.banana == 50);
		REQUIRE(store.log[6].client.schweppes == 50);
		REQUIRE(store.log[6].client.index == 2);

		INFO("Client with index 3 departs");
		REQUIRE(store.log[7].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[7].client.banana == 10);
		REQUIRE(store.log[7].client.schweppes == 0);
		REQUIRE(store.log[7].client.index == 3);
	}

	SECTION("Last 5 events should occur at minute 70 and minute 130") {
		store.advanceTo(130);
		INFO("8 events total");
		REQUIRE(store.log.size() == 13);

		INFO("Client with index 4 triggers two workers");
		REQUIRE(store.log[8].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[8].worker.resource == ResourceType::banana);

		INFO("Second Worker arrives at min 61 after First Worker");
		REQUIRE(store.log[9].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[9].worker.resource == ResourceType::schweppes);

		INFO("Client with index 4 departs last from the store at minute 120");
		REQUIRE(store.log[10].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[10].client.banana == 10);
		REQUIRE(store.log[10].client.schweppes == 20);
		REQUIRE(store.log[10].client.index == 4);
		REQUIRE(store.log[10].minute == 120);

		INFO("Second Worker arrives at min 130 after First Worker");
		REQUIRE(store.log[11].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[11].worker.resource == ResourceType::banana);
		REQUIRE(store.log[11].minute == 130);

		INFO("Second Worker arrives at min 130 after First Worker");
		REQUIRE(store.log[12].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[12].worker.resource == ResourceType::schweppes);
		REQUIRE(store.log[12].minute == 130);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 0;
		int schweppes = 0;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}

TEST_CASE("Different example #6") {
	TestStore store;
	store.init(4, 0, 0);

	store.addClients({
		Client{1,50,80,65},
		Client{1,60,40,60},
		Client{1,10,90,60},
		Client{1,80,10,61},
		});

	SECTION("First client") {
		store.advanceTo(1);
		INFO("First client will trigger 2 workers");
		REQUIRE(store.log.size() == 2);

		INFO("First Worker");
		REQUIRE(store.log[0].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[0].worker.resource == ResourceType::schweppes);

		INFO("Second Worker");
		REQUIRE(store.log[1].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[1].worker.resource == ResourceType::banana);
	}

	SECTION("Two Workers and Three Clients") {
		store.advanceTo(61);
		INFO("Two workers arrive at minute 61");
		REQUIRE(store.log.size() == 7);

		INFO("First Worker");
		REQUIRE(store.log[2].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[2].worker.resource == ResourceType::schweppes);

		INFO("Second Worker");
		REQUIRE(store.log[3].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[3].worker.resource == ResourceType::banana);

		INFO("Three clients depart from the store at minute 61");
		INFO("Client with index 0 departs");
		REQUIRE(store.log[4].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[4].client.banana == 50);
		REQUIRE(store.log[4].client.schweppes == 80);
		REQUIRE(store.log[4].client.index == 0);

		INFO("Client with index 1 departs");
		REQUIRE(store.log[5].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[5].client.banana == 50);
		REQUIRE(store.log[5].client.schweppes == 20);
		REQUIRE(store.log[5].client.index == 1);

		INFO("Client with index 2 departs");
		REQUIRE(store.log[6].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[6].client.banana == 0);
		REQUIRE(store.log[6].client.schweppes == 0);
		REQUIRE(store.log[6].client.index == 2);
	}

	SECTION("Advancing to minute 62 should trigger the last event") {
		store.advanceTo(62);
		INFO("8 events total");
		REQUIRE(store.log.size() == 8);

		INFO("Client with index 3 departs");
		REQUIRE(store.log[7].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[7].client.banana == 0);
		REQUIRE(store.log[7].client.schweppes == 0);
		REQUIRE(store.log[7].client.index == 3);
		REQUIRE(store.log[7].minute == 62);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 0;
		int schweppes = 0;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}

TEST_CASE("Different example #7") {
	TestStore store;
	store.init(6, 0, 0);

	store.addClients({
		Client{0,80,100,1},
		Client{0,120,100,60},
		Client{0,150,150,60},
		Client{10,201,201,60},
		});

	SECTION("First three clients") {
		store.advanceTo(0);
		INFO("First three clients will trigger 4 workers");
		REQUIRE(store.log.size() == 4);

		INFO("First Worker");
		REQUIRE(store.log[0].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[0].worker.resource == ResourceType::schweppes);

		INFO("Second Worker");
		REQUIRE(store.log[1].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[1].worker.resource == ResourceType::banana);

		INFO("Third Worker");
		REQUIRE(store.log[2].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[2].worker.resource == ResourceType::banana);

		INFO("Fourth Worker");
		REQUIRE(store.log[3].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[3].worker.resource == ResourceType::schweppes);
	}

	SECTION("First client") {
		store.advanceTo(1);
		INFO("First client departs at minute 1");
		REQUIRE(store.log.size() == 5);

		INFO("First client departs from the store at minute 1");
		INFO("Client with index 0 departs");
		REQUIRE(store.log[4].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[4].client.banana == 0);
		REQUIRE(store.log[4].client.schweppes == 0);
		REQUIRE(store.log[4].client.index == 0);
	}

	SECTION("Fourth Client") {
		store.advanceTo(10);
		INFO("Fourth client triggers two workers at minute 10arrive at minute 10");
		REQUIRE(store.log.size() == 7);

		INFO("First Worker");
		REQUIRE(store.log[5].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[5].worker.resource == ResourceType::banana);

		INFO("Second Worker");
		REQUIRE(store.log[6].type == StoreEvent::WorkerSend);
		REQUIRE(store.log[6].worker.resource == ResourceType::schweppes);
	}

	SECTION("Four workers arrive at minute 60, 2 clients depart at minute 60") {
		store.advanceTo(60);
		INFO("8 events total");
		REQUIRE(store.log.size() == 13);

		INFO("First Worker");
		REQUIRE(store.log[7].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[7].worker.resource == ResourceType::schweppes);

		INFO("Second Worker");
		REQUIRE(store.log[8].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[8].worker.resource == ResourceType::banana);

		INFO("Third Worker");
		REQUIRE(store.log[9].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[9].worker.resource == ResourceType::banana);

		INFO("Fourth Worker");
		REQUIRE(store.log[10].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[10].worker.resource == ResourceType::schweppes);

		INFO("Client with index 1 departs min 60");
		REQUIRE(store.log[11].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[11].client.banana == 120);
		REQUIRE(store.log[11].client.schweppes == 100);
		REQUIRE(store.log[11].client.index == 1);
		REQUIRE(store.log[11].minute == 60);

		INFO("Client with index 2 departs at min 60");
		REQUIRE(store.log[12].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[12].client.banana == 80);
		REQUIRE(store.log[12].client.schweppes == 100);
		REQUIRE(store.log[12].client.index == 2);
		REQUIRE(store.log[12].minute == 60);
	}

	SECTION("Fourth client, incoming workers") {
		store.advanceTo(70);
		INFO("Fourth client departs at minute 70");
		REQUIRE(store.log.size() == 16);

		INFO("First Worker");
		REQUIRE(store.log[13].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[13].worker.resource == ResourceType::banana);

		INFO("Second Worker");
		REQUIRE(store.log[14].type == StoreEvent::WorkerBack);
		REQUIRE(store.log[14].worker.resource == ResourceType::schweppes);

		INFO("Client with index 3 departs at min 60");
		REQUIRE(store.log[15].type == StoreEvent::ClientDepart);
		REQUIRE(store.log[15].client.banana == 100);
		REQUIRE(store.log[15].client.schweppes == 100);
		REQUIRE(store.log[15].client.index == 3);
		REQUIRE(store.log[15].minute == 70);
	}

	SECTION("Remaining resources") {
		store.advanceTo(500);
		int bananas = 0;
		int schweppes = 0;
		for (int c = 0; c < store.log.size(); c++) {
			const StoreEvent& ev = store.log[c];
			if (ev.type == StoreEvent::WorkerBack) {
				REQUIRE(store.log[c].worker.resource >= ResourceType::banana);
				REQUIRE(store.log[c].worker.resource <= ResourceType::schweppes);
				if (ev.worker.resource == ResourceType::banana) {
					bananas += RESTOCK_AMOUNT;
				}
				else if (ev.worker.resource == ResourceType::schweppes) {
					schweppes += RESTOCK_AMOUNT;
				}
			}
			else if (ev.type == StoreEvent::ClientDepart) {
				bananas -= ev.client.banana;
				schweppes -= ev.client.schweppes;
			}
		}

		INFO("Stock amount");
		REQUIRE(store.getBanana() == bananas);
		REQUIRE(store.getSchweppes() == schweppes);
	}
}