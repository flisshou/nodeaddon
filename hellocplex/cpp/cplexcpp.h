#include <iostream>
#include <string>
#include <vector>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

class CplexCpp {
public:

  //CplexCpp();
  void define_DataSize(int, int, int);
  void define_WeekBounds(int, int);
  void define_DayBounds(int, int);

  void define_TimeSections(int []);
  void define_BaseAmount(int []);
  void define_Coefficient();

  int get_IJ();
  int get_JK();
  int get_IK();

  void runCplex();
  void populate(IloModel, IloNumVarArray, IloRangeArray);
};

class Employee {
public:

  void unwrap_Availability(int []);
  void unwrap_Preference(int []);

  int get_IJK();


  void print_Duration(int []);
  void print_Preference(int []);
};
