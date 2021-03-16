llyfrgell_sources = [
  'main.c',
  'llyfr-application.c',
  'llyfr-search-bar.c',
  'llyfr-search-context.c',
  'llyfr-window.c',
]

llyfrgell_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk4', version: '>= 4.0'),
]

gnome = import('gnome')

llyfrgell_sources += gnome.compile_resources('llyfrgell-resources',
  'llyfrgell.gresource.xml',
  c_name: 'llyfrgell'
)

executable('llyfrgell', llyfrgell_sources,
  dependencies: llyfrgell_deps,
  install: true,
)
