# -*- coding: utf-8 -*-
"""
Database Schema DDL(Data Definition Language)
Table List
1. PFC_model
2. model_meta
3. system_status
4. order
5. order_result
6. participant
7. participant_meta
8. actuator
9. actuator_activity
10. actuator_meta
11. sensor
12. sensor_meta
13. sensor_data
14. sensor_data_meta
15. image
16. image_process
"""
import time
import sys
import os
import MySQLdb


sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from configure import pfc_conf

class DB_DDL:
	TABLE_LIST=(
		'PFC_model',
		'model_meta',
		'system_status',
		'actuator',
		'actuator_meta',
		'participant',
		'participant_meta',
		'order',
		'order_result',
		'actuator_activity',
		'sensor',
		'sensor_meta',
		'sensor_data',
		'sensor_data_meta',
		'image',
		'image_process'
	)
	DB = None
	DBC = None
	def __init__(
			self,
			model_id=None,
			mac_addr=None,
			db_host=None,
			db_port=None,
			db_id=None,
			db_pw=None,
			db_name=None
		):
		self.DB = MySQLdb.connect(host=db_host, user=db_id, passwd=db_pw, db=db_name, port=db_port, use_unicode=True, charset='utf8')
		self.DBC = self.DB.cursor()
		print("Connection Info : " + str(self.DB))
		print("Connection Cusor : " + str(self.DBC))
	def create_PFC_model(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `PFC_model`
(
	model_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
	mac_address VARCHAR(255),
	PRIMARY KEY(model_id)
)
		"""
		return create_sql
	def create_model_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `model_meta`
(
	meta_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	model_id INT(10) UNSIGNED NOT NULL,
	meta_key 	VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
		return create_sql
	def create_system_status(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `system_status`
(
	status_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	model_id INT(10) UNSIGNED NOT NULL,
	cpu_usage INT(10),
	memory_usage INT(15),
	momory_avail INT(15),
	cron_status VARCHAR(255),
	ros_avail VARCHAR(255),
	brain_status VARCHAR(255),
	datetime DATETIME NOT NULL DEFAULT 0,
	PRIMARY KEY(status_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
		return create_sql
	def create_order(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `order`
(
	order_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	model_id INT(10) UNSIGNED NOT NULL,
	order_message VARCHAR(500) NOT NULL,
	order_type VARCHAR(255),
	participant_id INT(10) UNSIGNED,
	datetime DATETIME DEFAULT 0,
	PRIMARY KEY(order_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id),
	FOREIGN KEY(participant_id) REFERENCES participant(participant_id)
)
		"""
		return create_sql
	def create_order_result(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `order_result`
(
	order_id INT(15) UNSIGNED NOT NULL,
	result VARCHAR(255),
	datetime DATETIME DEFAULT 0,
	FOREIGN KEY(order_id) REFERENCES `order`(order_id)
)
		"""
		return create_sql
	def create_participant(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `participant`
(
	participant_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
	type VARCHAR(255),
	PRIMARY KEY(participant_id)
)
		"""
		return create_sql
	def create_participant_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `participant_meta`
(
	meta_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	participant_id INT(10) UNSIGNED NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255) NOT NULL,
	PRIMARY KEY(meta_id),
	FOREIGN KEY(participant_id) REFERENCES participant(participant_id)
)
		"""
		return create_sql
	def create_actuator(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `actuator`
(
	actuator_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	model_id INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY(actuator_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
		return create_sql
	def create_actuator_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `actuator_meta`
(
	meta_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	actuator_id INT(15) UNSIGNED NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(actuator_id) REFERENCES actuator(actuator_id)
)
		"""
		return create_sql
	def create_actuator_activity(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `actuator_activity`
(
	activity_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	actuator_id INT(15) UNSIGNED NOT NULL,
	order_id INT(15) UNSIGNED NOT NULL,
	status VARCHAR(255),
	datetime DATETIME NOT NULL DEFAULT 0,
	PRIMARY KEY(activity_id),
	FOREIGN KEY(actuator_id) REFERENCES actuator(actuator_id),
	FOREIGN KEY(order_id) REFERENCES `order`(order_id)
)
		"""
		return create_sql
	def create_sensor(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor`
(
	sensor_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	model_id INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY(sensor_id),
	FOREIGN KEY(model_id) REFERENCES PFC_model(model_id)
)
		"""
		return create_sql
	def create_sensor_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor_meta`
(
	meta_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	sensor_id INT(15) UNSIGNED NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(sensor_id) REFERENCES sensor(sensor_id)
)
		"""
		return create_sql
	def create_sensor_data(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor_data`
(
	data_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	sensor_id INT(15) UNSIGNED NOT NULL,
	order_id INT(15) UNSIGNED NOT NULL,
	data_key VARCHAR(255) NOT NULL,
	data_value VARCHAR(255),
	datetime DATETIME NOT NULL DEFAULT 0,
	PRIMARY KEY(data_id),
	FOREIGN KEY(sensor_id) REFERENCES sensor(sensor_id),
	FOREIGN KEY(order_id) REFERENCES `order`(order_id)
)
		"""
		return create_sql
	def create_sensor_data_meta(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `sensor_data_meta`
(
	meta_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	data_id INT(15) UNSIGNED NOT NULL,
	meta_key VARCHAR(255) NOT NULL,
	meta_value VARCHAR(255),
	PRIMARY KEY(meta_id),
	FOREIGN KEY(data_id) REFERENCES sensor_data(data_id)
)
		"""
		return create_sql
	def create_image(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `image`
(
	image_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	sensor_id INT(15) UNSIGNED NOT NULL,
	file_path VARCHAR(255) NOT NULL,
	datetime DATETIME NOT NULL DEFAULT 0,
	PRIMARY KEY(image_id),
	FOREIGN KEY(sensor_id) REFERENCES sensor(sensor_id)
)
		"""
		return create_sql
	def create_image_process(self):
		create_sql="""
CREATE TABLE IF NOT EXISTS `image_process`
(
	process_image_id INT(15) UNSIGNED NOT NULL AUTO_INCREMENT,
	image_id INT(15) UNSIGNED NOT NULL,
	process_type VARCHAR(55) NOT NULL,
	file_path VARCHAR(255) NOT NULL,
	datetime DATETIME NOT NULL DEFAULT 0,
	PRIMARY KEY(process_image_id),
	FOREIGN KEY(image_id) REFERENCES image(image_id)
)
		"""
		return create_sql
	def check_exists_table(self):
		None
	def create_all_db(self):
		None

	def create_dummy_db(self):
		for table_name in self.TABLE_LIST:
			class_method_to_call = getattr(self, 'create_' + table_name)
			create_sql = class_method_to_call()
			print(create_sql)
			res = self.DBC.execute(create_sql)
			print(res)
			print('\n\n')






if __name__ == '__main__':
	PFC_MODEL_ID = pfc_conf.PFC_MODEL_ID
	PFC_MAC_ADDR = pfc_conf.PFC_MAC_ADDR
	PFC_DB_HOST = pfc_conf.PFC_DB_HOST
	PFC_DB_PORT = pfc_conf.PFC_DB_PORT
	PFC_DB_ID = pfc_conf.PFC_DB_ID
	PFC_DB_PW = pfc_conf.PFC_DB_PW
	PFC_DB_NAME = pfc_conf.PFC_DB_NAME

	DB_DDL = DB_DDL(
										model_id=PFC_MODEL_ID,
										mac_addr=PFC_MAC_ADDR,
										db_host=PFC_DB_HOST,
										db_port=PFC_DB_PORT,
										db_id =PFC_DB_ID,
										db_pw =PFC_DB_PW,
										db_name =PFC_DB_NAME
									)

	DB_DDL.create_dummy_db()
	DB_DDL.check_exists_table()
	DB_DDL.create_all_db()


















