//
// Created by Daryl Nakamoto on 12/5/17.
//

#ifndef P5_MYROAD_H
#define P5_MYROAD_H


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


#endif //P5_MYROAD_H
