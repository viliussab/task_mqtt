local map = Map("mqtt_subscriber")

local st_topic = map:section(TypedSection, "mqttsub_topic", translate("Topics"), translate("") )
st_topic.addremove = true
st_topic.anonymous = true
st_topic.novaluetext = translate("There are no topics created yet.")

local topic = st_topic:option(Value, "name", translate("Topic name"), translate(""))
topic.datatype = "string"
topic.maxlength = 512
topic.placeholder = translate("Topic")
topic.rmempty = false
topic.parse = function(self, section, novld, ...)
	local value = self:formvalue(section)
	if value == nil or value == "" then
		self.map:error_msg(translate("Topic name can not be empty"))
		self.map.save = false
	end
	Value.parse(self, section, novld, ...)
end

local qos = st_topic:option(ListValue, "security_level", translate("QoS level"), translate("The publish/subscribe QoS level used for this topic"))
qos:value("0", "At most once (0)")
qos:value("1", "At least once (1)")
qos:value("2", "Exactly once (2)")
qos.rmempty=false
qos.default="0"

return map