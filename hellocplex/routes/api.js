/*              MODEULES INITIALIZATION              */
//MODULES - npm
var express = require('express');
var router = express.Router();
var admin = require('firebase-admin');
var serviceAccount = require('/Users/flisshou/Desktop/nodeaddon/hellocplex/shifter-serviceaccountkey.json');
var moment = require('moment');
//MODULE - cplexcpp
var cplexcpp = require('/Users/flisshou/Desktop/nodeaddon/hellocplex/cpp/build/Release/cplexcpp');

// /*                    CPLEX SETUP                    */
// var retDataSize    = cplexcpp.define_data_size(4, 13, 2); //(I, J, K) = (員工數, [結束營業時間 - 開始營業時間], 天數)
var retDayBounds   = cplexcpp.define_day_bounds( 0, 8);
var retWeekBounds  = cplexcpp.define_week_bounds(20, 40);
var IndiceK  = 6;
/*       STORES INFORMATION*/
var StoreIdArray       = [];
var StoreBHArray       = [];
var StoreMemebersArray = [];
var CurrentStore       = "";
var IsSchedulingSwitch = false;
var CurrentEvent       = "";
/*       PARSING ARRAYS AND OBJECTS DECLARATION      */
//MANAGER EVENT - Declarations
var MngStartTimeArray = [];
var MngEndTimeArray   = [];
var MngBeverageArray  = [];
var MngCashierArray   = [];
var MngCleaningArray  = [];
var counterArray = [0, 0, 0, 0, 0, 0];

// var EmpIdArray = [];
// var EmpTimeArray = [];
// var EmpPrefArray = [];
// var EmpAvailabilityArray = [];
// var EmpPreferenceArray   = [];
// var EmpNameArray = [];
//DATA TO SHOW
var MngData = {};
// var EmpData = {};

// MANAGER TIME SEGMENTS
var startSegmentsArray = [];
var endSegmentsArray   = [];

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
var employeeSpareRef = db.ref('/employeeSpare');
var storeRef         = db.ref('/store');
var employeeRef      = db.ref('/employee');


/*===========================S     T     A     R     T===========================*/
Start();
/*===============================================================================*/
function Start () {
//Set StoreIdArray
  storeRef.orderByKey().on("value", function (snapshot) {
    snapshot.forEach(function (node) {
      StoreIdArray.push(node.key);
    });
    StoreSelections(0);
    console.log("StoreIdArray = " + StoreIdArray);

 //Set StoreBHArray
    storeRef.child(CurrentStore).child('businessHour').on("value", function (bh) {
      var sh = parseInt(moment(bh.val().startHour, "HH:mm").format('HH'));
      var eh = parseInt(moment(bh.val().endHour, "HH:mm").format('HH'));

      StoreBHArray.push(sh);
      StoreBHArray.push(eh);
      console.log("StoreBHArray = " + StoreBHArray);
    });



 //Set Listeners
    managerEventRef.child(CurrentStore).on("value", function (listener) {
      IsSchedulingSwitch = listener.val().isSchedulingSwitch;
      CurrentEvent       = listener.val().currentEvent;

      console.log("CurrentEvent = " + CurrentEvent);
      // if (IsSchedulingSwitch) {
      //   Scheduling();
      // }
      Scheduling();
    });
  });
}

/*=========================A  L  L  O  C  A  T  I  O  N==========================*/
// SupplyAndDemand();
/*===============================================================================*/
function isSufficient (MngDemandArray, xI) {
  var length = MngDemandArray.length;
  var demandTimeArray = dateHourToInt(MngEndTimeArray, MngStartTimeArray, "Mng");
  var demands  = 0;
  var supplies = 0;
  var upperBound = 40;

  for (var i = 0; i < length; i++) {
    demands += demandTimeArray[i] * MngDemandArray[i];
  }

  supplies = xI * upperBound;
  console.log("demands : supplies = " + demands + " : " + supplies);

  var result = supplies >= demands ? true : false;
  return result;
}
//MAIN FUNCTION
function Scheduling () {
  console.log("Start To Schedule.");
  ManagerDemand();
  // EmployeeSupply(MngBeverageArray, "beverage");
  // EmployeeSupply(MngCashierArray,  "cashier");
  EmployeeSupply(MngCleaningArray, "cleaning");
}

