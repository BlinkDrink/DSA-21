// 
// СУ "Св. Климент Охридски"
// Факултет по математика и информатика
// 
// Курсове Структури от данни и програмиране
//         Структури от данни
// Зимен семестър 2021/22 г.
// 
// Практически изпит
// 
// Име: Петко Иванов Петков
// ФН: 45811
// Специалност: Информатика
// Курс: 2
// Административна група: 2
// Ден, в който се явявате на изпита: 31/1/2022
// Начален час на изпита: (тук попълнете часа за вашата група) 9:00
// Кой компилатор използвате: (тук посочете едно от Visual C++, GCC или Clang) Visual C++
//

// (Можете да изтриете този коментар след като го прочетете)
// Редът по-долу е специфичен за Visual C++.
// Ако използвате друг компилатор, можете да го изтриете.
// Тъй като strlen, strcmp и т.н. са дефинирани като deprecated,
// имате две възможности:
//
// * да използвате безопасните версии на тези функции
//   (strlen_s, strcmp_s и т.н.). В този случай можете да
//   изтриете дадената по-долу #define директива.
//
// * да дефинирате _CRT_SECURE_NO_WARNINGS преди да
//   включите съответните header файлове от стандартната
//   библиотека.
//
#define _CRT_SECURE_NO_WARNINGS 

#include <cassert>
#include <iostream>

using std::cout;
using std::cin;

enum class Drinks
{
	beer, vodka, whiskey, tequila, cola, rum, jin, menta
};

struct BarSlot
{
	bool isTaken;
	bool drinksPerSlot[8];
	BarSlot* next;

	BarSlot(bool isTaken, bool drinks[8], BarSlot* next = nullptr) : isTaken(isTaken), next(next)
	{
		for (size_t i = 0; i < 8; i++)
			drinksPerSlot[i] = drinks[i];
	}
};

struct Client
{
	bool isSeated;
	bool desiredDrinks[8];
	Client* prev;
	Client* next;

	Client(bool isSeated, bool drinks[8], Client* prev = nullptr, Client* next = nullptr) : isSeated(isSeated), prev(prev), next(next)
	{
		for (size_t i = 0; i < 8; i++)
			desiredDrinks[i] = drinks[i];
	}
};

bool hasEnoughDrinksForClient(bool barSlotDrinks[], bool clientDrinks[])
{
	for (size_t i = 0; i < 8; i++)
		if (barSlotDrinks[i] != clientDrinks[i])
			return false;

	return true;
}

bool checkCurrentPosition(BarSlot* startingBar, Client* clients)
{
	const size_t maxDistanceBetweenGrp = 5;
	BarSlot* tmp = startingBar;
	Client* tmpCl = clients;
	size_t slotCount = 0, currDistance = 0;
	bool canBeSeated = true;
	if (!startingBar->isTaken && currDistance <= maxDistanceBetweenGrp && hasEnoughDrinksForClient(startingBar->drinksPerSlot, clients->desiredDrinks))
	{
		startingBar = startingBar->next;
		clients = clients->next;
		currDistance = 1;
	}
	else
	{
		startingBar = startingBar->next;
		currDistance++;
	}

	if (clients != nullptr) {
		clients = tmpCl;
		startingBar = tmp;
		return false;
	}

	clients = tmpCl;
	startingBar = tmp;
	return true;
}

bool place(BarSlot* bar, Client* clients)
{
	const size_t maxDistanceBetweenGrp = 5;
	size_t slotCount = 0, currDistance = 0;
	BarSlot* startingSlot = bar;
	Client* startingClient = clients;

	bool isAvailable = false;
	while (startingSlot)
	{
		isAvailable = checkCurrentPosition(startingSlot, startingClient);

		if (isAvailable)
		{
			if (!startingSlot->isTaken && currDistance <= maxDistanceBetweenGrp && hasEnoughDrinksForClient(startingSlot->drinksPerSlot, startingClient->desiredDrinks))
			{
				startingSlot->isTaken = true;
				startingClient->isSeated = true;

				startingSlot = startingSlot->next;
				startingClient = startingClient->next;
				currDistance = 1;
			}
			else
			{
				startingSlot = startingSlot->next;
				currDistance++;
			}
		}
	}

	if (isAvailable)
		return true;

	return false;
}

void clear(BarSlot*& bar)
{
	if (!bar)
		return;

	clear(bar->next);
	delete bar;
	bar = nullptr;
}

void clear(Client*& cl)
{
	if (!cl)
		return;

	clear(cl->next);
	delete cl;
	cl = nullptr;
}
//
//int main()
//{
//	size_t barSlots, groups;
//
//	BarSlot* bar = nullptr, * prevBar = nullptr;
//	BarSlot* startBar = bar;
//	Client* clients = nullptr, * prevClients = nullptr;
//	Client* startClients = clients;
//
//	cout << "Enter bar slots: ";
//	cin >> barSlots;
//	cout << "Enter number of groups of clients: ";
//	cin >> groups;
//
//	cout << "For every drink(in this order) type 1 if available, 0 if not." << "\n";
//	for (size_t i = 0; i < barSlots; i++)
//	{
//		bool drinks[8];
//
//		cout << "(beer, vodka, whiskey, tequila, cola, rum, jin, menta):";
//		for (size_t j = 0; j < 8; j++) {
//			cin >> drinks[j];
//		}
//
//		bar = new BarSlot(false, drinks);
//		if (i == 0) {
//			startBar = bar;
//		}
//		else
//		{
//			prevBar->next = bar;
//		}
//
//		prevBar = bar;
//		bar = bar->next;
//	}
//
//	size_t numClients;
//	cout << "For every drink(in this order) type 1 if desired, 0 if not" << "\n";
//	for (size_t i = 0; i < groups; i++)
//	{
//		cout << "Enter number of clients of this group: ";
//		cin >> numClients;
//		for (size_t j = 0; j < numClients; j++)
//		{
//			bool drinks[8];
//			cout << "(beer, vodka, whiskey, tequila, cola, rum, jin, menta):";
//			for (size_t k = 0; k < 8; k++)
//				cin >> drinks[k];
//
//			clients = new Client(false, drinks, prevClients);
//			if (j == 0)
//			{
//				startClients = clients;
//			}
//			else
//			{
//				prevClients->next = clients;
//			}
//
//			prevClients = clients;
//			clients = clients->next;
//		}
//	}
//
//	cout << place(startBar, startClients);
//
//
//
//	return 0;
//}