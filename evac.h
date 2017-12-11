// Author: Sean Davis
#ifndef evacH
#define evacH

#include "vec.h"
#include "StackAr.h"
#include "QueueAr.h"
#include "MyCity.h"
#include "EvacRunner.h"

class Evac
{
private:
  int clock;

public:
  MyCity** cities;
  bool* wasVisited;
  bool* isEvacCity;
  int* roadCurrUsing;
  int track;

  Queue<MyCity> bfs;
  int numCities;
  int totalRoads;
  int numPeopleLeft;

  Evac(City *cities, int numCities, int numRoads);
  void evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes, int &routeCount); // student sets evacRoutes and routeCount
  int DFS(MyCity* origin, int originID, int waiting, EvacRoute* evacRoutes, int& routeCount);

  // for qsort()
  static int compareRoads (const void * a, const void * b);
  static int compareCity (const void * a, const void * b);
}; // class Evac

#endif