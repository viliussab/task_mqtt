local map = Map("mqtt_subscriber")
local ds = require "luci.dispatcher"

arg[1] = arg[1] or ""

local eventsection = map:section(TypedSection, "mqttsub_event", translate("Event creation"), translate("") )
eventsection.addremove = true
eventsection.anonymous = true
eventsection.novaluetext = translate("There are no events created yet.")

-- local topic = eventsection:option(Value, "topic", translate("Topic name"), translate(""))
-- topic.datatype = "string"
-- topic.maxlength = 512
-- topic.placeholder = translate("Topic")
-- topic.rmempty = false
-- topic.parse = function(self, section, novld, ...)
-- 	local value = self:formvalue(section)
-- 	if value == nil or value == "" then
-- 		self.map:error_msg(translate("Topic name can not be empty"))
-- 		self.map.save = false
-- 	end
-- 	Value.parse(self, section, novld, ...)
-- end

local topic = eventsection:option(ListValue, "topic", translate("Select topic"), translate(""))
map.uci:foreach("mqtt_subscriber", "mqttsub_topic",
	function(i)
		topic:value(i.name, i.name)
	end)

local key = eventsection:option(Value, "key", translate("Key"), translate("Specify the key of the value"))
key.datatype = "string"
key.maxlength = 512
key.placeholder = translate("key")
key.rmempty = false
key.parse = function(self, section, novld, ...)
	local value = self:formvalue(section)
	if value == nil or value == "" then
		self.map:error_msg(translate("Key value can not be empty"))
		self.map.save = false
	end
	Value.parse(self, section, novld, ...)
end

local data_type = eventsection:option(ListValue, "data_type", translate("Value type"), translate(""))

data_type:value("string", translate("String"))
data_type:value("number", translate("Number"))

local value_str = eventsection:option(Value, "value", translate("Value"), translate("Specify the value"))
value_str.datatype = "string"
value_str.maxlength = 512
value_str.placeholder = translate("value")
value_str.rmempty = false

local operator_type = eventsection:option(ListValue, "operator", translate("Comparison type"), translate(""))

operator_type:value("1", translate("Equals to the provided value"))
operator_type:value("2", translate("Is not equal to the provided value"))
operator_type:value("3", translate("Is more than the provided value"))
operator_type:value("4", translate("Is more than or equal to the provided value"))
operator_type:value("5", translate("Is less than the provided value"))
operator_type:value("6", translate("Is less than or equal to the provided value"))
operator_type.default = 0

local sender_conf = eventsection:option(ListValue, "sender_conf", translate("Select sender email"), translate("Select sender email configuration provided in System -> Administration -> Recipients"))
map.uci:foreach("user_groups", "email",
	function(i)
		if i.password ~= nil and i.password ~= "" then
			sender_conf:value(i.senderemail, i.name)
		end
	end)

receiver_mail = eventsection:option(Value, "receiver_mail", "Recipient's email address", "For whom you want to send an email to about the event occuring.")
receiver_mail.datatype = "email"
receiver_mail.placeholder = "mail@domain.com"
receiver_mail.rmempty = false

return map