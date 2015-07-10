/// <reference path="../typings/sequelize/sequelize.d.ts" />

var db_config = require('../config/db_config.json');
var Sequelize = require('sequelize');
var dbConnection = new Sequelize(db_config.database,db_config.username,db_config.password,db_config.option);

var statusBean = dbConnection.define('deviceStatus',{
	id:	Sequelize.INTEGER,
	appid:	Sequelize.STRING,
	last_online:	Sequelize.DATE(14)
	},
	{
		tableName: 'deviceStatus',
		timestamps: false
	}
);

var updateStatus = function(id){
	statusBean.findOrCreate({
		where: {
			id:id
		}
	}).ok(function(result){
		//TODO
		//EOD TAG
		statusBean.update({lastonline:Sequelize.fn('current_timestamp')},{where:{id:id}});
	});
};