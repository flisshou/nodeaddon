#include <iostream>
#include <string>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

class CplexCpp {
public:

  //CplexCpp();
  void define_DataSize(int, int, int);
  void define_WeekBounds(int, int);
  void define_DayBounds(int, int);
  void define_TimeSections(int []);
  void define_Coefficient();

  int get_IJ();
  int get_JK();
  int get_IK();

  void runCplex();
  void populate(IloModel, IloNumVarArray, IloRangeArray);

};
//
// class Employee {
// public:
//   typedef int availability;
//   typedef int preference;
//   typedef int seniority;
//
//   void unwrap_Availability ();
//   void unwrap_Preference();
// };
