#include <cstdlib>
#include <cstring>
#include "evac.h"

using namespace std;

int Evac::compareRoads(const void * a, const void * b)
{
  if(((MyRoad*) a)->peoplePerHour > ((MyRoad *) b)->peoplePerHour)
    return -1;

  if(((MyRoad*) a)->peoplePerHour < ((MyRoad *) b)->peoplePerHour)
    return 1;

  return 0;
}

int Evac::compareCity(const void * a, const void * b)
{
  if((*(MyCity**)a)->roadCount > (*(MyCity**)b)->roadCount)
    return -1;

  if((*(MyCity**)a)->roadCount < (*(MyCity**)b)->roadCount)
    return 1;

  return 0;
}

Evac::Evac(City *citie, int numCitie, int numRoads) :
           clock(1), bfs(numCitie), numCities(numCitie), totalRoads(numRoads * 2), numPeopleLeft(0), track(0)
{
  numCities = numCitie;
  cities = new MyCity*[numCitie]; // array of cities. index is ID of city
  wasVisited = new bool[numCitie]; // visited bool array. index is city ID
  isEvacCity = new bool[numCitie];
  roadCurrUsing = new int[totalRoads]; // keep track of current road PPH. index is road ID
  allRoads = new MyRoad[totalRoads];

  // initial setup
  memset(wasVisited, false, sizeof(bool) * numCitie); // set entire array to false
  memset(isEvacCity, false, sizeof(bool) * numCitie); // set entire array to false
  memset(roadCurrUsing, 0, sizeof(int) * totalRoads); // set entire array to 0

  int cityNum = 0; // keep track of city ID for setup
  int roadNum = 0; // keep track of road count for setup

  // populate cities array
  for(int i  = 0; i < numCitie; ++i)
  {
    cityNum = citie[i].ID;

    cities[cityNum] = new MyCity;
    cities[cityNum]->ID = cityNum;
    cities[cityNum]->population = citie[i].population;
    cities[cityNum]->roadCount = roadNum = citie[i].roadCount;
    cities[cityNum]->roads = new MyRoad[roadNum];
    cities[cityNum]->outRoads = new int[roadNum];

    for(int j = 0; j < roadNum; ++j)
    {
      cities[cityNum]->roads[j].ID = citie[i].roads[j].ID;
      cities[cityNum]->roads[j].peoplePerHour = citie[i].roads[j].peoplePerHour;
      cities[cityNum]->roads[j].destinationCityID = citie[i].roads[j].destinationCityID;
      cities[cityNum]->roads[j].sourceCityID = cityNum;

      // testing method to delete roads if a city is no longer viable
      allRoads[cities[cityNum]->roads[j].ID] = cities[cityNum]->roads[j];
      cities[cityNum]->outRoads[j] = cities[cityNum]->roads[j].ID;
    }

    qsort(cities[cityNum]->roads, roadNum, sizeof(MyRoad), &Evac::compareRoads); // sort every road in ascending order
  }
} // Evac()

