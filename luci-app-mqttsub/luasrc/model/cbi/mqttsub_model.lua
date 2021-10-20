map = Map("mqtt_subscriber")

section = map:section(NamedSection, "general", "connectionsettings", "Connection settings")

enable = section:option(Flag, "enable", "Enable")

host_ip = section:option(Value, "ipAddress", "Host IP address", "IP address of the MQTT broker to connect to")
host_ip.datatype = "ip4addr";

portnum = section:option(Value, "port", "Port number", "Port number for the specified MQTT brokers' IP address")
portnum.datatype = "port";

topicsection = map:section(NamedSection, "topics", "subscribedtopics", "Subscribed topics")

topics = topicsection:option(DynamicList, "topic", "Subscribed topics", "Topics that the subscriber will listen to for information")
topics.datatype = "string"

return map
