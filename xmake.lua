-- custom command
task("f2")
    on_run(function()
        import("core.base.task")

        -- generate compile commands
        task.run("project", { kind = "compile_commands", outputdir = "$(buildir)" })
    end)

    set_menu {
        usage = "xmake f2"
        , description = "Run config and generate compile commands to build dir."
        , options = {}
    }

task_end()

-- set project name
set_project("QVogenClient")

-- set project version
set_version("0.2.22")

-- set runtime output dir
set_config("runtime_output_dir", "bin")
set_config("archive_output_dir", "lib")

print("$(env PROGRAMFILES)")