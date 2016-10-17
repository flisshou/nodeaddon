const cmath = require('/Users/flisshou/Desktop/nodeaddon/hellocmath/build/Release/cmath');

const value1 = cmath.add(2, 3);
const value2 = cmath.add(10, 329);
console.log(" 2 +   3 = " + value1 + "\n");
console.log("10 + 329 = " + value2 + "\n");

var u = cmath.add(1);
console.log("u: " + u + "\n");
console.log("u===undefined: ");
console.log(u===undefined);
