#include<iostream>
#include<chrono>
#include <thread>

using namespace std;

int main()
{
	int x = 5;
	cout << "\033[2J\033[1;1H";

	while(x)
	{
		cout << "Herbert";
		cout.flush();
		this_thread::sleep_for(chrono::milliseconds(750));
		cout << ".";
		cout.flush();
		this_thread::sleep_for(chrono::milliseconds(750));
		cout << ".";
		cout.flush();
		this_thread::sleep_for(chrono::milliseconds(750));
		cout << ".";
		cout.flush();
		this_thread::sleep_for(chrono::milliseconds(750));
		cout << "\033[2J\033[1;1H";
		--x;
	}
	cout << "WTF IS THIS SHIT?!" << endl;
}
