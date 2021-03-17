sources = [
  'models/llyfr-search-context.c',
  'models/llyfr-search-result.c',
  'main.c',
  'llyfr-application.c',
  'llyfr-search-bar.c',
  'llyfr-window.c',
]

deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk4', version: '>= 4.0'),
  dependency('json-glib-1.0', version: '>= 1.2.0'),
]

includes = include_directories(
  'models'
)

gnome = import('gnome')

sources += gnome.compile_resources('llyfrgell-resources',
  'llyfrgell.gresource.xml',
  c_name: 'llyfrgell'
)

executable('llyfrgell',
  sources,
  include_directories: includes,
  dependencies: deps,
  install: true,
)
