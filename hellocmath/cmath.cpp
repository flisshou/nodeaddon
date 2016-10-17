#include <node.h>

using namespace v8;

void Add(const FunctionCallbackInfo<Value>& args){
	Isolate* isolate = args.GetIsolate();
	
	if (args.Length() < 2){
		//Throw an Error that is passed back to JavaScript
		isolate -> ThrowException(Exception :: TypeError(String :: NewFromUtf8(isolate, "Wrong number of arguments")));
		return;
	}

	//Value is created on the C++ runtime stack, not as part of the JavaScript execution context..
	double value = args[0] -> NumberValue() + args[1] -> NumberValue();

	//Now we create it in the JS execution context so we can return it
	Local<Number> num = Number :: New(isolate, value);
	args.GetReturnValue().Set(num);
	
	if (!args[0] -> IsNumber() || !args[1] -> IsNumber()){
		return;
	}	
}

//Called when addon is require'd from JS
void Init(Local<Object> exports){
        NODE_SET_METHOD(exports, "add", Add);
}


NODE_MODULE(cmath, Init)