function StoreSelections (StoreIndex) {
  CurrentStore = StoreIdArray[StoreIndex];
  console.log("Current Store is -> " + CurrentStore);
}

function ManagerDemand () {
  managerEventRef.child(CurrentStore).child(CurrentEvent).orderByChild("StartDate").on("child_added", function (snapshot) {
    var data = snapshot.val();
    MngStartTimeArray.push(data.StartDate);
    MngEndTimeArray.push(data.EndDate);
    MngBeverageArray.push(data.beverage);
    MngCashierArray.push(data.cashier);
    MngCleaningArray.push(data.cleaning);
  });

  console.log("MngBeverageArray -> " + MngBeverageArray);
  console.log("MngCashierArray  -> " + MngCashierArray);
  console.log("MngCleaningArray -> " + MngCleaningArray);
  setMngTimes(MngEndTimeArray, MngStartTimeArray);
}

function EmployeeSupply (MngDemandArray, profession) {
  counterArray = [0, 0, 0, 0, 0, 0];
  var EmpIdArray = [];
  var EmpTimeArray = [];
  var EmpPrefArray = [];
  var EmpAvailabilityArray = [];
  var EmpPreferenceArray   = [];
  var EmpNameArray = [];
  var curEvent = "";
  var IndiceI  = 0;
  var IndiceJ  = 0;
  // var IndiceK  = 6;
  console.log("Current profession  -> " + profession);
  console.log("MngDemandArray      -> " + MngDemandArray);
 //Retrieve EmpIdArray EmpNameArray

  employeeRef.orderByKey().on("child_added", function(snapshot) {
    if ((snapshot.val().store == CurrentStore) && (snapshot.val().profession == profession)) {
      EmpIdArray.push(snapshot.val().ID);
      EmpNameArray.push(snapshot.val().name);
    }
  });
 //Retrieve currentEvent in employeeEvent
  employeeEventRef.child(CurrentStore).on("value", function (date) {
    curEvent = date.val().currentEvent;
    console.log("curEvent            -> " + curEvent);
 //Retrieve EmpIdArray
    if (curEvent == CurrentEvent) {
      employeeEventRef.child(CurrentStore).child(curEvent).child(profession).orderByKey().on("value", function (snapshot) {

 //Set IndiceJ
        var Tk  = dateHourToInt(MngEndTimeArray, MngStartTimeArray, "Mng");
        IndiceJ = calcIndiceJ(Tk);

        // IndiceI = EmpIdArray.length;
        IndiceI = 0;
        if (isSufficient(MngDemandArray, IndiceI)) {
          console.log("It's Sufficient!!");
          console.log("EmpIdArray          -> " + EmpIdArray);
          console.log("EmpNameArray        -> " + EmpNameArray);
          // IndiceI = EmpIdArray.length;
          // console.log("IndiceJ             -> " + IndiceJ);
   //Send Indice to C++
          var retDataSize    = cplexcpp.define_data_size(IndiceI, IndiceJ, IndiceK);
          var retTimeSection = cplexcpp.define_time_sections(Tk, Tk.length);
          var retBaseAmount  = cplexcpp.define_base_amount(MngDemandArray);
          console.log("retDataSize         :: " + retDataSize);
          console.log("retTimeSection      :: " + retTimeSection);
          console.log("retBaseAmount       :: " + retBaseAmount);

          for (var i = 0; i < EmpIdArray.length; i++) {
            var emp = EmpIdArray[i];
            EmpTimeArray.push(getEmpTime(curEvent, profession, emp));
            EmpAvailabilityArray = EmpAvailabilityArray.concat(EmpTimeArray[i]);
            EmpPrefArray.push(getPref(curEvent, profession, emp));
            EmpPreferenceArray   = EmpPreferenceArray.concat(EmpPrefArray[i]);
          }

          console.log("EmpTimeArray         -> " + EmpTimeArray         + "; EmpTimeArray Length         = " + EmpTimeArray.length);
          console.log("EmpAvailabilityArray -> " + EmpAvailabilityArray + "; EmpAvailabilityArray Length = " + EmpAvailabilityArray.length);
          console.log("EmpPrefArray         -> " + EmpPrefArray         + "; EmpPrefArray Length         = " + EmpPrefArray.length);
          console.log("EmpPreferenceArray   -> " + EmpPreferenceArray   + "; EmpPreferenceArray Length   = " + EmpPreferenceArray.length);
          var retUnwrapBH       = cplexcpp.unwrap_business_hours(StoreBHArray);
          var retUnwrapAvail    = cplexcpp.unwrap_availability(EmpAvailabilityArray, EmpAvailabilityArray.length);
          var retUnwrapPref     = cplexcpp.unwrap_preference(EmpPreferenceArray, EmpPreferenceArray.length);
          var retRunCPLEX       = cplexcpp.run_cplex();
          var retRetrieveSpares = cplexcpp.retrieve_spares();
          console.log("retUnwrapBH       :: " + retUnwrapBH);
          console.log("retUnwrapAvail    :: " + retUnwrapAvail);
          console.log("retUnwrapPref     :: " + retUnwrapPref);
          console.log("retRunCPLEX       ::\n" + JSON.stringify(retRunCPLEX));
          console.log("retRetrieveSpares ::\n" + JSON.stringify(retRetrieveSpares));
          // segregateSolution(retRunCPLEX,       curEvent, profession, EmpIdArray, EmpNameArray, "shift");
          segregateSolution(retRetrieveSpares, curEvent, profession, EmpIdArray, EmpNameArray, "spare");
          console.log("\n\n\nDone.\n\n\n");

        } else {
          console.log("Invoking Allocation Functions.");
          var fakeData         = makeFakeEmpData(IndiceJ);
          var fakeId           = fakeData["fakeId"];
          var fakeName         = fakeData["fakeName"];
          var fakeAvailability = fakeData["fakeAvailability"];
          var fakePreference   = fakeData["fakePreference"];
          EmpIdArray.push(fakeId);
          EmpNameArray.push(fakeName);
          IndiceI = EmpIdArray.length;

          var retDataSize    = cplexcpp.define_data_size(IndiceI, IndiceJ, IndiceK);
          var retTimeSection = cplexcpp.define_time_sections(Tk, Tk.length);
          var retBaseAmount  = cplexcpp.define_base_amount(MngDemandArray);
          console.log("retDataSize        :: " + retDataSize);
          console.log("retTimeSection     :: " + retTimeSection);
          console.log("retBaseAmount      :: " + retBaseAmount);

          for (var i = 0; i < (IndiceI - 1); i++) {
            var emp = EmpIdArray[i];
            EmpTimeArray.push(getEmpTime(curEvent, profession, emp));
            EmpAvailabilityArray = EmpAvailabilityArray.concat(EmpTimeArray[i]);
            EmpPrefArray.push(getPref(curEvent, profession, emp));
            EmpPreferenceArray   = EmpPreferenceArray.concat(EmpPrefArray[i]);
          }
          EmpAvailabilityArray   = EmpAvailabilityArray.concat(fakeAvailability);
          EmpPreferenceArray     = EmpPreferenceArray.concat(fakePreference);
          console.log("EmpAvailabilityArray  -> " + EmpAvailabilityArray + "; EmpAvailabilityArray Length = " + EmpAvailabilityArray.length);
          console.log("EmpPreferenceArray    -> " + EmpPreferenceArray   + "; EmpPreferenceArray Length   = " + EmpPreferenceArray.length);
          console.log("EmpNameArray          -> " + EmpNameArray);
          console.log("EmpIdArray            -> " + EmpIdArray);

          var retUnwrapBH       = cplexcpp.unwrap_business_hours(StoreBHArray);
          var retUnwrapAvail    = cplexcpp.unwrap_availability(EmpAvailabilityArray, EmpAvailabilityArray.length);
          var retUnwrapPref     = cplexcpp.unwrap_preference(EmpPreferenceArray, EmpPreferenceArray);
          var retRunCPLEX       = cplexcpp.run_cplex();
          var retRetrieveSpares = cplexcpp.retrieve_spares();
          console.log("retUnwrapBH          :: "  + retUnwrapBH);
          console.log("retUnwrapAvail       :: "  + retUnwrapAvail);
          console.log("retUnwrapPref        :: "  + retUnwrapPref);
          console.log("returnRunCPLEX       ::\n" + JSON.stringify(retRunCPLEX));
          console.log("returnRetrieveSpares ::\n" + JSON.stringify(retRetrieveSpares));
          // segregateSolution(retRunCPLEX, curEvent, profession, EmpIdArray, EmpNameArray, "shift");
          segregateSolution(retRetrieveSpares, curEvent, profession, EmpIdArray, EmpNameArray, "spare");
          console.log("\n\n\nDone.\n\n\n");
        }



///////////Without Allocation///////////

//   // console.log("It's Sufficient!!");
//       console.log("EmpIdArray          -> " + EmpIdArray);
//       console.log("EmpNameArray        -> " + EmpNameArray);
//   // IndiceI = EmpIdArray.length;
//   // console.log("IndiceJ             -> " + IndiceJ);
// //Send Indice to C++
//       var retDataSize    = cplexcpp.define_data_size(IndiceI, IndiceJ, IndiceK);
//       var retTimeSection = cplexcpp.define_time_sections(Tk, Tk.length);
//       var retBaseAmount  = cplexcpp.define_base_amount(MngDemandArray);
//       console.log("retDataSize         :: " + retDataSize);
//       console.log("retTimeSection      :: " + retTimeSection);
//       console.log("retBaseAmount       :: " + retBaseAmount);
//
//       for (var i = 0; i < EmpIdArray.length; i++) {
//         var emp = EmpIdArray[i];
//         EmpTimeArray.push(getEmpTime(curEvent, profession, emp));
//         EmpAvailabilityArray = EmpAvailabilityArray.concat(EmpTimeArray[i]);
//         EmpPrefArray.push(getPref(curEvent, profession, emp));
//         EmpPreferenceArray   = EmpPreferenceArray.concat(EmpPrefArray[i]);
//       }
//
//       console.log("EmpTimeArray         -> " + EmpTimeArray         + "; EmpTimeArray Length         = " + EmpTimeArray.length);
//       console.log("EmpAvailabilityArray -> " + EmpAvailabilityArray + "; EmpAvailabilityArray Length = " + EmpAvailabilityArray.length);
//       console.log("EmpPrefArray         -> " + EmpPrefArray         + "; EmpPrefArray Length         = " + EmpPrefArray.length);
//       console.log("EmpPreferenceArray   -> " + EmpPreferenceArray   + "; EmpPreferenceArray Length   = " + EmpPreferenceArray.length);
//       var retUnwrapBH       = cplexcpp.unwrap_business_hours(StoreBHArray);
//       var retUnwrapAvail    = cplexcpp.unwrap_availability(EmpAvailabilityArray, EmpAvailabilityArray.length);
//       var retUnwrapPref     = cplexcpp.unwrap_preference(EmpPreferenceArray, EmpPreferenceArray.length);
//       var retRunCPLEX       = cplexcpp.run_cplex();
//       var retRetrieveSpares = cplexcpp.retrieve_spares();
//       console.log("retUnwrapBH       :: " + retUnwrapBH);
//       console.log("retUnwrapAvail    :: " + retUnwrapAvail);
//       console.log("retUnwrapPref     :: " + retUnwrapPref);
//       console.log("retRunCPLEX       ::\n" + JSON.stringify(retRunCPLEX));
//       console.log("retRetrieveSpares ::\n" + JSON.stringify(retRetrieveSpares));
//       // segregateSolution(retRunCPLEX,       curEvent, profession, EmpIdArray, EmpNameArray, "shift");
//       // segregateSolution(retRetrieveSpares, curEvent, profession, EmpIdArray, EmpNameArray, "spare");
//       console.log("\n\n\nDone.\n\n\n");


      });
    } else {
      console.log("Scheduling Time Does not Match.");
      console.log("Wait Until CurrentEvent Occurs.");
    }
  });
}


