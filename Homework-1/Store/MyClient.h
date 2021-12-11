#pragma once
#include "../StoreTests/interface.h"

/// @brief Descriptor of Client in the store. This one inherits the Client class given by the interface of the homework.
/// In addition to Client class given in interface.h, here I have added index of the client
struct MyClient : Client
{
	int index;

	MyClient(int arriveTime, int bananaCount, int schweppesCount, int waitTime, int index_of_client)
	{
		this->arriveMinute = arriveTime;
		this->banana = bananaCount;
		this->schweppes = schweppesCount;
		this->maxWaitTime = waitTime;
		this->index = index_of_client;
	}
};