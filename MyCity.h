//
// Created by Daryl Nakamoto on 12/3/17.
//

#ifndef P5_MYCITY_H
#define P5_MYCITY_H

#include "MyRoad.h"
#include "EvacRunner.h"

class MyCity
{

public:
  int ID;
  int population;
  int evacuees;
  int level;

  MyRoad* roads;
  int roadCount;

  MyCity() : level(-1), evacuees(0) {}

};

#endif //P5_MYCITY_H