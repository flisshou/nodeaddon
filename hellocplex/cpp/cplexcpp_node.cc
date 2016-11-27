#include <iostream>
#include <node.h>
#include <v8.h>
#include <uv.h>
#include "cplexcpp.h"

using namespace v8;
using namespace std;

void DefineDataSize (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;

    int JS_workers = args[0]->Int32Value();
    int JS_shifts  = args[1]->Int32Value();
    int JS_days    = args[2]->Int32Value();

    cpp.define_DataSize(JS_workers, JS_shifts, JS_days);

    Local<String> msg = String :: NewFromUtf8(isolate, "Calling DefineDataSize Function");
    
    args.GetReturnValue().Set(msg);
}

void DefineTimeSections (const FunctionCallbackInfo<Value>& args){
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    Local<Array> inputArray = Local<Array> :: Cast(args[0]);
    
    int outputLength = cpp.get_JK();
    int outputArray[outputLength];
    
    for (int i = 0; i < outputLength; i++) {
        outputArray[i] = inputArray->Get(i)->Int32Value();
    }
    
    cpp.define_TimeSections(outputArray);
    
    Local<String> msg = String :: NewFromUtf8(isolate, "Received Tjk Data.");
    args.GetReturnValue().Set(msg);

}

void DefineBaseAmount(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    Local<Array> inputArray = Local<Array> :: Cast(args[0]);
    
    int outputLength = cpp.get_JK();
    int outputArray[outputLength];
    
    for (int i = 0; i < outputLength; i++) {
        outputArray[i] = inputArray->Get(i)->Int32Value();
    }
    
    cpp.define_BaseAmount(outputArray);
    
    Local<String> msg = String :: NewFromUtf8(isolate, "Received Bjk Data.");
    args.GetReturnValue().Set(msg);
}

void DefineDayBounds (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    int JS_min = args[0]->Int32Value();
    int JS_max = args[1]->Int32Value();
    
    cpp.define_DayBounds(JS_min, JS_max);
    
    Local<String> msg = String :: NewFromUtf8(isolate, "Defined Day Bounds.");
    args.GetReturnValue().Set(msg);
}

void DefineWeekBounds (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    int JS_min = args[0]->Int32Value();
    int JS_max = args[1]->Int32Value();
    
    cpp.define_WeekBounds(JS_min, JS_max);
    
    Local<String> msg = String :: NewFromUtf8(isolate, "Defined Week Bounds.");
    args.GetReturnValue().Set(msg);
}

void UnwrapAvailability (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    Employee emp;
    
    Local<Array> inputArray = Local<Array> :: Cast(args[0]);
    
    int outputLength = cpp.get_IK();
    int outputArray[outputLength];
    
    for (int i = 0; i < outputLength; i++) {
        outputArray[i] = inputArray->Get(i)->Int32Value();
    }
    
    emp.print_Duration(outputArray);
    emp.unwrap_Availability(outputArray);
    
    Local<String> msg = String :: NewFromUtf8(isolate, "Received Aijk Data.");
    args.GetReturnValue().Set(msg);
}

void UnwrapPreference (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    Employee emp;
    
    Local<Array> inputArray = Local<Array> :: Cast(args[0]);
    
    int outputLength = cpp.get_IK();
    int outputArray[outputLength];
    
    for (int i = 0; i < outputLength; i++) {
        outputArray[i] = inputArray->Get(i)->Int32Value();
    }
    
    emp.unwrap_Preference(outputArray);
    emp.print_Preference(outputArray);
    
    Local<String> msg = String :: NewFromUtf8(isolate, "Received Pijk Data.");
    args.GetReturnValue().Set(msg);
}

void GetIJ (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    Local<Number> retValue = Int32 :: New(isolate, cpp.get_IJ());
    
    args.GetReturnValue().Set(retValue);
}

void GetJK (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    Local<Number> retValue = Int32 :: New(isolate, cpp.get_JK());
    
    args.GetReturnValue().Set(retValue);
}

void GetIK (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    Local<Number> retValue = Int32 :: New(isolate, cpp.get_IK());
    
    args.GetReturnValue().Set(retValue);
}

void RunCPLEX (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    
    cpp.runCplex();
    
    Local<String> msg = String :: NewFromUtf8(isolate, "Invoking the CPLEX CPP......");
    args.GetReturnValue().Set(msg);
}

