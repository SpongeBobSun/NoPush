/// <reference path="../typings/node/node.d.ts" />
/// <reference path="../typings/express/express.d.ts" />

var express = require('express');
var router = express.Router();

var msgSender = require('../socket').MsgSender;

router.get('/',function(req,res,next){
	res.render('message');
});

router.post('/',function(req,res,next){
	console.log('Push request :'+req.body);
	msgSender(JSON.stringify({
		notification: req.body.content
	}));
	res.render('message');
});
module.exports = router;