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
           clock(1), bfs(numCitie), numCities(numCitie), totalRoads(numRoads * 2), numPeopleLeft(0)
{
  numCities = numCitie;
  cities = new MyCity*[numCitie]; // array of cities. index is ID of city
  wasVisited = new bool[numCitie]; // visited bool array. index is city ID
  isEvacCity = new bool[numCitie];
  roadCurrUsing = new int[totalRoads]; // keep track of current road PPH. index is road ID

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

    for(int j = 0; j < roadNum; ++j)
    {
      cities[cityNum]->roads[j].ID = citie[i].roads[j].ID;
      cities[cityNum]->roads[j].peoplePerHour = citie[i].roads[j].peoplePerHour;
      cities[cityNum]->roads[j].destinationCityID = citie[i].roads[j].destinationCityID;
      cities[cityNum]->roads[j].sourceCityID = cityNum;
    }

    qsort(cities[cityNum]->roads, roadNum, sizeof(MyRoad), &Evac::compareRoads); // sort every road in ascending order
  }
} // Evac()

void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute* evacRoutes, int& routeCount)
{
  MyCity* bfsCurrent = NULL;
  MyCity* adjacent = NULL;
  MyCity* destination;
  MyRoad currentRoad;

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
      // look at every adjacent city and send people
      for(int j = 0; j < evacCities[i]->roadCount; ++j)
      {
        // get destination information
        currentRoad = evacCities[i]->roads[j];
        destination = cities[currentRoad.destinationCityID];

        // get population of current evac city
        give = evacCities[i]->population;

        // check if the road is limiting flow
        if(currentRoad.peoplePerHour - roadCurrUsing[currentRoad.ID] < give)
          give = currentRoad.peoplePerHour - roadCurrUsing[currentRoad.ID];

        // check if the destination is limiting flow
        if(destination->population - destination->evacuees < give)
          give = destination->population - destination->evacuees;

        // if nothing can be given to the adjacent city, move on
        if(!give)
          continue;

        // get the ACTUAL number of people lead to safety
        numTaken = DFS(destination, evacCities[i]->ID, give, evacRoutes, routeCount);

        if(numTaken > give)
        {
          cout << "ERROR: Too many people sent down a road" << endl;
          exit(0);
        }

        // if people were lead to safety, record the amount for the evac route an subtract from population and numLeft
        if(numTaken)
        {
          evacRoutes[routeCount].time = clock;              //
          evacRoutes[routeCount].numPeople = numTaken;      //  recording route
          evacRoutes[routeCount].roadID = currentRoad.ID;   //
          roadCurrUsing[currentRoad.ID] += numTaken;        //
          ++routeCount;                                     //

          // update evac city and numLeft
          evacCities[i]->population -= numTaken;
          numPeopleLeft -= numTaken;
        }
      }
      // reset visited cities
      memset(wasVisited, false, sizeof(bool) * numCities);
    }
    /**
     * Done: Maximum amount of people sent out of evac zone for each evac city. Reset the roads, bool array, and begin
     *       next hour. Repeat this until everyone is safe.
     */
    // reset roads
    memset(roadCurrUsing, 0, sizeof(int) * totalRoads);

    if(roadCurrUsing[28] >= 339)
      cout << "";

    if(clock == 1)
      cout << "";
    // begin next hour
    ++clock;
  }
  /**
   * Done: Everyone is safe
   */
} // evacuate

// FIX ME!!!! NEED TO STOP!
// BUG: numWaiting is going negative
// communicate how much was ACTUALLY taken from numWaiting (numTaken)
// if destination is source
// check road capacities!
// mark cities as false on the way back up so the same city can use the same path on the way down
int Evac::DFS(MyCity* origin, int originID, int waiting, EvacRoute* evacRoutes, int& routeCount)
{
  int numGiven = waiting;
  int numTaken = 0;
  int totalTaken = 0;

  MyCity* destination;
  MyRoad currentRoad;

  // for every adjacent city
  for(int i = 0; i < origin->roadCount; ++i)
  {
    destination = cities[origin->roads[i].destinationCityID];
    currentRoad = origin->roads[i];

    // if the city was not visited
    if(!wasVisited[originID])
    {
      // mark as true to prevent from coming back
      wasVisited[originID] = true;

      // if the destination level is >= origin, push some people there
      if(destination->level >= origin->level)
      {
        // keep track of how may people to give adjacent city
        int give = numGiven;

        // check if the road is limiting flow
        if(currentRoad.peoplePerHour - roadCurrUsing[currentRoad.ID] < give)
          give = currentRoad.peoplePerHour - roadCurrUsing[currentRoad.ID];

        // check if the destination is limiting flow
        if(destination->population - destination->evacuees < give)
          give = destination->population - destination->evacuees;

        // nothing to give. Move on.
        if(!give)
          continue;

        // get number of people taken
        numTaken = DFS(destination, give, destination->ID, evacRoutes, routeCount);

        // record route if people were taken
        if(numTaken)
        {
          evacRoutes[routeCount].roadID = currentRoad.ID;
          roadCurrUsing[currentRoad.ID] += numTaken;
          evacRoutes[routeCount].numPeople = numTaken;
          evacRoutes[routeCount].time = clock;
          ++routeCount;

          // keep cumulative total of EVERYONE that was taken
          totalTaken += numTaken;

          // keep track of residual in case this needs to be kept
          numGiven -= numTaken;
        }
      }
    }
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

  if(wasVisited[originID])
    wasVisited[originID] = false;

  // return the TOTAL amount of people taken
  return totalTaken;
}