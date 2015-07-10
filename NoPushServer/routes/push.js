/// <reference path="../typings/node/node.d.ts" />
/// <reference path="../typings/express/express.d.ts" />
/// <reference path="../socket.js" />

var express = require('express');
var router = express.Router();

var msgSender = require('../socket').MsgSender;

router.post('/',function(req,res,next){
	if (req.body.notification == null && req.body.message == null){
		res.end(JSON.stringify({
			errorCode: false,
			errorText: 'Notification and Message must not all be null!'
		}));
		return;
	}
	msgSender(JSON.stringify({
		notification: req.body.notification,
		message: req.body.message,
		icon: req.body.icon
	}));
	res.end(JSON.stringify({
		errorCode: true,
		errorText: 'Cool!'
	}));
});

module.exports = router;

