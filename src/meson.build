sources = [
  'core/llyfr-search-context.c',
  'core/llyfr-search-match.c',
  'core/llyfr-search-result.c',
  'gui/llyfr-search-bar.c',
  'gui/llyfr-search-context-switcher.c',
  'gui/llyfr-search-page.c',
  'gui/llyfr-search-result-view.c',
  'main.c',
  'llyfr-application.c',
  'llyfr-window.c',
]

deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk4', version: '>= 4.0'),
  dependency('json-glib-1.0', version: '>= 1.2.0'),
  dependency('libadwaita-1')
]

includes = include_directories(
  'core',
  'gui',
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
