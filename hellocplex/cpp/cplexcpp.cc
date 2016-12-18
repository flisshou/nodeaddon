#include "cplexcpp.h"

using namespace std;


//=======================DECLARATIONS=========================


IloInt IndiceI, IndiceJ, IndiceK;//4 26 2
IloInt Dmin, Dmax, Wmin, Wmax;
IloInt lowerBound = 0;
IloInt upperBound = 1;
IloInt CTAmin = 4;//Continuous Assigned Time = 4 Hours = 8 HalfAnHour

vector<int> MngTime;
vector<int> Tk;
vector<int> Bjk;
vector<int> Aijk;
vector<int> Pijk;
vector<int> Wi;
vector<int> ResultForV8;
vector<int> SparesForV8;

void populate(IloModel, IloIntVarArray, IloRangeArray);


//========================CPLEX CPP===========================


void CplexCpp :: define_DataSize (int workers, int shifts, int days) {
    IndiceI = workers;
    IndiceJ = shifts;//( EndTime - StartTime ) * 2  ~= ( 2200 - 0900 ) * 2 = 26
    IndiceK = days;

    cout << "[ workers shifts days ]  = [ " << IndiceI << " " << IndiceJ << " " << IndiceK << " ]" << endl;
}

void CplexCpp :: define_WeekBounds (int min, int max) {
    Wmin = min;
    Wmax = max;

    cout << "[ Wmin Wmax ] ( Every 30 Minutes ) = [ " << Wmin << " " << Wmax << "]" << endl;
}

void CplexCpp :: define_DayBounds (int min, int max) {
    Dmin = min;
    Dmax = max;

    cout << "[ Dmin Dmax ] ( Every 30 Minutes ) = [ " << Dmin << " " << Dmax << " ]" << endl;
    
}

void CplexCpp :: define_TimeSections (int timeArray [], int length) {

    int timeArraySize = length;
    Tk.resize(timeArraySize);
    for (int i = 0; i < timeArraySize; i++) {
        Tk[i] = timeArray[i];
    }
    
    cout << "Tjk ( Time Sections by Hour) = [ ";
    for (int i = 0; i < timeArraySize; i++) {
        cout << Tk[i] << " ";
    }
    
    cout << "]" << endl;
}

