local map = Map("mqtt_subscriber")

FileUpload.unsafeupload = true

-- General Settings

local st_general = map:section(NamedSection, "general", "connectionsettings", "Connection settings")

local enable = st_general:option(Flag, "enable", "Enable")

local host_ip = st_general:option(Value, "ipAddress", "Host IP address", "IP address of the MQTT broker to connect to")
host_ip.datatype = "ip4addr";

local portnum = st_general:option(Value, "port", "Port number", "Port number for the specified MQTT brokers' IP address")
portnum.datatype = "port";

local username = st_general:option(Value, "username", "Username", "Username login credential for MQTT broker")
username.datatype = "string";

local password = st_general:option(Value, "password", "Password", "Password login credential for MQTT broker")
password.datatype = "string";

-- General TLS

local tls_enable = st_general:option(Flag, "tlsenable", "TLS enable")

local ca = st_general:option(FileUpload, "ca_file", translate("CA File"), translate("Upload CA .cert file"));
ca:depends("tlsenable", "1")

local cert = st_general:option(FileUpload, "cert_file", translate("CERT File"), translate("Upload Client's .cert file"));
cert:depends("tlsenable", "1")

local key = st_general:option(FileUpload, "key_file", translate("Key File"), translate("Upload Client's .key file"));
key:depends("tlsenable", "1")


-- View messages

local s = map:section(Table, messages, translate("Messages received"), translate(""))
s.anonymous = true
s.template = "message_table"
s.addremove = false
s.refresh = true
s.table_config = {
    truncatePager = true,
    labels = {
        placeholder = "Search...",
        perPage = "Messages per page {select}",
        noRows = "No entries found",
        info = ""
    },
    layout = {
        top = "<table><tr style='padding: 0 !important; border:none !important'><td style='display: flex !important; flex-direction: row'>{select}<span style='margin-left: auto; width:100px'>{search}</span></td></tr></table>",
        bottom = "{info}{pager}"
    }
}

o = s:option(DummyValue, "date", translate("Date"))
o = s:option(DummyValue, "topic", translate("Topic"))
o = s:option(DummyValue, "message", translate("Message"))

s:option(DummyValue, "", translate(""))


-- Add topics

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
