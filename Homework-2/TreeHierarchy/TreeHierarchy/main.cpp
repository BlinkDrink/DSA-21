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
		Hierarchy h("Uspeshnia-Gosho\nUspeshnia-Misho\nUspeshnia-Slavi\nGosho-Pesho\nGosho-Dancho\nSlavi-Slav1\nSlavi-Slav2\nPesho-Alex\nDancho-Boris\nDancho-Kamen\nSlav1-Mecho\nMecho-Q12Adl"
		);

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

		cout << h.print();
		h.incorporate();
		cout << h.print();

		cout << h.getSalary("Slav1");
	}
	catch (const std::exception& e)
	{
		cout << e.what() << std::endl;
	}
	return 0;
}