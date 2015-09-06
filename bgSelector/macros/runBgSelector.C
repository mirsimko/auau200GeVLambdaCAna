#include "bgSelector/bgSelector.h"
#include <iostream>
using namespace std;

void runBgSelector()
{
  cout << "initializing bgSelector ..." << endl;
  bgSelector T;
  cout << "Looping over events ..." << endl;
  T.Loop();
  cout << "Writing into file ..." << endl;
  T.Write();
}
