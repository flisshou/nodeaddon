#include <iostream>
#include <string>
#include <vector>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

class CplexCpp {
public:

  // bool solution[];
  // int solution[];


  //CplexCpp();
  void define_DataSize(int, int, int);
  void define_WeekBounds(int, int);
  void define_DayBounds(int, int);

  void define_TimeSections(int []);
  void define_BaseAmount(int []);
  IloIntArray define_Coefficient(IloEnv);

  int get_IJ();
  int get_JK();
  int get_IK();

  void runCplex();

};

class Employee {
public:

  void unwrap_Availability(int []);
  void unwrap_Preference(int []);

  int get_IJK();


  void print_Duration(int []);
  void print_Preference(int []);
};
