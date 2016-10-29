#include "cplexcpp.h"

using namespace std;

void CplexCpp :: runCplex() {
  cout << "CplexCpp is running..." << endl;
  IloEnv env;

  try {
    IloModel model(env);
    IloCplex cplex(env);

    IloObjective obj;
    IloNumVarArray var(env);
    IloRangeArray rng(env);

    populate(model,var, rng);

    cplex.extract(model);

    if (cplex.solve()) {
      env.out() << "Solution Status ::" << cplex.getStatus()   << endl;
      env.out() << "Solution Value  ::" << cplex.getObjValue() << endl;

      IloNumArray values(env);

      cplex.getValues(values, var);
      env.out() << "Values        = " << values << endl;

      cplex.getSlacks(values, rng);
      env.out() << "Slacks        = " << values << endl;

      cplex.getDuals(values, rng);
      env.out() << "Duals         = " << values << endl;

      cplex.getReducedCosts(values, var);
      env.out() << "Reduced Costs = " << values << endl;
    }
  } catch (IloException& e) {
    cerr << "Concert Excpetion Caught: " << e << endl;
  } catch (...) {
    cerr << "Unknown Exception Caught: " << endl;
  }

  env.end();
}

void CplexCpp :: define_DataSize (int workers, int shifts, int days) {
  this->indiceI = workers;
  this->indiceJ = shifts;
  this->indiceK = days;

  cout << "[i, j, k] = [" << this->indiceI << ", " << this->indiceJ << ", " << this->indiceK << "]" << endl;
  cout << this->indiceI + this->indiceJ + this->indiceK << endl;
}

void CplexCpp :: define_WeekBounds (int min, int max) {
  this->Wmin = min;
  this->Wmax = max;
}

void CplexCpp :: define_DayBounds (int min, int max) {
  this->Dmin = min;
  this->Dmax = max;
}
