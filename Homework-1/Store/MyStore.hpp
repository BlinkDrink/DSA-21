#pragma once
#include "../StoreTests/interface.h"
#include "List.hpp"
#include "LinkedQueue.hpp"
#include"MyClient.h"
#include"MyWorker.h"

using std::invalid_argument;
using std::logic_error;

struct MyStore : Store {

	/// @brief Interface methods
public:
	ActionHandler* actionHandler = nullptr;

	void setActionHandler(ActionHandler* handler) override
	{
		actionHandler = handler;
	}

	void init(int workerCount, int startBanana, int startSchweppes) override
	{
		fBananas = startBanana;
		fSchweppes = startSchweppes;
		fWorkersCount = workerCount;
	}

	void addClients(const Client* clients, int count) override
	{
		int clientIndex = 0;
		for (int i = 0; i < count; i++) {
			MyClient c(clients[i].arriveMinute, clients[i].banana, clients[i].schweppes, clients[i].maxWaitTime, clientIndex++);
			fClients.push(c);
		}
	}

	void advanceTo(int minute) override
	{
		do {
			bool areThereClients = !fClients.empty();
			bool areThereWaitingClients = !fWaitingClients.empty();
			bool areThereWorkers = !fWorkers.empty();

			if (thereIsActionForThisMinute(minute))
				doAction(areThereClients, areThereWaitingClients, areThereWorkers, minute);
			else
				fMinute = minute;

		} while (thereIsActionForThisMinute(minute) || fMinute != minute);
	}

	virtual int getBanana() const { return fBananas; }

	virtual int getSchweppes() const { return fSchweppes; }

	/// @brief Private member functions
private:

	/// @brief Decides which product we should send the worker for
	/// @param c current client that we will check what products they want
	/// @return The resource type that we will send the worker for
	ResourceType desiredResource(const MyClient& c) const
	{
		if (c.banana < c.schweppes)
			return ResourceType::schweppes;

		return ResourceType::banana;
	}

	/// @brief Does the approriate action in the store depending on wheter we have
	/// non-empty clients, waiting clients and workers containers
	/// @param areThereClients Whether we have incoming clients or not
	/// @param areThereWaitingClients Whether we have waiting clients or not
	/// @param areThereWorkers  Whether we have incoming workers or not
	/// @param minute the minute we strive to reach
	void doAction(bool areThereClients, bool areThereWaitingClients, bool areThereWorkers, int minute)
	{
		if (!areThereClients && !areThereWaitingClients && !areThereWorkers) /// We have an empty store
		{
			fMinute = minute;
		}
		else if (areThereClients && !areThereWaitingClients && !areThereWorkers) /// We only have incoming clients
		{
			if (fClients.top().arriveMinute <= minute)
				fMinute = fClients.top().arriveMinute;

			serveIncomingClient();
		}
		else if (!areThereClients && !areThereWaitingClients && areThereWorkers) /// We only have incoming workers
		{
			if (fWorkers.front().arriveMinute <= minute)
				fMinute = fWorkers.front().arriveMinute;

			serveIncomingWorker();
		}
		else if (!areThereClients && areThereWaitingClients && !areThereWorkers) /// We only have waiting clients
		{
			List<MyClient>::Node* priorityClient = findHighestPriorityWaitingClient();

			if (getClientDepartureTime(priorityClient->fData) <= minute)
				fMinute = getClientDepartureTime(priorityClient->fData);

			serveDepartingWaitingClient(priorityClient);
		}
		else if (!areThereClients && areThereWaitingClients && areThereWorkers) /// We have waiting clients and incoming workers
		{
			List<MyClient>::Node* priorityClient = findHighestPriorityWaitingClient();
			MyWorker incWorker = fWorkers.front();

			if (incWorker.arriveMinute <= getClientDepartureTime(priorityClient->fData))
			{
				if (incWorker.arriveMinute <= minute)
					fMinute = incWorker.arriveMinute;

				serveIncomingWorker();
			}
			else {
				if (getClientDepartureTime(priorityClient->fData) <= minute)
					fMinute = getClientDepartureTime(priorityClient->fData);

				serveDepartingWaitingClient(priorityClient);
			}
		}
		else if (areThereClients && areThereWaitingClients && !areThereWorkers) /// We have waiting clients and incoming clients
		{
			List<MyClient>::Node* firstClientToDepart = findHighestPriorityWaitingClient();
			MyClient incClient = fClients.top();
			if (incClient.arriveMinute <= getClientDepartureTime(firstClientToDepart->fData))
			{
				if (incClient.arriveMinute <= minute)
					fMinute = incClient.arriveMinute;

				serveIncomingClient();
			}
			else
			{
				if (getClientDepartureTime(firstClientToDepart->fData) <= minute)
					fMinute = getClientDepartureTime(firstClientToDepart->fData);

				serveDepartingWaitingClient(firstClientToDepart);
			}
		}
		else if (areThereClients && !areThereWaitingClients && areThereWorkers) /// We have workers and incoming workers
		{
			MyClient curr = fClients.top();
			if (curr.arriveMinute <= fWorkers.front().arriveMinute)
			{
				if (curr.arriveMinute <= minute)
					fMinute = curr.arriveMinute;

				serveIncomingClient();
			}
			else
			{
				if (fWorkers.front().arriveMinute <= minute)
					fMinute = fWorkers.front().arriveMinute;

				serveIncomingWorker();
			}
		}
		else /// We have incoming clients, waiting clients and incoming workers
		{
			List<MyClient>::Node* clientToDepart = findHighestPriorityWaitingClient();
			MyClient incClient = fClients.top();
			MyWorker incWorker = fWorkers.front();
			if (incClient.arriveMinute <= incWorker.arriveMinute && incClient.arriveMinute <= getClientDepartureTime(clientToDepart->fData))
			{
				if (incClient.arriveMinute <= minute)
					fMinute = incClient.arriveMinute;

				serveIncomingClient();
			}
			else if (incWorker.arriveMinute < incClient.arriveMinute && incWorker.arriveMinute <= getClientDepartureTime(clientToDepart->fData))
			{
				if (incWorker.arriveMinute <= minute)
					fMinute = incWorker.arriveMinute;

				serveIncomingWorker();
			}
			else
			{
				if (getClientDepartureTime(clientToDepart->fData) <= minute)
					fMinute = getClientDepartureTime(clientToDepart->fData);

				serveDepartingWaitingClient(clientToDepart);
			}
		}
	}

