local version = "0.1.0"
local versions = version:split(".",{plain = true}) -- lua array index start from 1

set_project("parser")
set_languages("c17","c++17")

target("keyvalue")
    add_rules("mode.debug")
    set_kind("binary")
    add_files("example/keyvalue.cpp")
    add_headerfiles("src/(keyvalue/*.hpp)")
    set_basename("parser_keyvalue")
    set_installdir(format("/data/vendor/parser-%s.%s", versions[1], versions[2]))
--[[
rule("llparse")
    on_load(function (target) 
        os.iorun(format("node $(projectdir)/llparse.js $(projectdir)/src/%s/parser.js $(buildir)/gen/%s", target._NAME, target._NAME))
        target:add("includedirs", format("$(buildir)/gen/%s", target._NAME))
        target:add("files", format("$(buildir)/gen/%s/*.c", target._NAME))
        target:add("headerfiles", format("$(buildir)/gen/(%s/*.h)", target._NAME))
    end)
    after_clean(function (target)
        local gdir = format("$(buildir)/gen/%s", target._NAME)
        if os.exists(gdir) then
            os.rmdir(gdir)
        end
    end)

    -- add_rules("llparse")
    -- add_includedirs("vendor/tree-sitter/lib/include", "vendor/tree-sitter/lib/src")
]]

target("multipart")
    set_kind("binary")
    add_rules("mode.debug")
    add_files("example/multipart.cpp")
    add_headerfiles("src/(multipart/*.hpp)")
    set_basename("parser_multipart")
    set_installdir(format("/data/vendor/parser-%s.%s", versions[1], versions[2]))

target("url")
    set_kind("binary")
    add_rules("mode.debug")
    add_files("example/url.cpp")
    add_headerfiles("src/(url/*.hpp)")
    set_basename("parser_url")
    set_installdir(format("/data/vendor/parser-%s.%s", versions[1], versions[2]))