set_languages("c23", "c++23")
set_optimize("fastest")
add_rules("mode.release")

target("fan-snake")
    add_rules("qt.widgetapp")
    set_kind("binary")
    add_files("src/*.cc")
    add_includedirs("include")
    