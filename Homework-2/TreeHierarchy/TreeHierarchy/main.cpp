#include<iostream>
#include "interface.hpp"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using std::cout;

int main()
{
	try
	{
		Hierarchy h(
			"Uspeshnia - Gosho \n"
			"Uspeshnia - Misho \n"
			"Gosho     - Pesho \n"
			"Gosho     - Dancho\n"
			"Pesho     - Alex  \n"
			"Dancho    - Boris \n"
			"Dancho    - Kamen \n"
			"Uspeshnia - Slavi \n"
			"Slavi     - Slav1 \n"
			"Slavi     - Slav2 \n"
			"Slav1     - Mecho \n"
			"Mecho     - Q12Adl\n"
		);

		Hierarchy g
		(" Uspeshnia   -  MishoPetrov \n"
			" MishoPetrov -  Misho       \n"
			" MishoPetrov -  Slav        \n");

		/*cout << h.longest_chain() << std::endl;
		cout << "Manager of Misho is " << h.manager("Misho") << "\n";
		cout << h.hire("Q12Adl", "Gosho") << "\n";
		cout << "Manager of Q12Adl is " << h.manager("Q12Adl") << "\n";
		cout << h.print();
		cout << h.num_subordinates("Q12Adl") << "\n";
		cout << h.num_subordinates("Dancho");
		cout << h.hire("Dancho", "Slav2") << "\n";
		cout << "Manager of Dancho is " << h.manager("Dancho") << "\n";
		cout << h.print();*/

		Hierarchy res = h.join(g);
		cout << res.print();

		cout << h.getSalary("Slav1");
	}
	catch (const std::exception& e)
	{
		cout << e.what() << std::endl;
	}
	return 0;
}