#pragma once
#include "../StoreTests/interface.h"

/// @brief Descriptor of a worker that works at the store
struct MyWorker {
	int arriveMinute; ///< The time at which the worker arrives at the store
	ResourceType resource; ///< The resource the worker is carrying
};