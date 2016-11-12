#include "cplexcpp.h"

using namespace std;

IloIntArray objScalars;
IloInt IndiceI, IndiceJ, IndiceK;
IloInt Dmin, Dmax, Wmin, Wmax;
int Tjk[] = {0, 0, 0, 0};

void CplexCpp :: runCplex() {
  cout << "CplexCpp is running..." << endl;
  IloEnv env;

  try {
    IloModel model(env);
    IloCplex cplex(env);

    IloObjective obj;
    IloNumVarArray var(env);
    IloRangeArray rng(env);

    populate(model, var, rng);

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
    IndiceI = workers;
    IndiceJ = shifts;
    IndiceK = days;

    cout << "[ workers shifts days ]  = [ " << IndiceI << ", " << IndiceJ << " " << IndiceK << " ]" << endl;
}

void CplexCpp :: define_WeekBounds (int min, int max) {
    Wmin = min;
    Wmax = max;

    cout << "[ Wmin Wmax ] = [ " << Wmin << " " << Wmax << "]" << endl;
}

void CplexCpp :: define_DayBounds (int min, int max) {
    Dmin = min;
    Dmax = max;

    cout << "[ Dmin Dmax ] = [ " << Dmin << " " << Dmax << " ]" << endl;
}

void CplexCpp :: define_Coefficient () {
    //(Pijk * Aijk) + (Aijk - 1)

}

void CplexCpp :: define_TimeSections(int array []) {
    cout << "[ T11 T21 T12 T22 ] = [ ";
    int length = get_JK();
    
    for (int i = 0; i < length; i++) {
        
        Tjk[i] = array[i];
        
        cout << array[i] << " ";
    }
    
    cout << "]" << endl;
}

int CplexCpp :: get_IJ() {
    return IndiceI * IndiceJ;
}

int CplexCpp :: get_JK() {
    return IndiceJ * IndiceK;
}

int CplexCpp :: get_IK() {
    return IndiceI * IndiceK;
}



// ============================================================================================================
/*
void Employee :: unwrap_Availability () {
    availability avail_size = IndiceI * IndiceJ;
    availability A[avail_size];
    
}

void Employee :: unwrap_Preference () {
    preference pref_size = IndiceI * IndiceJ;
    preference P[pref_size];

}
*/