void ReturnSolution (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CplexCpp cpp;
    Employee emp;
    
    Local<Array>  EmpIdArray        = Local<Array> :: Cast(args[0]);
    Local<Array>  MngStartTimeArray = Local<Array> :: Cast(args[1]);
    Local<Array>  MngEndTimeArray   = Local<Array> :: Cast(args[2]);
    
    Local<Object> outputObject = Object :: New(isolate);
    Local<Object> schedObject  = Object :: New(isolate);
    
    Local<String> start = String :: NewFromUtf8(isolate, "StartTime");
    Local<String> end   = String :: NewFromUtf8(isolate, "EndTime");
    Local<String> date  = String :: NewFromUtf8(isolate, "2016-10-06");
    
    int outputLength = emp.get_IJK();
    int JK = cpp.get_JK();

    int diff = emp.get_IJK() / cpp.get_IJ(); // diff = K = 2
    
    int solution[] = {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0};
    
    for (int i = 0; i < outputLength; i += diff) {
        
        
        
        int empId = 0;
        
        switch (i) {
            case  0:
            case  1:
            case  2:
            case  3:
                empId = 0;
                break;
            case  4:
            case  5:
            case  6:
            case  7:
                empId = 1;
                break;
            case  8:
            case  9:
            case 10:
            case 11:
                empId = 2;
                break;
            case 12:
            case 13:
            case 14:
            case 15:
                empId = 3;
                break;
            default:
                break;
        }
        
        Local<Object> d1 = Object :: New(isolate);
        Local<Object> d2 = Object :: New(isolate);
        
        Local<Object> emp = Object :: New(isolate);
        
        Local<String> d1Key = String :: NewFromUtf8(isolate, "001");
        Local<String> d2Key = String :: NewFromUtf8(isolate, "002");
        
        Local<Value> empKey = Local<Value> :: New(isolate, EmpIdArray->Get(i));
        
        
        if (solution[i] == 1 && solution[i+1] == 0) {//只上早班
            
            if (i % JK == 0) {//DAY 1
                
                
                
                d1->Set(start, MngStartTimeArray->Get(0));
                d1->Set(end, MngEndTimeArray->Get(0));
                emp->Set(d1Key, d1);
                
                schedObject->Set(empKey, emp->Clone());

//                emp->Delete(d1Key);
            } else if (i % JK != 0) {//DAY 2
                d2->Set(start, MngStartTimeArray->Get(2));
                d2->Set(end, MngEndTimeArray->Get(2));
                emp->Set(d2Key, d2);
                
                schedObject->Set(empKey, emp->Clone());
        
//                emp->Delete(d2Key);
            } else {
                cout << "Technical Problem in [1, 0] Scenario" << endl;
                cout << "Returning..." << endl;
            }
        } else if (solution[i] == 0 && solution[i+1] == 1) {//只上晚班
            
            if (i % JK == 0) {//DAY 1
                d1->Set(start, MngStartTimeArray->Get(1));
                d1->Set(end, MngEndTimeArray->Get(1));
                emp->Set(d1Key, d1);
                
                schedObject->Set(empKey, emp->Clone());
                
//                emp->Delete(d1Key);
            } else if (i % JK != 0) {//DAY 2
                d2->Set(start, MngStartTimeArray->Get(3));
                d2->Set(end, MngEndTimeArray->Get(3));
                emp->Set(d2Key, d2);
                
                schedObject->Set(empKey, emp->Clone());
            
//                emp->Delete(d2Key);
            } else {
                cout << "Technical Problem in [0, 1] Scenario" << endl;
                cout << "Returning..." << endl;
            }
        } else if (solution[i] == 1 && solution[i+1] == 1) {//上早晚班

            
            if (i % JK == 0) {//DAY 1
                d1->Set(start, MngStartTimeArray->Get(0));
                d1->Set(end, MngEndTimeArray->Get(1));
                emp->Set(d1Key, d1);

                
                schedObject->Set(empKey, emp->Clone());
                
//                emp->Delete(d1Key);
            } else if (i % JK == 1) {
                d2->Set(start, MngStartTimeArray->Get(2));
                d2->Set(end, MngEndTimeArray->Get(3));
                emp->Set(d2Key, d2);
                
                schedObject->Set(empKey, emp->Clone());
                
//                emp->Delete(d2Key);
            } else {
                cout << "Technical Problem in [1, 1] Scenario" << endl;
                cout << "Returning..." << endl;
            }
        } else {
            cout << "Something goes to ELSE here" << endl;
        }
    }
    

//    
//    cout << "solution ----> [ ";
//    for (int i = 0; i < outputLength; i++) {
//        cout << solution[i] << " ";
//    }
//    cout << "]" << endl << endl;
//    
    
//    for (int i = 0; i < JK; i++) {
//        
//        int index;
//        
//        for (index = i * JK; index < (index + JK); index += diff) {
//            
//            if (solution[index] == 1 && solution[index + 1] == 0) {//只上早班
//                Local<Object> d1 = Object :: New(isolate);
//                Local<Object> d2 = Object :: New(isolate);
//
//                
//                if (index % JK == 0) {//Day 1
//                    d1->Set(start, MngStartTimeArray->Get(0));
//                    d1->Set(end, MngEndTimeArray->Get(0));
//
//                    schedObject->Set(EmpIdArray->Get(i), d1->Clone());
//
//                    d1->Delete(start);
//                    d2->Delete(end);
//                } else if (index % JK != 0) {//Day 2
//                    d2->Set(start, MngStartTimeArray->Get(2));
//                    d2->Set(end, MngEndTimeArray->Get(2));
//
//                    schedObject->Set(EmpIdArray->Get(i), d2->Clone());
//
//                    d2->Delete(start);
//                    d2->Delete(end);
//                }
//                
//            } else if (solution[index] == 0 && solution[index + 1] == 1) {//只上晚班
//                Local<Object> d1 = Object :: New(isolate);
//                Local<Object> d2 = Object :: New(isolate);
//                
//                if (index % JK == 0) {//Day 1
//                    d1->Set(start, MngStartTimeArray->Get(1));
//                    d1->Set(end, MngEndTimeArray->Get(1));
//
//                    schedObject->Set(EmpIdArray->Get(i), d1->Clone());
//
//                    d1->Delete(start);
//                    d1->Delete(end);
//                } else if (index % JK != 0 ) {//Day 2
//                    d2->Set(start, MngStartTimeArray->Get(3));
//                    d2->Set(end, MngEndTimeArray->Get(3));
//
//                    schedObject->Set(EmpIdArray->Get(i), d2->Clone());
//
//                    d2->Delete(start);
//                    d2->Delete(end);
//                }
//            } else if (solution[index] == 1 && solution[index + 1] == 1) {//早晚班都上
//                Local<Object> d1 = Object :: New(isolate);
//                Local<Object> d2 = Object :: New(isolate);
//                
//                if (index % JK == 0) {//Day 1
//                    d1->Set(start, MngStartTimeArray->Get(0));
//                    d1->Set(end, MngEndTimeArray->Get(1));
//                    
//                    schedObject->Set(EmpIdArray->Get(i), d1->Clone());
//                    
//                    d1->Delete(start);
//                    d1->Delete(end);
//                } else if (index % JK != 0) {//Day 2
//                    d2->Set(start, MngStartTimeArray->Get(2));
//                    d2->Set(end, MngEndTimeArray->Get(3));
//
//                    schedObject->Set(EmpIdArray->Get(i), d2->Clone());
//
//                    d2->Delete(start);
//                    d2->Delete(end);
//                }
//                
//            } else if (solution[index] == 0 && solution[index + 1] == 0) {
//
//            }
//        }
//        
//    }
    
    
    
    
    
    outputObject->Set(date, schedObject);
    
    args.GetReturnValue().Set(outputObject);

}

