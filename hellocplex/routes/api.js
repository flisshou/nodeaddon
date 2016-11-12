/*        MODEULES INITIALIZATION       */
//MODULES - npm
var express = require('express');
var router = express.Router();
var admin = require('firebase-admin');
var serviceAccount = require('/Users/flisshou/Desktop/nodeaddon/hellocplex/shifter-serviceaccountkey.json');
//MODULE - cplexcpp
var cplexcpp = require('/Users/flisshou/Desktop/nodeaddon/hellocplex/cpp/build/Release/cplexcpp');

/*              FIREBASE                */
//FIREBASE - Initializtion
admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://shifter-35349.firebaseio.com/"
});
var db = admin.database();

//FIREBASE - References
var managerEventRef  = db.ref('/managerEvent');
var managerShiftRef  = db.ref('/managerShift');
var employeeEventRef = db.ref('/employeeEvent');
var employeeShiftRef = db.ref('/employeeShift');

//DECLARATION - Arrays for being Parsed to V8 as CPLEX Constraints
var StartTimeArray        = [];
var EndTimeArray          = [];
var CodingArray = [];
var CleaningArray = [];
var DancingArray = [];

var Data = {};
var currentEvent = "";

//Query Manager Event
managerEventRef.child("010").on("value", function(date) {
  currentEvent = date.val().currentEvent;
  // console.log("currentEvent -> " + currentEvent);
  if (currentEvent == "2016-10-05"){
    managerEventRef.child("010").child("2016-10-05").orderByChild("StartDate").on("child_added", function (snapshot) {
      var data = snapshot.val();
      StartTimeArray.push(data.StartDate);
      EndTimeArray.push(data.EndDate);
      CodingArray.push(data.Coding);
      CleaningArray.push(data.Cleaning);
      DancingArray.push(data.Dancing);

      Data = {StartTimeArray, EndTimeArray, CleaningArray, DancingArray, CodingArray};
    });

    var retTimeSection = cplexcpp.define_time_sections(dateHourToInt(EndTimeArray, StartTimeArray));
    var retBaseAmount  = cplexcpp.define_base_amount(CodingArray);
    console.log("retTimeSection :: " + retTimeSection);
    console.log("retBaseAmount  :: " + retBaseAmount);
  } else {
    console.log("Not yet...");
  }
});

//QUERY EmployeeEvent DATA


//SEND DATA TO V8


function dateHourToInt(endArray, startArray){
  var length = endArray.length;
  var output = [];

  for (var i = 0; i < length; i++) {
    var endHour   = endArray[i].substring(11, 13);
    var startHour = startArray[i].substring(11, 13);
    var duration = parseInt(endHour) - parseInt(startHour);
    output.push(duration);
  }
  // console.log("in dateToInt -> " + output);
  // cplexcpp.define_time_sections(output);
  return output;
}

//SETUP CPLEX MODULE
var retDataSize    = cplexcpp.define_data_size(3, 2, 2);
var retDayBounds   = cplexcpp.define_day_bounds(0, 6);
var retWeekBounds  = cplexcpp.define_week_bounds(7, 13);
console.log("retDataSize    :: " + retDataSize);
console.log("retDayBounds   :: " + retDayBounds);
console.log("retWeekBounds  :: " + retWeekBounds);





/* GET /api/v1/mimic.*/
router.get('/mimic', function(req, res) {
  res.send(Data);
});

// /* POST /api/v1/testdata/users/cplex_managers_parameters*/
// router.post('/mimic/ManagerEvent/010/2016-12-12', function(req, res) {
//   if(!req.body) return res.sendStatus(400);
//   for(var key in req.body) {
//     managerRef.child(key).set(req.body[key], function(error) {
//       if (error) {
//         console.log("Data could not be saved. " + error);
//       } else {
//         console.log("Data saved Successfully");
//       }
//     });
//   } res.sendStatus(200);
// });
//

// modeule.exports = api;
module.exports = router;