	/// @brief Calculates the time of departure of the given client
	/// @param c Client whoose departure time we will calculate
	/// @return the departure time of the client
	int getClientDepartureTime(const MyClient& c) const
	{
		return c.arriveMinute + c.maxWaitTime;
	}

	/// @brief Finds the client closest to departure in the waiting clients list
	/// @return The target client
	List<MyClient>::Node* findClientClosestToDeparture()
	{
		if (fWaitingClients.empty()) {
			return nullptr;
		}

		if (fWaitingClients.begin()->fpNext == nullptr) {
			return fWaitingClients.begin();
		}

		List<MyClient>::Node* result = fWaitingClients.begin();
		List<MyClient>::Node* curr = fWaitingClients.begin()->fpNext;

		while (curr != nullptr)
		{
			if (getClientDepartureTime(result->fData) > getClientDepartureTime(curr->fData))
			{
				result = curr;
			}
			else if (getClientDepartureTime(result->fData) == getClientDepartureTime(curr->fData))
			{
				if (result->fData.index > curr->fData.index)
				{
					result = curr;
				}
			}

			curr = curr->fpNext;
		}

		return result;
	}

	/// @brief Searches for the waiting client who can be send off with products immediately or their maxWaitTime has ended
	/// @return The target client
	List<MyClient>::Node* findHighestPriorityWaitingClient()
	{
		if (fWaitingClients.empty())
			return nullptr;

		if (fWaitingClients.begin()->fpNext == nullptr)
			return fWaitingClients.begin();

		List<MyClient>::Node* curr = fWaitingClients.begin();
		while (curr != nullptr)
		{
			if (getClientDepartureTime(curr->fData) <= fMinute || (fBananas >= curr->fData.banana && fSchweppes >= curr->fData.schweppes))
				return curr;

			curr = curr->fpNext;
		}

		return findClientClosestToDeparture();
	}

	/// @brief Serve the first departing client in the queue of waiting clients
	/// @param c The client which will be send out
	/// @param clients Clients queue from which the clients gets removed
	void serveDepartingWaitingClient(List<MyClient>::Node*& clientNode)
	{
		if (getClientDepartureTime(clientNode->fData) > fMinute && (fBananas < clientNode->fData.banana || fSchweppes < clientNode->fData.schweppes))
			return;

		int bananasTaken = clientNode->fData.banana <= fBananas ? clientNode->fData.banana : fBananas;
		int schweppesTaken = clientNode->fData.schweppes <= fSchweppes ? clientNode->fData.schweppes : fSchweppes;

		actionHandler->onClientDepart(clientNode->fData.index, fMinute, bananasTaken, schweppesTaken);
		clientNode = fWaitingClients.erase(clientNode);

		fBananas -= bananasTaken;
		fSchweppes -= schweppesTaken;
	}

	/// @brief Used in serveIncomingClient to check if we can send off an incoming client immediately
	/// @param c The client which we will check 
	void serveDepartingIncomingClient(MyClient c)
	{
		if (getClientDepartureTime(c) > fMinute && (fBananas < c.banana || fSchweppes < c.schweppes))
			return;

		int bananasTaken = c.banana <= fBananas ? c.banana : fBananas;
		int schweppesTaken = c.schweppes <= fSchweppes ? c.schweppes : fSchweppes;

		actionHandler->onClientDepart(c.index, fMinute, bananasTaken, schweppesTaken);

		fBananas -= bananasTaken;
		fSchweppes -= schweppesTaken;
		fClients.pop();
	}

