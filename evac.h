// Author: Sean Davis
#ifndef evacH
#define evacH

#include "vec.h"
#include "StackAr.h"
#include "QueueAr.h"
#include "EvacRunner.h"


class MyRoad
{
public:
  int ID;
  int destinationCityID;
  int sourceCityID;
  int peoplePerHour;
  int peopleThisHour;

  MyRoad() : ID(-1), destinationCityID(-1), sourceCityID(-1), peoplePerHour(-1), peopleThisHour(0) {}
};

class MyCity
{

public:
  int ID;
  int population;
  int evacuees;
  int level;
  bool isViable;

  MyRoad* roads;
  int roadCount;
  int* outRoads;

  MyCity() : level(-1), evacuees(0), isViable(true) {}

};

class Evac
{
private:
  int clock;

public:
  MyCity** cities;
  MyRoad* allRoads;
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
  int DFS(MyCity* origin, int originID, int given, EvacRoute* evacRoutes, int& routeCount);

  // for qsort()
  static int compareRoads (const void * a, const void * b);
  static int compareCity (const void * a, const void * b);
}; // class Evac

#endif