void TestJSON (const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
//    CplexCpp cpp;
//    Employee emp;
    
    Local<Array>  EmpIdArray        = Local<Array> :: Cast(args[0]);//{102306111 102306222 102306333 102306444}
    Local<Array>  MngStartTimeArray = Local<Array> :: Cast(args[1]);//{   09        16        09        16}
    Local<Array>  MngEndTimeArray   = Local<Array> :: Cast(args[2]);//{   16        22        16        22}
    
    Local<Object> outputObject = Object :: New(isolate);
    Local<Object> schedObject  = Object :: New(isolate);
    
    Local<String> start = String :: NewFromUtf8(isolate, "StartTime");
    Local<String> end   = String :: NewFromUtf8(isolate, "EndTime");
    Local<String> date  = String :: NewFromUtf8(isolate, "2016-10-06");
    
//    int outputLength = emp.get_IJK();
//    int JK = cpp.get_JK();
////
//    int diff = emp.get_IJK() / cpp.get_IJ(); // diff = K = 2
//    
//    int solution[] = {1, 0, 0, 0};
    
    Local<Object> d1 = Object :: New(isolate);
    
    d1->Set(start->ToString(), MngStartTimeArray->Get(0));
    d1->Set(end->ToString(), MngEndTimeArray->Get(0));
    
    schedObject->Set(EmpIdArray->Get(0), d1->Clone());
    
    d1->Delete(start);
    d1->Delete(end);
    
    outputObject->Set(date, schedObject);
    
    
    
//    for (int i = 0; i < JK; i += diff) {
//        
//        Local<Object> d1 = Object :: New(isolate);
//        Local<Object> d2 = Object :: New(isolate);
//        
//        if (solution[i] == 1 && solution[i+1] == 0) {//只上早班
//          
//            if (i % JK == 0) {//Day 1
//                
////                Local<Object> tp = Object :: New(isolate);
//                
//                d1->Set(start, MngStartTimeArray->Get(0));
//                d1->Set(end, MngEndTimeArray->Get(0));
////                tp->Set(start, d1->Get(start));
////                tp->Set(end, d1->Get(end));
//                
//                schedObject->Set(EmpIdArray->Get(0), d1->Clone());
//                
//                d1->Delete(start);
//                d1->Delete(end);
//                
//
//            } else if (i % JK != 0) {
////                Local<Object> tp = Object :: New(isolate);
//                
//                d2->Set(start, MngStartTimeArray->Get(2));
//                d2->Set(end, MngEndTimeArray->Get(2));
////                tp->Set(start, MngStartTimeArray->Get(start));
////                tp->Set(end, MngEndTimeArray->Get(end));
//                
//                schedObject->Set(EmpIdArray->Get(0), d2->Clone());
//                
//                d2->Delete(start);
//                d2->Delete(end);
//            }
//            
//        } else if (solution[i] == 0 && solution[i+1] == 1) { //只上晚班
//            
//            if (i % JK == 0) {
////                Local<Object> tp = Object :: New(isolate);
//                
//                d1->Set(start, MngStartTimeArray->Get(1));
//                d1->Set(end, MngEndTimeArray->Get(1));
//                
//                schedObject->Set(EmpIdArray->Get(0), d1->Clone());
//                
//                d1->Delete(start);
//                d1->Delete(end);
//            } else if (i % JK != 0) {
//                d2->Set(start, MngStartTimeArray->Get(3));
//                d2->Set(end, MngEndTimeArray->Get(3));
//                
//                schedObject->Set(EmpIdArray->Get(0), d2->Clone());
//                
//                d2->Delete(start);
//                d2->Delete(end);
//            }
//        } else if (solution[i] == 1 && solution[i+1] == 1) {
//            if (i % JK == 0) {
//                d1->Set(start, MngStartTimeArray->Get(0));
//                d1->Set(end, MngEndTimeArray->Get(1));
//                
//                schedObject->Set(EmpIdArray->Get(0), d1->Clone());
//                
//                d1->Delete(start);
//                d1->Delete(end);
//            } else if (i % JK != 0) {
//                d2->Set(start, MngStartTimeArray->Get(2));
//                d2->Set(end, MngEndTimeArray->Get(3));
//                
//                schedObject->Set(EmpIdArray->Get(0), d2->Clone());
//                
//                d2->Delete(start);
//                d2->Delete(end);
//            }
//            
//        } else {
//            cout << "Something goes wrong" << endl;
//            cout << "Returning..........." << endl << endl;
//            
//            return;
//        }
//    }
    
//    outputObject->Set(date, schedObject);
    
    args.GetReturnValue().Set(outputObject);
    
    
}

void Init (Handle <Object> exports, Handle<Object> module) {
    NODE_SET_METHOD(exports, "define_data_size",     DefineDataSize);
    NODE_SET_METHOD(exports, "define_day_bounds",    DefineDayBounds);
    NODE_SET_METHOD(exports, "define_week_bounds",   DefineWeekBounds);
    NODE_SET_METHOD(exports, "define_time_sections", DefineTimeSections);
    NODE_SET_METHOD(exports, "define_base_amount",   DefineBaseAmount);
    NODE_SET_METHOD(exports, "unwrap_availability",  UnwrapAvailability);
    NODE_SET_METHOD(exports, "unwrap_preference",    UnwrapPreference);
    NODE_SET_METHOD(exports, "get_IJ", GetIJ);
    NODE_SET_METHOD(exports, "get_JK", GetJK);
    NODE_SET_METHOD(exports, "get_IK", GetIK);
    NODE_SET_METHOD(exports, "run_cplex",       RunCPLEX);
    NODE_SET_METHOD(exports, "return_solution", ReturnSolution);
    NODE_SET_METHOD(exports, "test_JSON", TestJSON);
}

NODE_MODULE(cplexcpp, Init)



































