class pfc_connection_arduino:
	USB_PORT = "/dev/ttyACM0"
	BAUD_RATE = 19200
	SLAVE_ADDRESS = 0x04
	ACTUATOR_MES_TMPL = [
		("all",bool),
		("led",bool),
		("solution_a_pump",bool),
		("solution_b_pump",bool),
		("ph_minus_pump",bool),
		("ph_plus_pump",bool),
		("water_pump",bool),
		("air_pump",bool),
		("air_fan",bool),
		("ventil_fan",bool),
		("humidifier",bool),
		("heater",bool),
		("cold_cooler",bool),
		("dummy1",bool),
		("dummy2",bool),
		("dummy3",bool)
	]
	SENSOR_MES_TMPL = [


	]

	def __init(self):
		None
	def get_USB_PORT(self):
		return self.USB_PORT
	def get_BAUD_RATE(self):
		return self.BAUD_RATE
