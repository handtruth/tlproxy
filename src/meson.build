sources = files([
  'sample_source.cpp'
])

src = include_directories('.')

sources += configure_file(output : 'config.cpp', input : 'config.cpp.in',
  configuration : {
  'group' : group,
  'project' : meson.project_name(),
  'version' : meson.project_version(),
  'build' : run_command('git', 'describe').stdout().strip(),
  'maintainer' : maintainer,
})

static_lib = static_library(meson.project_name(), sources, dependencies: module_deps)
exe = executable(meson.project_name(), 'main.cpp', link_with : static_lib, install : true, dependencies : module_deps)
run_target('run', command : [exe] + get_option('run_args'), depends : exe)
