module("luci.controller.mqttsub_controller", package.seeall)

function index()

	entry({"admin", "services", "mqttsub", "subscriber"}, cbi("mqttsub_model"), ("Subscriber"), 120).leaf = true
	entry({"admin", "services", "mqttsub"}, cbi("mqttsub_model"), ("MQTT Subscriber"), 120)

end