	/// @brief Serves the first incoming client in the queue of incoming clients
	void serveIncomingClient()
	{
		MyClient currClient = fClients.top();

		if (fBananas >= currClient.banana && fSchweppes >= currClient.schweppes)
		{
			serveDepartingIncomingClient(currClient);
		}
		else
		{
			sendWorkerOut(currClient);
			fWaitingClients.push_back(fClients.top());
			fClients.pop();
		}
	}

	/// @brief Sends worker out for products if needed, if not - does nothing
	/// @param c Client which we will check if they will trigger the worker to go for products
	void sendWorkerOut(const MyClient& c)
	{
		if (fWorkersCount == 0)
			return;

		int tmpTotalBananas = fBananas + fIncomingBananas;
		int tmpTotalSchweppes = fSchweppes + fIncomingSchweppes;

		if (tmpTotalBananas >= c.banana && tmpTotalSchweppes >= c.schweppes)
			return;

		MyClient cpy = c;
		cpy.banana -= tmpTotalBananas;
		cpy.schweppes -= tmpTotalSchweppes;

		while ((cpy.banana > 0 || cpy.schweppes > 0) && fWorkersCount > 0)
		{
			ResourceType resource = desiredResource(cpy);
			fWorkers.push_back({ fMinute + RESTOCK_TIME, resource });
			fWorkersCount--;
			actionHandler->onWorkerSend(c.arriveMinute, resource);
			resource == ResourceType::banana ? cpy.banana -= RESTOCK_AMOUNT : cpy.schweppes -= RESTOCK_AMOUNT;
			resource == ResourceType::banana ? tmpTotalBananas += RESTOCK_AMOUNT : tmpTotalSchweppes += RESTOCK_AMOUNT;
			resource == ResourceType::banana ? fIncomingBananas += RESTOCK_AMOUNT : fIncomingSchweppes += RESTOCK_AMOUNT;
		}
	}

	/// @brief Unloads the first incoming worker and adds his resources to the store, also checks whether we can send off a waiting client
	/// @return True if the action was completed(means that fMinute is higher or
	/// equal to the time of arrival of worker), false otherwise
	void serveIncomingWorker()
	{
		while (!fWorkers.empty() && fWorkers.front().arriveMinute == fMinute)
		{
			MyWorker incWorker = fWorkers.front();
			incWorker.resource == ResourceType::banana ? fBananas += RESTOCK_AMOUNT : fSchweppes += RESTOCK_AMOUNT;
			incWorker.resource == ResourceType::banana ? fIncomingBananas -= RESTOCK_AMOUNT : fIncomingSchweppes -= RESTOCK_AMOUNT;
			fWorkersCount++;
			fWorkers.pop_front();
			actionHandler->onWorkerBack(incWorker.arriveMinute, incWorker.resource);
		}

		/// @brief Check if after unloading the incoming workers we can send off a client with desired products
		List<MyClient>::Node* beg = fWaitingClients.begin();
		while (beg != nullptr)
		{
			if (getClientDepartureTime(beg->fData) <= fMinute || (fBananas >= beg->fData.banana && fSchweppes >= beg->fData.schweppes))
				serveDepartingWaitingClient(beg);
			else
				beg = beg->fpNext;
		}
	}

	/// @brief Checks if there are requests to be executed before or during the
	/// given minute
	/// @param minute The minute to check whether we have events before or during
	/// it
	/// @return True if there are events happening before or during this minute, false otherwise
	bool thereIsActionForThisMinute(int minute)
	{
		if (!fClients.empty() && fClients.top().arriveMinute <= minute) {
			return true;
		}

		if (!fWorkers.empty() && fWorkers.front().arriveMinute <= minute) {
			return true;
		}

		List<MyClient>::Node* priorityClient = findHighestPriorityWaitingClient();
		if (priorityClient && getClientDepartureTime(priorityClient->fData) <= minute) {
			return true;
		}

		return false;
	}

	/// @brief Basic data members
private:
	int fWorkersCount = 0;
	int fBananas = 0;
	int fSchweppes = 0;
	int fMinute = 0;

	/// @brief Data members for checking how many resources are coming
	int fIncomingBananas = 0;
	int fIncomingSchweppes = 0;

	/// @brief Containers
private:
	LinkedQueue<MyClient> fClients;
	List<MyClient> fWaitingClients;
	List<MyWorker> fWorkers;

private:
	/// @brief Static members
	static const int RESTOCK_AMOUNT = 100;
	static const int RESTOCK_TIME = 60;
};
