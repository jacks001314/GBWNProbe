project('GBWNProbe','c',
    version: run_command(find_program('cat','more'),files('VERSION'),check:true).stdout().strip(),
    meson_version: '>=0.64.1',
    default_options: ['warning_level=3']
)

main_source = files('test.c')

subdir('lib')
probe_deps_lib = [common_lib]

executable('GBWProbe', 
  main_source,
  link_with : probe_deps_lib)

