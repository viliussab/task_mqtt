module("luci.controller.mqttsub_controller", package.seeall)

function index()

	entry({"admin", "services", "mqtt", "subscriber"}, firstchild(), ("Subscriber"), 120)
	entry({"admin", "services", "mqtt", "subscriber", "generalsettings"}, cbi("mqttsub_model"), _("General Settings"), 1)
	entry({"admin", "services", "mqtt", "subscriber", "messages"}, form("mqttsub_logs"), _("Messages Received"), 2)
	entry({"admin", "services", "mqtt", "subscriber", "topics"}, cbi("mqttsub_topics"), _("Subscribed Topics"), 3)
	entry({"admin", "services", "mqtt", "subscriber", "eventsending"}, cbi("mqttsub_events"), ("Event listening"), 4)
end
