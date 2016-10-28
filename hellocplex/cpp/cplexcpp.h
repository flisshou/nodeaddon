#include <iostream>
#include <string>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

class CplexCpp {
public:
  IloInt Wmin, Wmax, Dmin, Dmax;
  IloInt indiceI, indiceJ, indiceK;
  IloIntArray objScalars;

  void define_DataSize(int, int, int);
  void define_WeekBounds(int, int);
  void define_DayBounds(int, int);

  void runCplex();
  void populate(IloModel, IloNumVarArray, IloRangeArray);

};
