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

  void define_TimeSections(int [], int);
  void define_BaseAmount(int []);
  IloIntArray define_Coefficient(IloEnv);

  int get_IJ();
  int get_JK();
  int get_IK();

  void runCplex();
  void solveSchedule(IloCplex, IloIntVarArray, IloRangeArray);

  vector<int> get_Result();
  vector<int> get_Spares();
};

class Employee {
public:

  void unwrap_Availability(int [], int);
  void unwrap_Preference(int [], int);
  void unwrap_Seniority(int []);

  int get_IJK();

  void make_TimeSections(int []);

  void print_Preference(int []);
};
