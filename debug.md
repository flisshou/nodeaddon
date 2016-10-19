
##Debug

###package.json

- Adding the `"gypfile" : true`:

	Don't forget the **comma** when you update a new instance within the JSON object.

- The `"main" : "XXXX.js"`

	Make sure of the **value** pointing to the right JavaScript file.

---

###binding.gyp

- When you want to configure and build `node-gyp`:
        
        You must go to the right directory.
	Usually, we put the `binding.gyp` and any other C++ files together in a `/cpp` directory.
	Other JavaScript files would be in the project directory.
	
