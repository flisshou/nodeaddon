var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'CPLEX C++ node-gyp Node.js Express Firebase' });
});

module.exports = router;
