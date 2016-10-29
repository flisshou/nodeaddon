var express = require('express');
var router = express.Router();
var firebase = require('firebase');

firebase.initializeApp({
  serviceAccount: "./cplexcpp-serviceaccountkey.json",
  databaseURL: "https://tryfirebase-746a9.firebaseio.com/",
});

var db = firebase.database();
var ref = db.ref("testdata/cplex/users");
var managerRef = db.ref("testdata/cplex/users/0/cplex_managers_parameters");

var firebaseData = {};
var managerData = {};

ref.on("value",  function(snapshot) {
  firebaseData = snapshot.val();
}, function (errorObject) {
  console.log("The read failed: " + errorObject.code);
});

managerRef.on("value", function(snapshot) {
  managerData = snapshot.val();
}, function (errorObject) {
  console.log("The read failed: " + errorObject.code);
});

/* GET /api/v1/data.*/
router.get('/testdata/users', function(req, res) {
  res.send(firebaseData);
});

/* GET /api/v1/data/../cplex_managers_parameters */
router.get('/testdata/users/cplex_managers_parameters', function(req, res) {
  res.send(managerData);
});

/* POST /api/v1/testdata/cplex/users*/
router.post('/testdata/users/cplex_managers_parameters', function(req, res) {
  if(!req.body) return res.sendStatus(400);
  for(var key in req.body) {
    managerRef.child(key).set(req.body[key], function(error) {
      if (error) {
        console.log("Data could not be saved. " + error);
      } else {
        console.log("Data saved Successfully");
      }
    });
  } res.sendStatus(200);
});

module.exports = router;
