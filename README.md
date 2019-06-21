### 介绍
学习和测试 [llparse](https://github.com/nodejs/llparse) / [llhttp](https://github.com/nodejs/llhttp) 编写的 [toml](https://github.com/toml-lang/toml) 解析器;

* `llparse` - 可方便的用于文本类协议的增量解析器生成(目前支持的转换等功能是配合 HTTP 协议设计的, 可能有所不足), 可生成对应头文件、源文件或字节码;
* `llhttp` - 使用上述 `llparse` 开发生成的 HTTP 解析器, 据官方介绍效率较 [http_parser](https://github.com/nodejs/http-parser) 提升一倍有余;

### 协议
本项目使用 MIT 协议
