local version = "0.1.0"
local versions = version:split(".", {plain = true}) -- lua array index start from 1

set_project("parser")
set_languages("c17","c++17")

target("header-only")
    add_headerfiles("include/(vspp/*.hpp)")
    add_headerfiles("include/(vspp/keyvalue/*.hpp)")
    add_headerfiles("include/(vspp/multipart/*.hpp)")
    set_installdir(format("/data/vendor/vspp-%s.%s", versions[1], versions[2]))

target("keyvalue")
    add_rules("mode.debug")
    set_kind("binary")
    add_includedirs("include")
    add_files("example/keyvalue.cpp")
    on_install(function()
        -- none
    end)

target("multipart")
    set_kind("binary")
    add_rules("mode.debug")
    add_includedirs("include")
    add_files("example/multipart.cpp")
    on_install(function()
        -- none
    end)
