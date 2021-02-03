local version = "0.1.0"
local versions = version:split(".",{plain = true}) -- lua array index start from 1

set_project("parser")
set_languages("c17","c++17")

target("header-only")
    add_headerfiles("src/(keyvalue/*.hpp)")
    add_headerfiles("src/(multipart/*.hpp)")
    set_installdir(format("/data/vendor/vsp-%s.%s", versions[1], versions[2]))

target("keyvalue")
    add_rules("mode.debug")
    set_kind("binary")
    add_files("example/keyvalue.cpp")
    on_install(function()
        -- none
    end)

target("multipart")
    set_kind("binary")
    add_rules("mode.debug")
    add_files("example/multipart.cpp")
    on_install(function()
        -- none
    end)
