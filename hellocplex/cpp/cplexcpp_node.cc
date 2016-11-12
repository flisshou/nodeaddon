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

void Init (Handle <Object> exports, Handle<Object> module) {
    NODE_SET_METHOD(exports, "define_data_size",     DefineDataSize);
    NODE_SET_METHOD(exports, "define_day_bounds",    DefineDayBounds);
    NODE_SET_METHOD(exports, "define_week_bounds",   DefineWeekBounds);
    NODE_SET_METHOD(exports, "define_time_sections", DefineTimeSections);
    NODE_SET_METHOD(exports, "define_base_amount",   DefineBaseAmount);
}

NODE_MODULE(cplexcpp, Init)














