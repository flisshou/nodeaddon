//var hello = require('/Users/flisshou/Desktop/nodeaddon/hellomodule/hello_module');
//console.log(hello.say_hello());

const addon = require('/Users/flisshou/Desktop/nodeaddon/hellomodule/build/Release/hello_addon');

console.log(addon.hello());
