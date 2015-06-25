/// <reference path="../typings/node/node.d.ts" />
/// <reference path="../typings/express/express.d.ts" />
/// <reference path="../typings/node-uuid/node-uuid.d.ts" />

var express = require('express');
var router = express.Router();
var idGenerator = require('node-uuid');
var addIdBean = require('../model/appid').add;

router.get('/',function(req,res,next){
	res.render('appid',{message: ' '});
});

router.post('/',function(req,res,next){
	var appid = idGenerator.v4();
	console.log(req.body.grpName);
	addIdBean(appid,req.body.grpName);
	res.render('appid',{message: 'Your app id is:'+appid});
});

module.exports = router;