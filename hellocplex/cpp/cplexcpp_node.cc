#include <iostream>
#include <node.h>
#include <v8.h>
#include <uv.h>
#include "cplexcpp.h"

using namespace v8;
using namespace std;

void DefineDataSize (const v8 :: FunctionCallbackInfo<v8 :: Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CplexCpp cpp;

    int JS_workers = args[0]->Int32Value();
    int JS_shifts  = args[1]->Int32Value();
    int JS_days    = args[2]->Int32Value();

  cpp.define_DataSize(JS_workers, JS_shifts, JS_days);

  Local<String> message = String :: NewFromUtf8(isolate, "Calling DefineDataSize Function");

  args.GetReturnValue().Set(message);
}

void Init (Handle <Object> exports, Handle<Object> module) {
  NODE_SET_METHOD(exports, "define_data_size", DefineDataSize);
}

NODE_MODULE(cplexcpp, Init)
