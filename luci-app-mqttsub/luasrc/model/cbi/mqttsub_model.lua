local map = Map("mqtt_subscriber")

local certs = require "luci.model.certificate"
local s = map:section(NamedSection, "general", "mqtt_subscriber",  translate(""), translate(""))

enabled_pub = s:option(Flag, "enable", translate("Enable"), translate("Select to enable MQTT publisher"))

remote_addr = s:option(Value, "ipAddress", translate("Hostname"), translate("Specify address of the broker"))
remote_addr.placeholder  = "www.example.com"
remote_addr.datatype = "host"
remote_addr.parse = function(self, section, novld, ...)
	local enabled = luci.http.formvalue("cbid.mqtt_pub.mqtt_pub.enabled")
	local value = self:formvalue(section)
	if enabled and (value == nil or value == "") then
		self:add_error(section, "invalid", "Error: hostname is empty")
	end
	Value.parse(self, section, novld, ...)
end

remote_port = s:option(Value, "port", translate("Port"), translate("Specify port of the broker"))
remote_port.default = "1883"
remote_port.placeholder = "1883"
remote_port.datatype = "port"
remote_port.parse = function(self, section, novld, ...)
	local enabled = luci.http.formvalue("cbid.mqtt_pub.mqtt_pub.enabled")
	local value = self:formvalue(section)
	if enabled and (value == nil or value == "") then
		self:add_error(section, "invalid", "Error: port is empty")
	end
	Value.parse(self, section, novld, ...)
end

remote_username = s:option(Value, "username", translate("Username"), translate("Specify username of remote host"))
remote_username.datatype = "credentials_validate"
remote_username.placeholder = translate("Username")
remote_username.parse = function(self, section, novld, ...)
	local enabled = luci.http.formvalue("cbid.mqtt_pub.mqtt_pub.enabled")
	local pass = luci.http.formvalue("cbid.mqtt_pub.mqtt_pub.password")
	local value = self:formvalue(section)
	if enabled and pass ~= nil and pass ~= "" and (value == nil or value == "") then
		self:add_error(section, "invalid", "Error: username is empty but password is not")
	end
	Value.parse(self, section, novld, ...)
end

remote_password = s:option(Value, "password", translate("Password"), translate("Specify password of remote host. Allowed characters (a-zA-Z0-9!@#$%&*+-/=?^_`{|}~. )"))
remote_password.password = true
remote_password.datatype = "credentials_validate"
remote_password.placeholder = translate("Password")
remote_password.parse = function(self, section, novld, ...)
	local enabled = luci.http.formvalue("cbid.mqtt_pub.mqtt_pub.enabled")
	local user = luci.http.formvalue("cbid.mqtt_pub.mqtt_pub.username")
	local value = self:formvalue(section)
	if enabled and user ~= nil and user ~= "" and (value == nil or value == "") then
		self:add_error(section, "invalid", "Error: password is empty but username is not")
	end
	Value.parse(self, section, novld, ...)
end

FileUpload.size = "262144"
FileUpload.sizetext = translate("Selected file is too large, max 256 KiB")
FileUpload.sizetextempty = translate("Selected file is empty")
FileUpload.unsafeupload = true

tls_enabled = s:option(Flag, "tlsenable", translate("TLS"), translate("Select to enable TLS encryption"))

local cas = certs.get_ca_files().certs
local certificates = certs.get_certificates()
local keys = certs.get_keys()

tls_cafile = s:option(FileUpload, "ca_file", translate("CA file"), "")
tls_cafile:depends({tlsenable = "1"})

tls_certfile = s:option(FileUpload, "cert_file", translate("Certificate file"), "")
tls_certfile:depends({tlsenable = "1"})

tls_keyfile = s:option(FileUpload, "key_file", translate("Key file"), "")
tls_keyfile:depends({tlsenable = "1"})

function tls_cafile.write(self, section, value)
	map.uci:set(self.config, section, "ca_file", value)
end

tls_cafile.cfgvalue = function(self, section)
	return map.uci:get(map.config, section, "ca_file") or ""
end


function tls_certfile.write(self, section, value)
	map.uci:set(self.config, section, "cert_file", value)
end

tls_certfile.cfgvalue = function(self, section)
	return map.uci:get(map.config, section, "cert_file") or ""
end


function tls_keyfile.write(self, section, value)
	map.uci:set(self.config, section, "key_file", value)
end

tls_keyfile.cfgvalue = function(self, section)
	return map.uci:get(map.config, section, "key_file") or ""
end
-- local certs = require "luci.model.certificate"

-- FileUpload.size = "262144"
-- FileUpload.sizetext = translate("Selected file is too large, max 256 KiB")
-- FileUpload.sizetextempty = translate("Selected file is empty")
-- FileUpload.unsafeupload = true

-- -- General Settings

-- local st_general = map:section(NamedSection, "general", "connectionsettings", "Connection settings")

-- local enable = st_general:option(Flag, "enable", "Enable")

-- local host_ip = st_general:option(Value, "ipAddress", "Host IP address", "IP address of the MQTT broker to connect to")
-- host_ip.datatype = "host";

-- local portnum = st_general:option(Value, "port", "Port number", "Port number for the specified MQTT brokers' IP address")
-- portnum.datatype = "port";

-- local username = st_general:option(Value, "username", "Username", "Username login credential for MQTT broker")
-- username.datatype = "string";

-- local password = st_general:option(Value, "password", "Password", "Password login credential for MQTT broker")
-- password.datatype = "string";

-- -- General TLS

-- local tls_enable = st_general:option(Flag, "tlsenable", "TLS enable")

-- local cas = certs.get_ca_files().certs
-- local certificates = certs.get_certificates()
-- local keys = certs.get_keys()

-- tls_cafile = st_general:option(FileUpload, "cafile", translate("CA file"), "")
-- tls_cafile:depends({tlsenable = "1"})

-- tls_certfile = st_general:option(FileUpload, "certfile", translate("Certificate file"), "")
-- tls_certfile:depends({tlsenable = "1"})

-- tls_keyfile = st_general:option(FileUpload, "keyfile", translate("Key file"), "")
-- tls_keyfile:depends({tlsenable = "1"})


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
