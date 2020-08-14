import builder from "./yaml_core.js"
const {ERROR, NODE, SPAN, ON_MATCH} = builder

NODE("before_value_string_basic")
    .match('"""', SPAN("on_value").start(NODE("value_string_basic_multi")))
    .otherwise( SPAN("on_value").start(NODE("value_string_basic")) )

NODE("value_string_basic")
    .peek("\\", NODE("value_string_basic_escape_1"))
    .peek('"', SPAN("on_value").end(NODE("after_value_string_basic")))
    .skipTo(NODE("value_string_basic"))

NODE("value_string_basic_escape_1") // 转移符号 \
    .skipTo(NODE("value_string_basic_escape_2"))

NODE("value_string_basic_escape_2") // 转移符号后 首个字符
    .skipTo(NODE("value_string_basic"))

NODE("after_value_string_basic")
    .match('"', ON_MATCH("on_value_emit"))
    .otherwise( ERROR("UNEXPECTED_TOKEN", "after_value_string_basic") )

NODE("value_string_basic_multi")
    .peek("\\", NODE("value_string_basic_multi_escape_1"))
    .peek('"', SPAN("on_value").end(NODE("after_value_string_basic_multi")))
    .skipTo(NODE("value_string_basic_multi"))

NODE("after_value_string_basic_multi")
    .match('"""', ON_MATCH("on_value_emit"))
    // 虚假的结束，需要重新提交进入前猜测的引号
    .otherwise( SPAN("on_value").start(NODE("value_string_basic_multi_quote")) )

NODE("value_string_basic_multi_quote")
    .skipTo(NODE("value_string_basic_multi"))

NODE("value_string_basic_multi_escape_1") // 转移符号 \
    .skipTo(NODE("value_string_basic_multi_escape_2"))

NODE("value_string_basic_multi_escape_2") // 转移符号后 首个字符
    .skipTo(NODE("value_string_basic_multi"))

