#include "cplexcpp.h"

using namespace std;

IloIntArray objScalars;
IloInt IndiceI, IndiceJ, IndiceK;//4 2 2
IloInt Dmin, Dmax, Wmin, Wmax;
int Tjk[] = {0, 0, 0, 0};
int Bjk[] = {0, 0, 0, 0};


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

    cout << "[ workers shifts days ]  = [ " << IndiceI << " " << IndiceJ << " " << IndiceK << " ]" << endl;
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

void CplexCpp :: define_TimeSections (int timeArray []) {
    cout << "[ T11 T21 T12 T22 ] = [ ";
    int length = get_JK();
    
    for (int i = 0; i < length; i++) {
        
        Tjk[i] = timeArray[i];
        
        cout << timeArray[i] << " ";
    }
    
    cout << "]" << endl;
}

void CplexCpp :: define_BaseAmount (int baseArray []) {
    cout << "[ B11 B21 B12 B22 ] = [ " ;
    int length = get_JK();
    
    for (int i = 0; i < length; i++) {
        Bjk[i] = baseArray[i];
        
        cout << baseArray[i] << " ";
    }
    
    cout << "]" << endl;
    
}

int CplexCpp :: get_IJ () {
    return IndiceI * IndiceJ;
}

int CplexCpp :: get_JK () {
    return IndiceJ * IndiceK;
}

int CplexCpp :: get_IK () {
    return IndiceI * IndiceK;
}



//=======================EMPLOYEE EVENT========================



void Employee :: unwrap_Availability(int inputArray []) {
    //durationArray = [13,   13,    7,    7,         13,    6,       6,    6]
    //------->      [1, 1, 1, 1,    1, 0, 1, 0,    1, 1, 0, 1,    0, 1, 0, 1]
    //              [ 1 1 1 1       1 0 1 0        1 1 0 1          0 1 0 1 ]
    CplexCpp cpp;

    int inputLength  = cpp.get_IK();
    int outputLength = get_IJK();
    int outputArray[outputLength];
    
    int JK = cpp.get_JK();
    
    for (int i = 0; i < inputLength; i++) {
        
        int outputIndex = IndiceJ * i;
        int TjkIndex = (IndiceJ * i) % JK;
        
        if (inputArray[i] == Tjk[TjkIndex]) {
            
            outputArray[outputIndex]     = 1;
            outputArray[outputIndex + 1] = 0;
            
        } else if (inputArray[i] == Tjk[TjkIndex + 1]) {
            
            outputArray[outputIndex]     = 0;
            outputArray[outputIndex + 1] = 1;
            
        } else if (inputArray[i] == Tjk[TjkIndex] + Tjk[TjkIndex + 1]) {
            
            outputArray[outputIndex]     = 1;
            outputArray[outputIndex + 1] = 1;
            
        } else {
            cout << "Error..." << endl;
            cout << "Returning..." << endl;
            return;
        }
    }
    
    cout << "[ A111 A121 A112 A122 A211 A221 A212 A222 A311 A321 A312 A322 A411 A421 A412 A422] = [ " ;
    for (int i = 0; i < outputLength; i++) {
        cout << outputArray[i] << " " ;
    }
    cout << "]" << endl;
    
}

void Employee :: print_Duration (int durationArray []) {
    
    CplexCpp cpp;
    
    cout << "[ Eik ]--------" << endl;
    cout << "[ E11 E12 E21 E22 E31 E32 E41 E42 ] = [ ";
    int length = cpp.get_IK();
    
    for (int i = 0; i < length; i++) {
        cout << durationArray[i] << " ";
    }
    
    cout << "]" << endl;
}


int Employee :: get_IJK () {
    return IndiceI * IndiceJ * IndiceK;
}

//void Employee :: unwrap_Preference(int preference[]) {
//    
//}





