function calcIndiceJ (TkArray) {
  var retVal = 0;
  for (var i = 0; i < TkArray.length; i++) {
    retVal += TkArray[i]
  }
  return retVal / 6;
}

function empIdToInt (inputArray) {//["102306111", "102306222", "102306333", "102306444"]
  var length = inputArray.length;
  var outputArray = [];
  for (var i = 0; i < length; i++) {
    var newId = parseInt(inputArray[i].substring(6, 9));
    outputArray.push(newId);
  }
  console.log("empIdToInt -> " + outputArray);
  return outputArray;
}

function dateHourToInt (endArray, startArray, token){//2016-10-10-09:00
  var EmpOutput = [];
  var MngOutput = [];

  if (token == "Mng") {
    var length = endArray.length;
    for (var i = 0; i < length; i++) {
      var endHour   = endArray[i].substring(11, 13);
      var startHour = startArray[i].substring(11, 13);
      var duration = parseInt(endHour) - parseInt(startHour);
      MngOutput.push(duration);
    }

    return MngOutput;

  } else if (token == "Emp") {

    var length = endArray.length;
    for (var i = 0; i < length; i++) {

      var endHour   = endArray[i].substring(11, 13);//hh
      var startHour = startArray[i].substring(11, 13);//hh

      var endMinute   = endArray[i].substring(14, 16);//mm
      var startMinute = startArray[i].substring(14, 16);//mm

      var endTime;
      var startTime;
      if (endMinute == "30" && startMinute == "30") {
        endTime   = (parseInt(endHour)   * 2) + 1;
        startTime = (parseInt(startHour) * 2) + 1;
      } else if (endMinute != "30" && startMinute != "30") {
        endTime   = parseInt(endHour)   * 2;
        startTime = parseInt(startHour) * 2;
      } else if (endMinute == "30" && startMinute != "30") {
        endTime   = (parseInt(endHour)  * 2) + 1;
        startTime = parseInt(startHour) * 2;
      } else if (endMinute != "30" && startMinute == "30") {
        endTime   = parseInt(endHour)    * 2;
        startTime = (parseInt(startHour) * 2) + 1;
      }

      EmpOutput.push(startTime);
      EmpOutput.push(endTime);

    }
    return EmpOutput;
  }
}

