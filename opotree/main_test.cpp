#include <iostream>

#include "PCLoader.h"
#include "NodeGeometry.h"

using namespace std;

int main (int argc, char *argv[]) {

	cout << "Done!" << endl;

	string datadir = "ripple/";

	PCInfo pcinfo;
	if(PCLoader::loadPCInfo(datadir, pcinfo) != 0) {
		cout << "Error: cannot load pc info" << endl;
		return 1;
	}
	PCLoader::printPCInfo(pcinfo);

	string name = "r";

	NodeGeometry *node = new NodeGeometry(name);
	if(node->loadBinData(pcinfo)) {
		cout << "fail to load node: " << name << endl;
		return 1;
	}
	node->printInfo();

	return 0;
}