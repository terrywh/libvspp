
import { promises as fs } from 'fs'

import llparse from 'llparse'
import edge from 'llparse-builder/lib/edge.js'
const { Edge } = edge

const __build = Symbol("build")

function append(c, i) {
    if(typeof i === "number") i = "".padEnd(i*4, " ")
    else if(typeof i !== "string") i = ""
    if(typeof c === "object") c = c[__build] ? c[__build]() : c.toString()
    
    return c.split("\n").map((v) => `${i}${v}`).join("\n")
}
/**
 * 
 * @param {{name: string, errors: Array.<string>, props: Object.<string, llparse.source.PropertyType>}} options 
 */
export function create(options) {
    if(options.errors instanceof Array) 
        options.errors = Object.fromEntries(options.errors.map(function(v, i) {
            return [options.name.toUpperCase() + "_ERROR_" + v.toUpperCase(), i > 0 ? 200 + i : i]
        }))
    
    const __parser = new llparse.LLParse(options.name)
    const __parser_nodes = {}

    // 设置属性
    if(options.props) {
        for(let name in options.props) __parser.property(options.props[name], name)
    }

    return {
        /**
         * 返回错误名称对应的错误码
         * @return {Number}
         */
        ERROR_CODE(key) {
            key = options.name.toUpperCase() + "_ERROR_" + key.toUpperCase()
            if(!options.errors[key])
                throw new Error("undefined error '" + key + "'")
            return options.errors[key]
        },
        CALLBACK(key) {
            return options.name + "_" + key
        },
        /**
         * 返回指定名称的节点
         * @return {llparse.source.node.Node}
         */
        NODE(key) {
            key = options.name + "_" + key
            let x = __parser_nodes[key]
            if(!x)
                x = __parser_nodes[key] = __parser.node(key)
            return x
        },
        /**
         * 构建并返回一个错误节点
         * @return {llparse.source.node.Error}
         */
        ERROR(key, reason) {
            key = options.name.toUpperCase() + "_ERROR_" + key.toUpperCase()
            if(reason === undefined) reason = key
            if(!options.errors[key])
                throw new Error("undefined error '" + key + "'")
            return __parser.error(options.errors[key], reason)
        },
        /**
         * @return {llparse.source.node.Consume}
         */
        CONSUME(key) {
            key = options.name + "_" + key
            let x = __parser_nodes[key]
            if(!x) {
                x = __parser_nodes[key] = __parser.consume("x")
                x.consume = function(prop) {
                    x.name = "consume_" + prop
                    x.field = prop
                    return x
                }
            }
            return x
        },
        /**
         * @return {llparse.source.Span}
         */
        SPAN(key) {
            key = options.name + "_" + key
            let x = __parser_nodes[key]
            if(!x)
                x = __parser_nodes[key] = __parser.span(__parser.code.span(key))
            return x
        },
        /**
         * @return {llparse.source.node.Invoke}
         */
        ON_VALUE(key) {
            key = options.name + "_" + key
            let x = __parser_nodes[key]
            if(!x) {
                x = __parser_nodes[key] = __parser.invoke(__parser.code.value(key))
                x.match = function(status, next) {
                    const key = parseInt(status, 10)
                    x.addEdge(new Edge(next, true, key, undefined))
                    return x
                }
            }
            return x
        },
        /**
         * @return {llparse.source.node.Invoke}
         */
        ON_MATCH(key) {
            key = options.name + "_" + key
            let x = __parser_nodes[key]
            if(!x) {
                x = __parser_nodes[key] = __parser.invoke(__parser.code.match(key))
                x.match = function(status, next) {
                    const key = parseInt(status, 10)
                    x.addEdge(new Edge(next, true, key, undefined))
                    return x
                }
            }
            return x
        },
        /**
         * @return {llparse.source.node.Invoke}
         */
        ON_LOAD(key) {
            key = options.name + "_" + key
            let x = __parser_nodes[key]
            if(!x) {
                x = __parser_nodes[key] = __parser.invoke(__parser.code.load('x'))
                x.load = function(prop) {
                    x.code.field = prop
                    x.code.name = "load_" + prop
                    
                    x.name = "invoke_" + x.code.name
                    return x
                }
                x.match = function(status, next) {
                    const key = parseInt(status, 10)
                    x.addEdge(new Edge(next, true, key, undefined))
                    return x
                }
            }
            return x
        },
        /**
         * @return {llparse.source.node.Invoke}
         */
        ON_UPDATE(key) {
            key = options.name + "_" + key
            let x = __parser_nodes[key]
            if(!x) {
                x = __parser_nodes[key] = __parser.invoke(__parser.code.update("x", 0))
                x.update = function(prop, v) {
                    x.code.field = prop
                    x.code.value = v
                    x.code.name = "update_" + prop

                    x.name = "invoke_" + x.code.name
                    return x
                }
            }
            return x
        },
        /**
         * 用于生成头文件
         */
        header(filename) {
            let content = `#ifndef PARSER_${filename.toUpperCase()}_H
#define PARSER_${filename.toUpperCase()}_H\n\n`, 
                __built = false

            return {
                append(c, i) {
                    content += append(c, i)
                    return this
                },
                async output(__gen) {
                    if(!__built) {
                        __built = true
                        content += `\n#endif // PARSER_${filename.toUpperCase()}_H\n`
                    }
                    await fs.writeFile(__gen + "/" + filename + ".h", content)
                }
            }
        },
        /**
         * 声明、定义 枚举 
         */
        enumeration(name, base) {
            if(base === undefined) base = ""
            else base = `: ${base} `
            const __enumeration = {}
            let   __start = 0
            let   __strong = ""
            let   __comment = ""
            return {
                comment(c) {
                    __comment += c
                    return this
                },
                strong() {
                    __strong = "class"
                    return this
                },
                append(field, v) {
                    if(v === undefined) v = __start++
                    __enumeration[field] = v
                    return this
                },
                fromObject(o) {
                    Object.assign(__enumeration, o)
                    return this
                },
                fromArray(a, start) {
                    if(start !== undefined) __start = start
                    a = Object.fromEntries(a.map((v) => [v, __start++]));
                    this.fromObject(a)
                    return this
                },
                [__build]() {
                    let content = `\n// ${__comment}\ntypedef enum ${__strong} ${name}_e ${base}{\n`;
                    for(let field in __enumeration) {
                        content += `    ${name.toUpperCase()}_${field} = ${__enumeration[field]},\n`
                    }
                    content += `} ${name}_t;\n`
                    return content
                }
            }
        },
        /**
         * 声明、定义结构体
         */
        struct(name, base) {
            if(base === undefined) base = ""
            else base = `: public ${base} `

            const __struct = {}
            const __comment = ""
            return {
                comment(c) {
                    __comment += c
                    return this
                },
                append(field, type) {
                    __struct[field] = type
                    return this
                },
                fromObject(o) {
                    Object.assign(__struct, o)
                    return this
                },
                [__build]() {
                    
                    let content = `\n// ${__coment}\ntypedef struct ${name}_s ${base}{\n`
                    for(let field in __enumeration) {
                        content += `    ${__enumeration[field]} ${field};\n`
                    }
                    content += `} ${name}_t;\n`
                    return content
                }
            }
        },
        /**
         * 用于生成 源代码
         */
        source(filename) {
            let content = "", src = {
                append(c, i) {
                    content += append(c, i)
                    return src
                },
                async output(__gen) {
                    await fs.writeFile(__gen + "/" + filename + ".c", content)
                }
            }
            return src
        },
        /**
         * 文件包含
         */
        include(filepath) {
            let __sys = false
            return {
                system() {
                    __sys = true
                    return this
                },
                [__build]() {
                    return __sys ? `#include <${filepath}>\n` : `#include "${filepath}"\n`
                }
            }
        },
        /**
         * 
         * @param {*} key 
         */
        callback(key) {
            let __comment = ""
            let __content = ""
            return {
                comment(c) {
                    __comment += c
                    return this
                },
                body(c) {
                    __content += c
                    return this
                },
                [__build]() {
                    if(__content == "") __content = ";\n"
                    else __content = ` {\n${__content}\n}\n`
                    return `// ${__comment}\nint ${options.name}_${key}(${options.name}_t* s, const unsigned char* p, const unsigned char* endp)${__content}`
                }
            }
        },
        //
        parser(entry_name) {
            let key = options.name + "_" + entry_name
            return {
                async output(__src) {
                    const art = __parser.build(__parser_nodes[key], {"c": {
                        "header": options.name,
                    }});
                    await fs.writeFile(__src + "/" + options.name + ".h", art.header)
                    await fs.writeFile(__src + "/" + options.name + ".c", art.c)
                }
            }
        },
    }
}

