#include <cmath>
#include <iostream>

#include "Editor.h"

int main() {
	std::cout << "Welcome to Hackamonth Study Editor!" << std::endl;
	auto app = new Editor();
	app->Run();
}