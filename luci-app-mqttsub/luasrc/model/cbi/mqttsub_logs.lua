map = SimpleForm("system")
map.submit = false
map.reset = false

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

return map