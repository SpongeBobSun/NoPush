/// <reference path="../typings/sequelize/sequelize.d.ts" />

var db_config = require('../config/db_config.json');
var Sequelize = require('sequelize');
var dbConnection = new Sequelize(db_config.database,db_config.username,db_config.password,db_config.option);

var appidBean = dbConnection.define('appGroupIds',
	{
		id:	Sequelize.INTEGER,
		appid:	Sequelize.STRING,
		group:	Sequelize.STRING
	},
	{
		tableName: 'appGroupIds',
		timestamps: false
	});

var addAppId = function(id, group){
	appidBean.create({
		appid:	id,
		group:	group
	}).done(function(err, result){
		
	});
};

module.exports.add = addAppId;