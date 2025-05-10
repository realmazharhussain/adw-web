adw_web_sources = [
  'main.c',
  'adw-web-application.c',
  'adw-web-window.c',
]

adw_web_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
]

adw_web_sources += gnome.compile_resources('adw-web-resources',
  'adw-web.gresource.xml',
  c_name: 'adw_web'
)

executable('adw-web', adw_web_sources,
  dependencies: adw_web_deps,
       install: true,
)