function setMngTimes (endArray, startArray) { // Extend the time info in between startTime and endTime
    //       index                0                 1                 2                 3
    //MngStartTimeArray = [yyyy-mm-d1-09:00, yyyy-mm-d1-16:00, yyyy-mm-d2-09:00, yyyy-mm-d2-16:00]
    //MngEndTimeArray   = [yyyy-mm-d1-16:00, yyyy-mm-d1-22:00, yyyy-mm-d2-16:00, yyyy-mm-d2-22:00]

    var durationArray      = dateHourToInt(endArray, startArray, "Mng");

    for (var i = 0; i < endArray.length; i++) {//0 ... 3
      var startPoint = moment(startArray[i], "YYYY-MM-DD-HH:mm");
      startSegmentsArray.push(startPoint.format("YYYY-MM-DD-HH:mm"));

      var duration = durationArray[i];//14 12 14 12
      var starter  = startPoint;

      for (var j = 1; j < duration; j++) {
        var startTime = starter.clone().add(1, 'H');
        startSegmentsArray.push(startTime.format("YYYY-MM-DD-HH:mm"));
        endSegmentsArray.push(startTime.format("YYYY-MM-DD-HH:mm"));
        starter = startTime;
      }
      var endPoint   = moment(endArray[i], "YYYY-MM-DD-HH:mm");
      endSegmentsArray.push(endPoint.format("YYYY-MM-DD-HH:mm"));
    }
    //
    console.log("startSegmentsArray = " + startSegmentsArray);
    console.log("endSegmentsArray   = " + endSegmentsArray);
}

