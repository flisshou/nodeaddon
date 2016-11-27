#include "cplexcpp.h"

using namespace std;


IloInt IndiceI, IndiceJ, IndiceK;//4 2 2
IloInt Dmin, Dmax, Wmin, Wmax;
IloInt lowerBound = 0;
IloInt upperBound = 1;
int Tjk[] = {0, 0, 0, 0};
int Bjk[] = {0, 0, 0, 0};
vector<int> Aijk;
vector<int> Pijk;
void populate(IloModel, IloNumVarArray, IloRangeArray);
void solveSchedule(IloCplex, IloNumVarArray, IloRangeArray);




void CplexCpp :: runCplex() {
    cout << "CplexCpp is running..." << endl;
    IloEnv env;
    Employee emp;
    
    try {
        
        IloModel model(env);
        
        IloObjective obj;
        IloNumVarArray var(env);
        IloRangeArray rng(env);
        
        IloInt varNumber = emp.get_IJK();
        IloIntArray objScalars = define_Coefficient(env);
        var = IloNumVarArray(env, varNumber, lowerBound, upperBound, ILOINT);
        model.add(IloMaximize(env, IloScalProd(objScalars, var)));
        
        
        populate(model, var, rng);
        
        IloCplex cplex(model);
        cplex.extract(model);
        
        solveSchedule(cplex, var, rng);
        
        model.end();
        
    } catch (IloException& e) {
        cerr << "Concert Exception Caught: " << e << endl;
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

IloIntArray CplexCpp :: define_Coefficient (IloEnv env) {
    //(Pijk * Aijk) + (Aijk - 1)
    IloInt length = Aijk.size();
    IloIntArray scal(env);
    
    for (IloInt i = 0 ; i < length; i++) {
        scal.add((Aijk[i]*Pijk[i]) + (Aijk[i] - 1));
    }
    
    env.out() << "IloCPLEX scal is:  " << scal << endl;
    return scal;
}





//=======================EMPLOYEE EVENT========================





void Employee :: unwrap_Availability(int inputArray []) {
    //durationArray = [13,   13,    7,    7,         13,    6,       6,    6]
    //------->      [1, 1, 1, 1,    1, 0, 1, 0,    1, 1, 0, 1,    0, 1, 0, 1]
    //              [ 1 1 1 1       1 0 1 0        1 1 0 1          0 1 0 1 ]
    CplexCpp cpp;

    int inputLength  = cpp.get_IK();
    int outputLength = get_IJK();
    
    Aijk.resize(outputLength);
    
    for (int i = 0; i < inputLength; i++) {
        
        int JK = cpp.get_JK();
        int TjkIndex = (IndiceJ * i) % JK;
        int outputIndex = IndiceJ * i;
        
        if (inputArray[i] == Tjk[TjkIndex]) {
            Aijk[outputIndex] = 1;
        } else if (inputArray[i] == Tjk[TjkIndex + 1]) {
            Aijk[outputIndex + 1] = 1;
        } else if (inputArray[i] == Tjk[TjkIndex] + Tjk[TjkIndex + 1]) {
            Aijk[outputIndex] = 1;
            Aijk[outputIndex + 1] = 1;
        } else {
            cout << "Something is wrong..." << endl;
            cout << "Returning....." << endl;
            return;
        }
    }
    
    cout << "Aijk = < ";
    for (int i = 0; i < outputLength; i++) {
        cout << Aijk[i] << " ";
    }
    cout << ">" << endl;
//    int outputArray[outputLength];
    //    for (int i = 0; i < inputLength; i++) {
    //
    //        int outputIndex = IndiceJ * i;
    //        int TjkIndex = (IndiceJ * i) % JK;
    //
    //        if (inputArray[i] == Tjk[TjkIndex]) {
    //
    //            outputArray[outputIndex]     = 1;
    //            outputArray[outputIndex + 1] = 0;
    //
    //        } else if (inputArray[i] == Tjk[TjkIndex + 1]) {
    //
    //            outputArray[outputIndex]     = 0;
    //            outputArray[outputIndex + 1] = 1;
    //
    //        } else if (inputArray[i] == Tjk[TjkIndex] + Tjk[TjkIndex + 1]) {
    //
    //            outputArray[outputIndex]     = 1;
    //            outputArray[outputIndex + 1] = 1;
    //
    //        } else {
    //            cout << "Error..." << endl;
    //            cout << "Returning..." << endl;
    //            return;
    //        }
    //    }}
}

void Employee :: unwrap_Preference(int inputArray []) {
    CplexCpp cpp;
    
    int inputLength  = cpp.get_IK();
    int outputLength = get_IJK();
    
    Pijk.resize(outputLength);
    
    for (int i = 0; i < inputLength; i++) {
        
        int APIndex = 2 * i;
        
        if (Aijk[APIndex] && Aijk[APIndex + 1]) {
            Pijk[APIndex] = inputArray[i];
            Pijk[APIndex + 1] = inputArray[i];
        } else if (Aijk[APIndex] && Aijk[APIndex + 1] == 0) {
            Pijk[APIndex] = inputArray[i];
        } else if (Aijk[APIndex] == 0 && Aijk[APIndex + 1]) {
            Pijk[APIndex + 1] = inputArray[i];
        } else {
            cout << "Wrong Pairing for Aijk and Pijk..." << endl;
            cout << "Returning....." << endl;
            return;
        }
    }
    
    cout << "Pijk = < " ;
    for (int i = 0; i < outputLength; i++) {
        cout << Pijk[i] << " ";
    }
    cout << ">" << endl;
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

void Employee :: print_Preference(int preferenceArray []) {
    
    CplexCpp cpp;
    
    cout << "[ Pik ]--------" << endl;
    cout << "[ P11 P12 P21 P22 P31 P32 P41 P42 ] = [ ";
    int length = cpp.get_IK();
    
    for (int i = 0; i < length; i++) {
        cout << preferenceArray[i] << " ";
    }
    
    cout << "]" << endl;
}

int Employee :: get_IJK () {
    return IndiceI * IndiceJ * IndiceK;
}





//==========================MODEL==============================





void populate(IloModel model, IloNumVarArray Yijk, IloRangeArray rng) {
    Employee emp;
    CplexCpp cpp;

    IloEnv env = model.getEnv();

    IloInt JK        = cpp.get_JK();
    IloInt varNumber = emp.get_IJK();

//    IloIntArray objScalars = cpp.define_Coefficient(env);
    
//    Yijk = IloIntVarArray(env, varNumber, 0, 1);
//    IloNumVarArray Xik = IloNumVarArray(env, varNumber/2, lowerBound, upperBound, ILOINT);
    
    //Objective Function
//    IloObjective obj = IloAdd(model, IloMaximize(env, IloScalProd(objScalars, Yijk)));
    
//    model.add(IloMaximize(env, IloScalProd(objScalars, Yijk)));
    
    //Constraint 1-1:
    IloExpr expr1(env);
    for (IloInt i = 0; i < varNumber; i += JK) {
        expr1.setLinearCoef(Yijk[i], 1);
    }
    IloRange rng1 = IloRange(env, Bjk[0], expr1, Bjk[0]);
    rng.IloExtractableArray::add(rng1);
    expr1.end();
    
    //Constraint 1-2:
    IloExpr expr2(env);
    for (IloInt i = 1; i < varNumber; i += 4) {
        expr2.setLinearCoef(Yijk[i], 1);
    }
    IloRange rng2 = IloRange(env, Bjk[1], expr2, Bjk[1]);
    rng.IloExtractableArray::add(rng2);
    expr2.end();
    
    //Constraint 1-3:
    IloExpr expr3(env);
    for (IloInt i = 2; i < varNumber; i += 4) {
        expr3.setLinearCoef(Yijk[i], 1);
    }
    IloRange rng3 = IloRange(env, Bjk[2], expr3, Bjk[2]);
    rng.IloExtractableArray::add(rng3);
    expr3.end();
    
    //Constraint 1-4:
    IloExpr expr4(env);
    for (IloInt i = 3; i < varNumber; i += 4) {
        expr4.setLinearCoef(Yijk[i], 1);
    }
    IloRange rng4 = IloRange(env, Bjk[3], expr4, Bjk[3]);
    rng.IloExtractableArray::add(rng4);
    expr4.end();
    
    //Constraint 2:
    for (IloInt i = 0; i < varNumber; i += IndiceJ) {
        IloInt TjkIndex = i % 4;
//        IloInt min = Dmin * Xik[i/2];
        rng.add(IloRange(env, Dmin, Tjk[TjkIndex] * Yijk[i] + Tjk[TjkIndex+1]* Yijk[i+1], Dmax));
    }
    
    //Constrain 3:
    for (IloInt i = 0; i < varNumber; i += JK) {
        rng.add(IloRange(env, Wmin, Tjk[0] * Yijk[i] + Tjk[1] * Yijk[i+1] + Tjk[2] * Yijk[i+2] + Tjk[3] * Yijk[i+3], Wmax));
    }
    
    //Merge Constraints into CPLEX Extractable Model
    model.add(rng);
    
    //Constraint 4:
//    IloAnd cons1(env);
//    for (IloInt i = 0; i < varNumber; i += IndiceK) {
//        cons1.add(Yijk[i]   <= Xik[i/2]);
//        cons1.add(Yijk[i+1] <= Xik[i/2]);
//        model.add(cons1);
//    }
    
    env.out() << "THE MODEL:      " << endl << model << endl;
}

void solveSchedule(IloCplex cplex, IloNumVarArray var, IloRangeArray rng){
    
    IloEnv env = var.getEnv();
//    CplexCpp cpp;
//    Employee emp;
    
    if (cplex.solve()) {
        
        env.out() << "Solution Status :: " << cplex.getStatus()   << endl;
        env.out() << "Solution Value  :: " << cplex.getObjValue() << endl;
        
        
        IloNumArray values(env);
//        IloInt nvar = var.getSize();
        
        cplex.getValues(values, var);
        env.out() << "Values: " << values << endl;
//        
//        for (int i = 0; i < nvar; i++) {
//            if (values[i] == 1) {
//                cpp.solution[i] = true;
//            } else {
//                cpp.solution[i] = false;
//            }
//        
//        }
        values.end();
        
//        cout << "cpp.solution in cplexcpp.cc ----> [ ";
//        for (int i = 0; i < emp.get_IJK(); i++) {
//            cout << cpp.solution[i] << " ";
//        }
//        cout << "]" << endl << endl;
    }
    env.out() << endl;
}




//=====================RETURN SOLUTION=========================

//Solution
//       [                   Day 1  Day 2
//Emp1    1 0 0 0    --->    09~16    0
//Emp2    0 0 1 0    --->      0    09~16
//Emp3    1 0 0 1    --->    09~16  16~22
//Emp4    0 1 1 0    --->    16~22  09~16    => Should avoid this kind of situation
//       ]








