void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute* evacRoutes, int& routeCount)
{
  MyCity* bfsCurrent = NULL;
  MyCity* adjacent = NULL;
  MyRoad roadUsed;

  routeCount = 0;
  int numWaiting = 0; // number of people waiting to be placed somewhere

  // sort evac cities by road count
  MyCity** evacCities = new MyCity*[numEvacs];
  for(int i = 0; i < numEvacs; ++i)
  {
    evacCities[i] = cities[evacIDs[i]];
    numPeopleLeft += cities[evacIDs[i]]->population; // get number of people to evac
  }
  qsort(evacCities, numEvacs, sizeof(MyCity*), &Evac::compareCity);

  // about to start BFS. Place evac citites in queue and mark as visisted
  for(int i = 0; i < numEvacs; ++i)
  {
    evacCities[i]->level = 0;
    isEvacCity[evacCities[i]->ID] = true; // mark as an evacuee city
    bfs.enqueue(evacCities[i]);
  }

  /**
   * Start: BFS - level everything
   */
  while(!bfs.isEmpty())
  {
    // dequeue city
    bfsCurrent = bfs.dequeue();

    // for every city adjacent
    for(int i = 0; i < bfsCurrent->roadCount; ++i)
    {
      adjacent = cities[bfsCurrent->roads[i].destinationCityID];

      // put in queue if not visited yet
      if(!wasVisited[adjacent->ID])
      {
        wasVisited[adjacent->ID] = true;
        adjacent->level = bfsCurrent->level + 1;
        bfs.enqueue(adjacent);
      }
    }
  }
  // reset bool array
  memset(wasVisited, false, sizeof(bool) * numCities);
  /**
   * Done: BFS - leveled everything
   */

  //
  //
  //

  /**
   * Start: Initiate evacuation
   */
    int give = 0;
    int numTaken = 0;
    int totalTaken = 0;

  while(numPeopleLeft)
  {
    /**
     * Start: Send the maximum amount of people out of the evac zone to a safety zone for the current hour for each
     *        evac city.
     */
    // for every evac city
    for(int i = 0; i < numEvacs; ++i)
    {
      // mark evac city as visited before starting DFS
      wasVisited[evacCities[i]->ID] = true;

      // get population of current evac city
      give = evacCities[i]->population;

      // look at every adjacent city and send people. Check if the evac city is empty
      for(int j = 0; give && j < evacCities[i]->roadCount; ++j)
      {
        // get adjacent city and road to get there
        roadUsed = evacCities[i]->roads[j];
        adjacent = cities[roadUsed.destinationCityID];

        // check if the road is limiting flow
        if(roadUsed.peoplePerHour - roadCurrUsing[roadUsed.ID] < give)
          give = roadUsed.peoplePerHour - roadCurrUsing[roadUsed.ID];

        // if the road is full, check next road
        if(!give)
          continue;

        // get the ACTUAL number of people lead to safety
        numTaken = DFS(adjacent, adjacent->ID, give, evacRoutes, routeCount);

        // if people were lead to safety,
        // record the amount for the evac route and subtract from population and numPeopleLeft
        if(numTaken)
        {
          evacRoutes[routeCount].time = clock;              //
          evacRoutes[routeCount].numPeople = numTaken;      //  recording route
          evacRoutes[routeCount].roadID = roadUsed.ID;   //
          roadCurrUsing[roadUsed.ID] += numTaken;        //
          ++routeCount;                                     //

          // update evac city and give
          give = evacCities[i]->population -= numTaken;

          // update numPeopleLeft
          numPeopleLeft -= numTaken;

          // record totalTaken during this hour.
          // to check if going through this city will produce a viable path
          totalTaken += numTaken;
        }
      }

      // if the the evac city had a viable path down it, make it available
      //if(totalTaken)
        //wasVisited[evacCities[i]->ID] = false;

      memset(wasVisited, false, sizeof(bool) * numCities);

      // reset totalTaken for next city
      totalTaken = 0;
    } // evacuate people for this hour
    /**
     * Done: Maximum amount of people sent out of evac zone for each evac city. Reset the roads, bool array, and begin
     *       next hour. Repeat this until everyone is safe.
     */
    memset(roadCurrUsing, 0, sizeof(int) * totalRoads);   // reset roads and visited
    memset(wasVisited, false, sizeof(bool) * numCities);  //


    cout << numPeopleLeft << endl;

    // begin next hour
    ++clock;
  }
  /**
   * Done: Everyone is safe
   */
} // evacuate

// BUG: Infinite loop occurring. I suspect the wasVisited[] is not set correctly on the way back up
// communicate how much was ACTUALLY taken from numWaiting (numTaken)
// Do I need to check if the destination is the CALLING evac city?
// mark cities as false on the way back up so the same city can use the same path on the way down
int Evac::DFS(MyCity* origin, int originID, int given, EvacRoute* evacRoutes, int& routeCount)
{
  int numGiven = given;
  int numTaken = 0;
  int totalTaken = 0;

  MyCity* adjacent;
  MyRoad roadUsed;

  // if the city was visited, don't go here -> return nothing
  if(wasVisited[originID])
    return 0;

  // for every adjacent city
  for(int i = 0; i < origin->roadCount; ++i)
  {
    // mark current city as visited
    wasVisited[originID] = true;

    // get the adjacent city and the road which leads to it
    roadUsed = origin->roads[i];
    adjacent = cities[roadUsed.destinationCityID];

    // keep track of how many people to give adjacent city
    int give = numGiven;

    // City used up everything given to it. Leave
    if(!give)
      break;

    // if the adjacent level is >= origin, send some people there
   // if(adjacent->level >= origin->level)
    //{
      // check if the road is limiting flow
      if(roadUsed.peoplePerHour - roadCurrUsing[roadUsed.ID] < give)
        give = roadUsed.peoplePerHour - roadCurrUsing[roadUsed.ID];

      // if the road is full, check next road
      if(!give)
        continue;

      // get number of people taken
      numTaken = DFS(adjacent, adjacent->ID, give, evacRoutes, routeCount);

      // record route if people were taken
      if(numTaken)
      {
        evacRoutes[routeCount].roadID = roadUsed.ID;
        roadCurrUsing[roadUsed.ID] += numTaken;
        evacRoutes[routeCount].numPeople = numTaken;
        evacRoutes[routeCount].time = clock;
        ++routeCount;

        // keep cumulative total of EVERYONE that was taken
        totalTaken += numTaken;

        // keep track of residual in case this needs to be kept
        numGiven -= numTaken;
      }
    //}
  }

  // if the current city is not an evac city AND there are people still waiting here
  if(!isEvacCity[originID] && numGiven)
  {
    int space = origin->population - origin->evacuees;

    // check if the current city has limited space
    if(space < numGiven)
      numGiven = space;

    cities[originID]->evacuees += numGiven;
    totalTaken += numGiven;
  }

  if(totalTaken)
    wasVisited[originID] = false;

  // return the TOTAL amount of people taken
  return totalTaken;
}