function makeWeeklyArray () {
  var ThisWeekArray = [];
  var Monday    = moment(CurrentEvent, "YYYY-MM-DD").hours(0).minute(0);//2016-12-19
  var Tuesday   = Monday.clone().add(1, 'd');                           //2016-12-20
  var Wednesday = Tuesday.clone().add(1, 'd');                          //2016-12-21
  var Thursday  = Wednesday.clone().add(1, 'd');                        //2016-12-22
  var Friday    = Thursday.clone().add(1, 'd');                         //2016-12-23
  var Saturday  = Friday.clone().add(1, 'd');                           //2016-12-24
  var Sunday    = Saturday.clone().add(1, 'd');                         //2016-12-25

  ThisWeekArray.push(Monday.format("YYYY-MM-DD-HH:mm"));
  ThisWeekArray.push(Tuesday.format("YYYY-MM-DD-HH:mm"));
  // ThisWeekArray.push(Wednesday.format("YYYY-MM-DD-HH:mm"));
  ThisWeekArray.push(Thursday.format("YYYY-MM-DD-HH:mm"));
  ThisWeekArray.push(Friday.format("YYYY-MM-DD-HH:mm"));
  ThisWeekArray.push(Saturday.format("YYYY-MM-DD-HH:mm"));
  ThisWeekArray.push(Sunday.format("YYYY-MM-DD-HH:mm"));

  return ThisWeekArray;
}

