
#Instruction to install node-gyp

###Step 1: Create a C++ file

```cpp
//hello.cc

#include <node.h>

namespace demo {

using v8 :: FunctionCallbackInfo;
using v8 :: Isolate;
using v8 :: Local;
using v8 :: Object;
using v8 :: String;
using v8 :: Value;

void Method () {
	Isolate* isolate = args.GetIsolate();
	args.GetReturnValue().Set(String :: NewFromUtf8(isolate, "world"));
}

void init (Local<Object> exports) {
	NODE_SET_METHOD(exports, "hello", Method);
}

NODE_MODULE(addon, init)	//No semi-colon after NODE_MODULE
}

```

---

###Step 2: Building

Create a file called `binding.gyp`

```javascript
{
	"targets" : [
		{
		  "target_name" : "addon",
		  "sources" : [hello.cc]		
		}
	]
}


```

---

###Step 3: Install node-gyp

In terminal:

```bash
$ npm init
```

```bash
$ sudo npm install -g node-gyp
```

---

###Step 4: Edit package.json

In package.json: add a key-value pair

```javascript
"gypfile" : true
```

---

###Step 5: Configuration

In terminal:

```bash
$ node-gyp configure
```

```bash
$ node-gyp build
```

---

###Step 6: Connection of Node.js and C++

Create a JavaScript file:

```javascript
//hello.js

const addon = require('./build/Release/addon');

console.log(addon.hello());

```

---

###Step 7: Run

```bash
$ node hello.js 
```
>world

---

##Additional 

[Some simple debug might encounter](https://github.com/flisshou/nodeaddon/blob/master/debug.md)

---
