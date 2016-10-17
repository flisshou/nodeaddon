#Essential Practice - Hello World

##Create C++ File

```cpp
//hello.cpp

#include <node.h>

using namespace v8;

void Method(const FunctionCallbackInfo<Value>& args){
	Isolate* isolate = args.GetIsolate();
	Local<String> retval = String :: NewFromUtf8(isolate, "world");
	args.GetReturnValue().Set(retval);
}

void init(Local<Object> exports){
	NODE_SET_METHOD(exports, "hello", Method); 
	//the 2nd parameter is the function name when you invoke in JavaScript file
	//the 3rd parameter is the executing code when the function is invoked
}

NODE_MODULE(hello_addon, init)
```

##Create `binding.gyp`
```javascript
{
	targets: [
        {
	    "target_name": "hello_addon",
	    "sources": ["hello.cpp"]
	}
]
}
```
> Remember to match "target_name" with the first parameter in NODE_MODULE, 

> and match "sources" to the right C++ filename as well.

###Create JavaScript File

```javascript
//hello.js

const addon = require('./build/Release/hello_addon');
console.log(addon.hello());
```

> Remember to match the "target_name" when you require the module made by C++,

> so that node-gyp could correctly integrate all of the files.

###Set Environment

```bash
$ npm init
```

###Build node-gyp

```bash
$ node-gyp configure build
```

###Run the Result

```bash
$ node hello.js
```
>world

---
