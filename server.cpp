#include<iostream>
#include<chrono>
#include <thread>

using namespace std;

int main()
{
	int x = 10;

	while(x)
	{
		cout << "Herbert" << endl;
		this_thread::sleep_for(chrono::milliseconds(750));
		--x;
	}
	cout << "WTF IS THIS SHIT?!" << endl;
}
