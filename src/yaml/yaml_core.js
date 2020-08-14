import {create} from "../builder.js"
// 错误
export const ERRORS = [
    "SUCCESS",
    "UNEXPECTED_TOKEN",
    "UNSUPPORTED_TYPE",
]
// 类型
export const TYPES = [
    "UNKNOWN", // 未知类型，需要额外检测
    "COMMENT",
    "FIELD_BARE",
    "FIELD_QUOTED",
    "FIELD_DOTTED", // Can include BARE / QUOTED KEY s
    "VALUE_STRING_BASIC",
    "VALUE_STRING_BASIC_MULTI",
    "VALUE_STRING_LITERAL",
    "VALUE_STRING_LITERAL_MULTI",
    "VALUE_INTEGER",
    "VALUE_FLOAT", // inf nan
    "VALUE_BOOLEAN", 
    "VALUE_OFFSET_DATE_TIME",
    "VALUE_LOCAL_DATE_TIME",
    "VALUE_LOCAL_DATE",
    "VALUE_LOCAL_TIME",
    "VALUE_ARRAY", // Can include any value of any type
    "VALUE_TALBE",
    "VALUE_TABLE_INLINE",
    "VALUE_ARRAY_TABLE",
]
const builder = create({
    name: "yaml_parser",
    errors: ERRORS,
})
export default builder

const {ERROR, NODE, SPAN, ON_MATCH} = builder

NODE("comment")
    .match("\n", NODE("before_field"))
    .skipTo(NODE("comment"))

// 部分 field 可以决策实际的 value 类型
NODE("before_field")
    .match([" ", "\t", "\r", "\n"], NODE("before_field"))
    .match("#", NODE("comment") )
    .match('"', SPAN("on_field").start( NODE("field_quoted") ))
    // .match("[")
    // 空 field 是不允许的
    .match([".", "]", "=", ":"], ERROR("UNEXPECTED_TOKEN", "before_field"))
    .otherwise( SPAN("on_field").start( NODE("field_basic")  ))

// Bare Key 基础 KEY 字段
NODE("field_basic")
    .peek([" ", "\t", ".", "]"], SPAN("on_field").end( ON_MATCH("on_field_emit") ))
    .peek(["=", ":"], SPAN("on_field").end(NODE("before_value")))
    .peek(["\r", "\n"], ERROR("UNEXPECTED_TOKEN", "field_basic"))
    .skipTo(NODE("field_basic"))

// Quoted Keys 使用引号包裹额 KEY 字段
NODE("field_quoted")
    .peek("\\", NODE("filed_quoted_escape_1"))
    .peek('"', SPAN("on_field").end( ON_MATCH("on_field_emit") ))
    .skipTo( NODE("field_quoted") )

NODE("filed_quoted_escape_1") // 转义符号 \
    .skipTo(NODE("filed_quoted_escape_2"))

NODE("filed_quoted_escape_2") // 转义后首个字符
    .skipTo(NODE("field_quoted"))

// 在数据结束时调用，用于可能多级 KEY 的场景
ON_MATCH("on_field_emit")
    .otherwise(NODE("after_field"))

// 单个 field 结束后
NODE("after_field")
    .match(".", NODE("before_field"))
    .match(["=", ":"], NODE("before_value"))
    // TODO 其他类型支持
    // .match("]", SPAN("on_value").start( NODE("value_table_basic") ))
    .match(["\r", "\n"], ERROR("UNEXPECTED_TOKEN", "after_field"))
    .skipTo( NODE("after_field") )

NODE("before_value")
    .match([" ", "\t"], NODE("before_value"))
    // 部分数据起始标志符号可以决策实际的 value 类型
    .peek('"', NODE("before_value_string_basic"))
    // .match("{", SPAN("on_value").start(NODE("value_table_inline")))
    // .match("[", SPAN("on_value").start(NODE("value_array")))
    .otherwise(ERROR("UNSUPPORTED_TYPE")) // TODO 暂未支持其他类型

// 在数据结束时调用，用于实际解析本段数据
ON_MATCH("on_value_emit")
    .otherwise( NODE("after_value") )

NODE("after_value")
    .otherwise(NODE("before_field"))