function getEmpTime (cur, prof, emp) {
  var empSTArray = [];
  var empETArray = [];

  var output = [];
  employeeEventRef.child(CurrentStore).child(cur).child(prof).child(emp).on("child_added", function (snap) {
    var data = snap.val();
    empSTArray.push(data.StartDate);
    empETArray.push(data.EndDate);

  });
  empSTArray = makeEmpTimeArray(empSTArray);
  empETArray = makeEmpTimeArray(empETArray);

  var length = empSTArray.length;
  for (var i = 0; i < length * 2; i += 2) {
    var st = moment(empSTArray[i/2], "YYYY-MM-DD-HH:mm");
    var et = moment(empETArray[i/2], "YYYY-MM-DD-HH:mm");
    var diff = et.diff(st, 'hours');

  //30 minutes a unit
    // output[i]   = parseInt(st.format("HH")) * 2;
    // output[i+1] = parseInt(diff) * 2;
  //1 hour a unit
    output[i]   = parseInt(st.format("HH"));
    output[i+1] = parseInt(diff);

  }
  return output;
}

function makeEmpTimeArray (input) {
  var thisWeek    = makeWeeklyArray();
  var outputArray = [null, null, null, null, null, null];


  for (var i = 0; i < input.length; i++) {
    if (moment(input[i], "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[0], "YYYY-MM-DD-HH:mm"),moment(thisWeek[1], "YYYY-MM-DD-HH:mm"))) {
      outputArray[0] = input[i];
    } else if (moment(input[i], "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[1], "YYYY-MM-DD-HH:mm"), moment(thisWeek[2], "YYYY-MM-DD-HH:mm"))) {
      outputArray[1] = input[i];
    }
    // else if (moment(input[i], "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[2], "YYYY-MM-DD-HH:mm"), moment(thisWeek[3], "YYYY-MM-DD-HH:mm"))) {
    //   outputArray[2] = input[i];
    // }
    else if (moment(input[i], "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[2], "YYYY-MM-DD-HH:mm"), moment(thisWeek[3], "YYYY-MM-DD-HH:mm"))) {
      outputArray[2] = input[i];
    } else if (moment(input[i], "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[3], "YYYY-MM-DD-HH:mm"), moment(thisWeek[4], "YYYY-MM-DD-HH:mm"))) {
      outputArray[3] = input[i];
    } else if (moment(input[i], "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[4], "YYYY-MM-DD-HH:mm"), moment(thisWeek[5], "YYYY-MM-DD-HH:mm"))) {
      outputArray[4] = input[i];
    }
    else if (moment(input[i], "YYYY-MM-DD-HH:mm").isAfter(moment(thisWeek[5], "YYYY-MM-DD-HH:mm"))) {
      outputArray[5] = input[i];
    }
  }

  for (var i = 0; i < outputArray.length; i++) {
    if (outputArray[i] == null) {
      outputArray[i] = thisWeek[i];
    }
  }

  return outputArray;
}

function getPref (cur, prof, emp) {
  var thisWeek = makeWeeklyArray();
  var outputArray = [0, 0, 0, 0, 0, 0];

  employeeEventRef.child(CurrentStore).child(cur).child(prof).child(emp).on("child_added", function (snap) {
    var data = snap.val();
    if (moment(data.StartDate, "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[0], "YYYY-MM-DD-HH:mm"), moment(thisWeek[1], "YYYY-MM-DD-HH:mm"))) {
      outputArray[0] = parseInt(data.Preference);
    } else if (moment(data.StartDate, "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[1], "YYYY-MM-DD-HH:mm"), moment(thisWeek[2], "YYYY-MM-DD-HH:mm"))) {
      outputArray[1] = parseInt(data.Preference);
    } else if (moment(data.StartDate, "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[2], "YYYY-MM-DD-HH:mm"), moment(thisWeek[3], "YYYY-MM-DD-HH:mm"))) {
      outputArray[2] = parseInt(data.Preference);
    } else if (moment(data.StartDate, "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[3], "YYYY-MM-DD-HH:mm"), moment(thisWeek[4], "YYYY-MM-DD-HH:mm"))) {
      outputArray[3] = parseInt(data.Preference);
    } else if (moment(data.StartDate, "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[4], "YYYY-MM-DD-HH:mm"), moment(thisWeek[5], "YYYY-MM-DD-HH:mm"))) {
      outputArray[4] = parseInt(data.Preference);
    }
    // else if (moment(data.StartDate, "YYYY-MM-DD-HH:mm").isBetween(moment(thisWeek[5], "YYYY-MM-DD-HH:mm"), moment(thisWeek[6], "YYYY-MM-DD-HH:mm"))) {
    //   outputArray[5] = parseInt(data.Preference);
    // }
    else if (moment(data.StartDate, "YYYY-MM-DD-HH:mm").isAfter(moment(thisWeek[5], "YYYY-MM-DD-HH:mm"))) {
      outputArray[5] = parseInt(data.Preference);
    }
  });
  // console.log("Preference outputArray = " + outputArray);
  return outputArray;
}

function packShiftData (solution, currentEvent, profession, EmpId, EmpName, token) {

  var shiftRef = employeeShiftRef.child(CurrentStore).child(currentEvent);
  var spareRef = employeeSpareRef.child(CurrentStore).child(currentEvent);
  var mngRef   = managerShiftRef.child(CurrentStore).child('2018-07-07');

  var employeeShiftArray = [];
  var employeeSpareArray = [];
  var startTime = null;
  var endTime   = null;
  var length;

  var xJK  = solution.length;  //60
  var xK   = IndiceK;          //6
  var xJ   = xJK / xK;         //10
  // var counter = 0;

  console.log("When invoking profession---" + profession + "---, counterArray = " + counterArray);
  for (var day = 0; day < xK; day++) {//0 1 2 3 4 5 6

    var starter    = day * xJ;       // 0 10 20 30 40 50
    var terminator = starter + xJ;//10 20 30 40 50 60

    for (var i = starter; i < terminator; i++) {

      if (solution[i] == 1 && startTime == null) {//solution[0~3][0~10 || 10~20 || 20~30 || 30~40 || 40~50 || 50~60]
        startTime = startSegmentsArray[i];
      } else if (solution[i] == 0 && startTime != null) {
        endTime   = endSegmentsArray[(i-1)];
      } else if ((i == terminator - 1) && solution[i] == 1 && startTime != null) {
        endTime   = endSegmentsArray[i];
      }

      //Set All employeeShift and employeeSpare and managerShift
      if (startTime != null && endTime != null && token == 'shift') {
        employeeShiftArray.push({
          "startDate" : startTime,
          "endDate"   : endTime
        });
      //Set managerShift
        // var obj          = {"StartDate" : startTime, "EndDate" : endTime, "ID" : EmpId, "name" : EmpName};
        // var dayStr       = "Day" + (day+1);
        // var counter      = counterArray[day];
        // var endDateVal   = endSegmentsArray[terminator-1];
        // var startDateVal = startSegmentsArray[starter];
        // mngRef.child(dayStr).child(profession).child(counter).set(obj);
        // mngRef.child(dayStr).child('endDate').set(endDateVal);
        // mngRef.child(dayStr).child('startDate').set(startDateVal);
        //
        // counterArray[day]++;
        // startTime = null;
        // endTime   = null;
      //
      } else if (startTime != null && endTime != null & token == 'spare') {

        employeeSpareArray.push({
          "startDate" : startTime,
          "endDate"   : endTime
        });
        startTime = null;
        endTime   = null;

      }

    }

  }
  console.log("employeeShiftArray = " + JSON.stringify(employeeShiftArray));
  console.log("employeeSpareArray = " + JSON.stringify(employeeSpareArray));
  console.log("counterArray       = " + JSON.stringify(counterArray));


  if (token == "shift") {
    for (var i = 0; i < employeeShiftArray.length; i++) {
      shiftRef.child(EmpId).push(employeeShiftArray[i]);
      shiftRef.child("announcementSwitch").set(false);
    }
    mngRef.child("announcementSwitch").set(false);
    console.log("Sent employeeShift Data!");
    // packMngData(employeeShiftArray, currentEvent, profession, EmpId, EmpName);
  } else if (token == "spare") {
    for (var i = 0; i < employeeSpareArray.length; i++) {
      spareRef.child(profession).child(EmpId).push(employeeSpareArray[i]);
    }
    console.log("Sent employeeSpare Data!");
  }
}

function segregateSolution (solution, curEvent, profession, EmpIdArray, EmpNameArray, token) {
  var inputLength = parseInt(solution.length);          // i * j * k
  var empAmount   = parseInt(EmpIdArray.length);        // i
  var segLength   = inputLength / empAmount;  // j * k
  var outputShiftArray = [];              // i
  var outputSpareArray = [];

  console.log("{inputLength, empAmount, segLength} = { " + inputLength + ", " + empAmount + ", " + segLength + " }");

  if (token == 'shift') {

    for (var i = 0; i < inputLength; i += segLength) {
      var arr = [];
      var EmpId = EmpIdArray[i/segLength];
      var EmpName = EmpNameArray[i/segLength];

      for (var j = 0; j < segLength; j++) {
        arr.push(solution[j+i]);
      }
      packShiftData(arr, curEvent, profession, EmpId, EmpName, token);

    }
    counterArray = [0,0,0,0,0,0];

  } else if (token == 'spare') {

    for (var i = 0; i < inputLength; i += segLength) {
      var arr = [];
      var EmpId = EmpIdArray[i/segLength];
      var EmpName = EmpNameArray[i/segLength];

      for (var j = 0; j < segLength; j++) {
        arr.push(solution[j+i]);
      }
      packShiftData(arr, curEvent, profession, EmpId, EmpName, token);

    }

  }

}

function makeFakeEmpData (xJ) {
  // var outputObj = {'fakeName' : "", 'fakeId' : "", 'fakeAvailability': [], 'fakePreference': 3};
  var fakeId   = '999999999';
  var fakeName = 'e04';
  var fakeAvailability = [];
  var fakePreference   = [];

  var startTime = StoreBHArray[0];
  var endTime   = StoreBHArray[1];
  var duration  = endTime - startTime;
  var length    = IndiceK * 2;

  for (var i = 0; i < length; i += 2) {
    fakeAvailability[i]   = startTime;
    fakeAvailability[i+1] = duration;
  }

  for (var i = 0; i < IndiceK; i++) {
    fakePreference[i] = 3;
  }

  return {"fakeId" : fakeId, "fakeName" : fakeName, "fakeAvailability" : fakeAvailability, "fakePreference" : fakePreference};

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
