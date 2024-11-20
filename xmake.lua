local projectName = "WaveTimer"

target(projectName)
    add_rules("ue4ss.mod")
    add_includedirs(".")
    add_files("**.cpp")
    on_link(function (target)
        print(target)
    end)
