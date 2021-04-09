sources = [
  'gui/llyfr-search-context-switcher.c',
  'models/llyfr-search-context.c',
  'models/llyfr-search-match.c',
  'models/llyfr-search-result.c',
  'main.c',
  'llyfr-application.c',
  'llyfr-search-result-view.c',
  'llyfr-search-screen.c',
  'llyfr-window.c',
]

deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk4', version: '>= 4.0'),
  dependency('json-glib-1.0', version: '>= 1.2.0'),
  dependency('libadwaita-1')
]

includes = include_directories(
  'gui',
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