void CplexCpp :: define_BaseAmount (int baseArray []) {
    // i       =     0   1   2   3   4   5   6   7   8   9  10  11
    //Tjk      = [   2  15   2  15   2  15   2  15   2  15   2  15   ]
    
    // 0- 1  2-16
    //17-18 19-33
    //34-35 36-50
    //51-52 53-57
    
    //Cleaning = [ 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
    //             0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
    //             0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
    //             0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
    //             0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
    //             0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 ]
    int TkSize  = Tk.size();//12
    int BjkSize = get_JK();//102
    Bjk.resize(BjkSize);
    cout << "TkSize = " << TkSize << endl;
    
    int total    = 0;
    int starter  = 0;
    for (int i = 0; i < TkSize; i ++) {
        int duration = Tk[i];
        
        for (int j = starter; j < starter + duration; j++) {
            Bjk[j] = baseArray[i];
            total++;
        }
        starter = total;
    }
    
    cout << "Bjk = [ " ;
    for (int i = 0; i < BjkSize; i++) {
        cout << Bjk[i] << " " ;
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

//    for (IloInt i = 0; i < IndiceI; i++) {
//        for (IloInt j = 0; j < length; j++) {
//            scal.add(Wi[i] * (Aijk[j] * Pijk[j]) + (Aijk[j] - 1));
//        }
//    }
    cout << "Start to make scal" << endl;
    cout << "length = Aijk.size() = " << length << endl;
    //No seniority
    for (IloInt i = 0 ; i < length; i++) {
        scal.add((Aijk[i] * Pijk[i]) + (Aijk[i] - 1));
    }
    
    //No preference and seniority
    //    for (IloInt i = 0; i < length; i++) {
    //        scal.add((Aijk[i] * 2) - 1);
    //    }

    env.out() << "IloCPLEX scal is:  " << scal << endl;
    return scal;
}

void CplexCpp :: runCplex() {
    cout << "CplexCpp is running..." << endl;
    IloEnv env;
    Employee emp;
    
    try {

        IloModel model(env);
        IloCplex cplex(env);
        IloObjective obj;
        IloIntVarArray var(env);
        IloRangeArray rng(env);
        
        //Make Objective Function
        IloInt varNumber = emp.get_IJK();
        IloIntArray objScalars = define_Coefficient(env);
        var = IloIntVarArray(env, varNumber, lowerBound, upperBound);
        
        model.add(IloMaximize(env, IloScalProd(objScalars, var)));
        
        cout << "Before populate line is here...." << endl;
        populate(model, var, rng);
        
        
        cplex.extract(model);
        cplex.exportModel("schedule.lp");
        solveSchedule(cplex, var, rng);
        
        model.end();
        
    } catch (IloException& e) {
        cerr << "Concert Exception Caught: " << e << endl;
    } catch (...) {
        cerr << "Unknown Exception Caught: " << endl;
    }
    
    env.end();
}

void CplexCpp :: solveSchedule(IloCplex cplex, IloIntVarArray var, IloRangeArray rng){
    
    IloEnv env = var.getEnv();
    Employee emp;

    if (cplex.solve()) {
        
        env.out() << "Solution Status :: " << cplex.getStatus()   << endl;
        env.out() << "Solution Value  :: " << cplex.getObjValue() << endl;
        
        IloNumArray values(env);
        
        cplex.getValues(values, var);
        env.out() << "Values got from CPLEX: " << values << endl;
        
        //Set Result
        int on  = 1;
        int off = 0;
        int size = emp.get_IJK();
        ResultForV8.resize(size);
        
        for (int i = 0; i < size; i++) {
            values[i] == 1 ? ResultForV8[i] = on : ResultForV8[i] = off;
        }
    }
    env.out() << endl;
}

vector<int> CplexCpp :: get_Result() {
    return ResultForV8;
}

vector<int> CplexCpp :: get_Spares() {
    
    int length = ResultForV8.size();
    
    SparesForV8.resize(length, 0);
    for (int i = 0; i < length; i++) {
        if (Aijk[i] == 1 && ResultForV8[i] == 0) {
            SparesForV8[i] = 1;
        }
    }
    return SparesForV8;
}

//=======================EMPLOYEE EVENT========================





void Employee :: unwrap_Availability(int inputArray [], int length) {
    
//                      0      2      4      6      8     10     12         14     16     18     20     22     24     26         28     30     32     34     36     38     40
    
    //MngTimeArray = [/12 46  12 46  12 46  12 46  12 46  12 46  12 46/    /12 46  12 46  12 46  12 46  12 46  12 46  12 46/    /12 46  12 46  12 46  12 46  12 46  12 46  12 46/]
    
//                      0     34     68    102    136    170    204        238    272    306    340    374    408    442        476    510    544    578    612    646    680    714
    
    //inputArray   = [ 30 16  30 16  30 16  30 16  30 16  00 00  00 00      12 16  12 16  12 16  00 00  12 16  12 16  12 16      28  8  28  8  28  8  28  8  28  8  28  8  28  8 ]
    //d e c r y p t :   arg0: startTimeHour * 2;   arg1: durationHour * 2

    int outputLength = get_IJK();       //outputLength = 714
    Aijk.resize(outputLength, 0);
    
    for (int i = 0; i < length; i += 2) {
        int startPoint = inputArray[i] - MngTime[i];
        int floater    = (i / 2) * (IndiceJ);
        int duration   = inputArray[i+1];
        cout << "[ startPoint, floater, duration ] = [ " << startPoint << ", " << floater << ", " << duration << " ]" << endl;
        for (int j = startPoint + floater; j < startPoint + duration + floater; j++) {
            Aijk[j] = 1;
        }

    }
    
    cout << "Aijk = [";
    for (int i = 0; i < outputLength; i++) {
        cout << Aijk[i] << " ";
    }
    cout << "]" << endl;

//    
//    int outputLength = get_IJK();
//    Aijk.resize(outputLength, 1);
    
//    cout << "Complete Aijk. Aijk Looks Like : " << endl;
//    for (int i = 0; i < outputLength; i++) {
//        cout << "i = " << i << ", Aijk[" << i << "] = " << Aijk[i] << endl;
//    }
//    cout << endl;
}

void Employee :: unwrap_Preference(int inputArray [], int length) {
    

    int outputLength = get_IJK();
    
    Pijk.resize(outputLength, 0);

    for (int i = 0; i < length; i++) {
        
        int startPoint = i * IndiceJ;
        int endPoint   = (i + 1) * IndiceJ;
        
        for (int APIndex = startPoint; APIndex < endPoint; APIndex++) {
            
            if (Aijk[APIndex] == 1) {
                Pijk[APIndex] = inputArray[i];
            }
        }
    }
    
    cout << "Pijk = [ ";
    for (int i = 0; i < outputLength; i++) {
        cout << Pijk[i] << " ";
    }
    cout << "]" << endl;
}

void Employee :: unwrap_Seniority(int inputArray []) {
    int length = IndiceI;
    
    Wi.resize(length, 0);
    for (int i = 0; i < length; i++) {
        Wi[i] = inputArray[i];
    }
    
    cout << "Wi = [ ";
    for (int i = 0; i < length; i++) {
        cout << Wi[i] << " ";
    }
    cout << "]" << endl;
}

void Employee :: make_TimeSections(int timeArray []) {
    CplexCpp cpp;
    
    int length = cpp.get_IK() * 2;
    MngTime.resize(length);
    
    for (int i = 0; i < length; i++) {
        MngTime[i] = timeArray[i];
    }
    
    cout << "timeArray = [";
    for (int i = 0; i < length; i++) {
        cout << timeArray[i] << " ";
    }
    cout << "]" << endl;
}

int  Employee :: get_IJK () {
    return IndiceI * IndiceJ * IndiceK;
}





//==========================MODEL==============================

                                    /*Model Information*/
//
//
//                                n(Yijk) = 4 * 26 * 2 = 208
//
//         D1 EMP1      X0    X1    X2    X3   ......    X13         X14   ......   X25
////       D2 EMP1     X26   X27   X28   X29   ......    X39         X40   ......   X51

//         D1 EMP2     X52   X53   X54   X55   ......    X65         X66   ......   X77
////       D2 EMP2     X78   X79   X80   X81   ......    X91         X92   ......  X103

//         D1 EMP3    X104  X105  X106  X107   ......   X117        X118   ......  X129
////       D2 EMP3    X130  X131  X132  X131   ......   X143        X144   ......  X155

//         D1 EMP4    X156  X157  X158  X159   ......   X169        X170   ......  X181
////       D2 EMP4    X182  X183  X184  X185   ......   X195        X196   ......  X207



//                    [0]   [1]   [2]   [3]             [13]        [14]           [25]
//             Bj1     2     2     2     2     ......    2           1     ......   1

////                  [26]  [27]  [28]  [29]            [26]        [27]           [51]
////           Bj2     2     2     2     2     ......    2           1     ......   1


//              Tk    ------------------7-------------------        ----------6--------
//
void populate(IloModel model, IloIntVarArray Yijk, IloRangeArray rng) {
    Employee emp;
    CplexCpp cpp;

    IloEnv env = model.getEnv();
    
    //Boundaries and/or Index Settings
    IloInt varNumber = emp.get_IJK();
    IloInt TkSize    = Tk.size();//4
    IloInt JK        = cpp.get_JK();//34 * 7 = 238


    cout << "TkSize =   " << TkSize    << endl;//
    cout << "varNumber: " << varNumber << endl;
    cout << "JK =       " << JK        << endl;
//    
//    IloIntArray objScalars = cpp.define_Coefficient(env);
//    Yijk = IloIntVarArray(env, varNumber, lowerBound, upperBound);
//    
//    model.add(IloMaximize(env, IloScalProd(objScalars, Yijk)));

//******************************CONSTRAINT 1************************************人力數量限制
    
    for (IloInt i = 0; i < JK; i++) {
        IloExpr expr(env);
        for (IloInt j = i; j < varNumber; j += JK) {
            expr.setLinearCoef(Yijk[j], 1);
        }
        IloRange cons1 = IloRange(env, Bjk[i], expr, Bjk[i]);
        rng.IloExtractableArray::add(cons1);
        expr.end();
    }
   
//==============================================================================
    
    
//******************************CONSTRAINT 2************************************日工時限制
    
    for (IloInt i = 0; i < varNumber; i += IndiceJ) { //間隔 ＝ Di
        IloExpr expr(env);
        IloInt  terminator = i + IndiceJ;
        for (IloInt j = i; j < terminator; j++) {
            expr.setLinearCoef(Yijk[j], 1);
        }
        IloRange cons2 = IloRange(env, Dmin, expr, Dmax);
        rng.IloExtractableArray::add(cons2);
        expr.end();
    }

//==============================================================================

  
//******************************CONSTRAINT 3************************************周工時限制
    
    for (IloInt i = 0; i < varNumber; i += JK) { //間隔 ＝ D1時間間隔 + D2時間間隔 + D3時間間隔 + D4時間間隔 + ... + D7時間間隔
        IloExpr expr(env);//          i = 0,   52, 104, 156
        IloInt  terminator = i + JK;//t = 52, 104, 156, 208
        for (IloInt j = i; j < terminator; j++) {
            expr.setLinearCoef(Yijk[j], 1);
        }
        IloRange cons3 = IloRange(env, Wmin, expr, Wmax);
        rng.IloExtractableArray::add(cons3);
        expr.end();
    }
    
//==============================================================================

    
//******************************CONSTRAINT 5************************************連續性
    
    for (IloInt i = 0; i < varNumber; i += IndiceJ) {
        
        IloInt terminator = i + IndiceJ;
        for (IloInt j = i; j < terminator; j++) {
            
            IloExpr LHSVars(env);
            IloExpr RHSVars(env);
            IloOr   LorR(env);
            
            IloInt filter  = j % IndiceJ;
            IloInt boarder = CTAmin - 1;//7
            
            if (filter >= 1 && filter < boarder) {//0~6
                
                LHSVars.setLinearCoef(Yijk[i], 1);
                LHSVars.setLinearCoef(Yijk[i+1], 1);
                LHSVars.setLinearCoef(Yijk[i+2], 1);
                LHSVars.setLinearCoef(Yijk[i+3], 1);
//                LHSVars.setLinearCoef(Yijk[i+4], 1);
//                LHSVars.setLinearCoef(Yijk[i+5], 1);
//                LHSVars.setLinearCoef(Yijk[i+6], 1);
//                LHSVars.setLinearCoef(Yijk[i+7], 1);
                
                RHSVars.setLinearCoef(Yijk[j], 1);
                RHSVars.setLinearCoef(Yijk[j+1], 1);
                RHSVars.setLinearCoef(Yijk[j+2], 1);
                RHSVars.setLinearCoef(Yijk[j+3], 1);
//                RHSVars.setLinearCoef(Yijk[j+4], 1);
//                RHSVars.setLinearCoef(Yijk[j+5], 1);
//                RHSVars.setLinearCoef(Yijk[j+6], 1);
//                RHSVars.setLinearCoef(Yijk[j+7], 1);
                
                LorR.add(LHSVars == CTAmin);
                LorR.add(RHSVars == CTAmin);
                
                rng.IloConstraintArray::add(IloIfThen(env, Yijk[j] == 1, LorR));

//                LHSVars.operator+=(Yijk[i] + Yijk[i+1] + Yijk[i+2] + Yijk[i+3] + Yijk[i+4] + Yijk[i+5] + Yijk[i+6] + Yijk[i+7]);
//                RHSVars.operator+=(Yijk[j] + Yijk[j+1] + Yijk[j+2] + Yijk[j+3] + Yijk[j+4] + Yijk[j+5] + Yijk[j+6] + Yijk[j+7]);
                
//                LHSVars = Yijk[i] + Yijk[i+1] + Yijk[i+2] + Yijk[i+3] + Yijk[i+4] + Yijk[i+5] + Yijk[i+6] + Yijk[i+7];
//                RHSVars = Yijk[j] + Yijk[j+1] + Yijk[j+2] + Yijk[j+3] + Yijk[j+4] + Yijk[j+5] + Yijk[j+6] + Yijk[j+7];
                
            } else if (filter >= boarder && filter < (IndiceJ - boarder)) {//7~18
                
//                LHSVars.setLinearCoef(Yijk[j-7], 1);
//                LHSVars.setLinearCoef(Yijk[j-6], 1);
//                LHSVars.setLinearCoef(Yijk[j-5], 1);
//                LHSVars.setLinearCoef(Yijk[j-4], 1);
                LHSVars.setLinearCoef(Yijk[j-3], 1);
                LHSVars.setLinearCoef(Yijk[j-2], 1);
                LHSVars.setLinearCoef(Yijk[j-1], 1);
                LHSVars.setLinearCoef(Yijk[j], 1);
                
                RHSVars.setLinearCoef(Yijk[j], 1);
                RHSVars.setLinearCoef(Yijk[j+1], 1);
                RHSVars.setLinearCoef(Yijk[j+2], 1);
                RHSVars.setLinearCoef(Yijk[j+3], 1);
//                RHSVars.setLinearCoef(Yijk[j+4], 1);
//                RHSVars.setLinearCoef(Yijk[j+5], 1);
//                RHSVars.setLinearCoef(Yijk[j+6], 1);
//                RHSVars.setLinearCoef(Yijk[j+7], 1);
                
                LorR.add(LHSVars == CTAmin);
                LorR.add(RHSVars == CTAmin);
                
                rng.IloConstraintArray::add(IloIfThen(env, Yijk[j] == 1, LorR));
                
//                LHSVars.operator+=(Yijk[j-7] + Yijk[j-6] + Yijk[j-5] + Yijk[j-4] + Yijk[j-3] + Yijk[j-2] + Yijk[j-1] + Yijk[j]);
//                RHSVars.operator+=(Yijk[j]   + Yijk[j+1] + Yijk[j+2] + Yijk[j+3] + Yijk[j+4] + Yijk[j+5] + Yijk[j+6] + Yijk[j+7]);
                
//                LHSVars = Yijk[j-7] + Yijk[j-6] + Yijk[j-5] + Yijk[j-4] + Yijk[j-3] + Yijk[j-2] + Yijk[j-1] + Yijk[j];
//                RHSVars = Yijk[j]   + Yijk[j+1] + Yijk[j+2] + Yijk[j+3] + Yijk[j+4] + Yijk[j+5] + Yijk[j+6] + Yijk[j+7];
                
            } else if (filter >= (IndiceJ - boarder) && filter < IndiceJ) {//19~25
                
//                LHSVars.setLinearCoef(Yijk[j-7], 1);
//                LHSVars.setLinearCoef(Yijk[j-6], 1);
//                LHSVars.setLinearCoef(Yijk[j-5], 1);
//                LHSVars.setLinearCoef(Yijk[j-4], 1);
                LHSVars.setLinearCoef(Yijk[j-3], 1);
                LHSVars.setLinearCoef(Yijk[j-2], 1);
                LHSVars.setLinearCoef(Yijk[j-1], 1);
                LHSVars.setLinearCoef(Yijk[j], 1);
                
                RHSVars.setLinearCoef(Yijk[terminator-1], 1);
                RHSVars.setLinearCoef(Yijk[terminator-2], 1);
                RHSVars.setLinearCoef(Yijk[terminator-3], 1);
                RHSVars.setLinearCoef(Yijk[terminator-4], 1);
//                RHSVars.setLinearCoef(Yijk[terminator-5], 1);
//                RHSVars.setLinearCoef(Yijk[terminator-6], 1);
//                RHSVars.setLinearCoef(Yijk[terminator-7], 1);
//                RHSVars.setLinearCoef(Yijk[terminator-8], 1);
                
                LorR.add(LHSVars == CTAmin);
                LorR.add(RHSVars == CTAmin);
                
                rng.IloConstraintArray::add(IloIfThen(env, Yijk[j] == 1, LorR));
                
//                LHSVars.operator+=(Yijk[j-7]   + Yijk[j-6]   + Yijk[j-5]   + Yijk[j-4]   + Yijk[j-3]   + Yijk[j-2]   + Yijk[j-1]   + Yijk[j]);
//                RHSVars.operator+=(Yijk[end-1] + Yijk[end-2] + Yijk[end-3] + Yijk[end-4] + Yijk[end-5] + Yijk[end-6] + Yijk[end-7] + Yijk[end-8]);
                
//                LHSVars = Yijk[j-7]   + Yijk[j-6]   + Yijk[j-5]   + Yijk[j-4]   + Yijk[j-3]   + Yijk[j-2]   + Yijk[j-1]   + Yijk[j];
//                RHSVars = Yijk[end-1] + Yijk[end-2] + Yijk[end-3] + Yijk[end-4] + Yijk[end-5] + Yijk[end-6] + Yijk[end-7] + Yijk[end-8];
            }
            
            
         

        }
        


    }
    
//==============================================================================
    

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




//=====================RETURN SOLUTION v1.0=========================

//Solution
//       [                   Day 1  Day 2
//Emp1    1 0 0 0    --->    09~16    0
//Emp2    0 0 1 0    --->      0    09~16
//Emp3    1 0 0 1    --->    09~16  16~22
//Emp4    0 1 1 0    --->    16~22  09~16    => Should avoid this kind of situation
//       ]





































