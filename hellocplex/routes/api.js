/*              MODEULES INITIALIZATION              */
//MODULES - npm
var express = require('express');
var router = express.Router();
var admin = require('firebase-admin');
var serviceAccount = require('/Users/flisshou/Desktop/nodeaddon/hellocplex/shifter-serviceaccountkey.json');
//MODULE - cplexcpp
var cplexcpp = require('/Users/flisshou/Desktop/nodeaddon/hellocplex/cpp/build/Release/cplexcpp');

/*                    CPLEX SETUP                    */
var retDataSize    = cplexcpp.define_data_size(4, 2, 2);
var retDayBounds   = cplexcpp.define_day_bounds(0, 6);
var retWeekBounds  = cplexcpp.define_week_bounds(7, 13);

var sizeIJ = cplexcpp.get_IJ();
var sizeJK = cplexcpp.get_JK();
var sizeIK = cplexcpp.get_IK();

console.log("retDataSize    :: " + retDataSize);
console.log("retDayBounds   :: " + retDayBounds);
console.log("retWeekBounds  :: " + retWeekBounds);

console.log("sizeIJ :: " + sizeIJ);
console.log("sizeJK :: " + sizeJK);
console.log("sizeIK :: " + sizeIK);

/*       PARSING ARRAYS AND OBJECTS DECLARATION      */
//MANAGER EVENT - Declarations
var MngStartTimeArray = [];
var MngEndTimeArray   = [];
var MngCodingArray    = [];
var MngCleaningArray  = [];
var MngDancingArray   = [];
//EMPLOYEE EVENT - Declarations
var EmpPreferenceArray = [];
var EmpStartTimeArray  = [];
var EmpEndTimeArray    = [];
//DATA TO SHOW
var MngData = {};
var EmpData = {};

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

//Query Manager Event
managerEventRef.child("010").on("value", function(date) {
  var currentEvent = date.val().currentEvent;
  console.log("currentEvent in managerEventRef-> " + currentEvent);

  if (currentEvent == "2016-10-05") {
    managerEventRef.child("010").child("2016-10-05").orderByChild("StartDate").on("child_added", function (snapshot) {
      var data = snapshot.val();
      MngStartTimeArray.push(data.StartDate);
      MngEndTimeArray.push(data.EndDate);
      MngCodingArray.push(data.Coding);
      MngCleaningArray.push(data.Cleaning);
      MngDancingArray.push(data.Dancing);

      MngData = {MngStartTimeArray, MngEndTimeArray, MngCleaningArray, MngDancingArray, MngCodingArray};
    });

    ////SEND PARSED DATA TO V8
    var retTimeSection = cplexcpp.define_time_sections(dateHourToInt(MngEndTimeArray, MngStartTimeArray));
    var retBaseAmount  = cplexcpp.define_base_amount(MngCodingArray);
    console.log("retTimeSection :: " + retTimeSection);
    console.log("retBaseAmount  :: " + retBaseAmount);
  } else {
    console.log("Not yet...");
  }
});
//Query Employee Event
employeeEventRef.child("010").on("value", function(date) {
  var currentEvent = date.val().currentEvent;
  console.log("currentEvent in employeeEventRef -> " + currentEvent);

  if (currentEvent == "2016-10-16") {
    employeeEventRef.child("010").child("2016-10-16").orderByKey().on("child_added", function (snapshot) {
      snapshot.forEach(function(childSnapshot) {
        var data = childSnapshot.val();
        // console.log("snapshot.key      -> " + snapshot.key);
        // console.log("childSnapshot.key -> " + childSnapshot.key);

        EmpStartTimeArray.push(data.StartDate);
        EmpEndTimeArray.push(data.EndDate);
        EmpPreferenceArray.push(data.Preference);

        EmpData = {EmpStartTimeArray, EmpEndTimeArray, EmpPreferenceArray};
      });
    });

    var durationArray = dateHourToInt(EmpEndTimeArray, EmpStartTimeArray);
    // console.log("durationArray -> " + durationArray);

    var retPrintDuration = cplexcpp.unwrap_availability(durationArray);
    console.log("retPrintDuration   :: " + retPrintDuration);

    console.log("EmpPreferenceArray -> " + EmpPreferenceArray);
    var retPrintPreference = cplexcpp.unwrap_preference(EmpPreferenceArray);
    console.log("retPrintPreference :: " + retPrintPreference);

    // console.log("EmpStartTimeArray  -> " + EmpStartTimeArray);
    // console.log("EmpEndTimeArray    -> " + EmpEndTimeArray);
    // console.log("EmpPreferenceArray -> " + EmpPreferenceArray);
    // console.log(EmpData);
  } else {
    console.log("Not yet...");
  }

  var retRunCPLEX = cplexcpp.run_cplex();
  console.log("retRunCPLEX :: " + retRunCPLEX);
});


//LOCAL FUNCTION - PARSE yyyy-mm-dd-hh:mm -> hh
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




/* GET /api/v1/mimic.*/
router.get('/mimic', function(req, res) {
  res.send(EmpData);
});

// modeule.exports = api;
module.exports = router;






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
