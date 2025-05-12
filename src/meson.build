adw_web_sources = [
  'main.c',
  'adw-web-application.c',
  'adw-web-window.c',
  'adw-web-folder-item.c',
]

adw_web_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.4'),
  dependency('webkitgtk-6.0')
]

blueprints = custom_target('blueprints',
  input: files(
    'gtk/help-overlay.blp',
    'adw-web-window.blp',
    'adw-web-folder-item.blp',
  ),
  output: '.',
  command: [find_program('blueprint-compiler'), 'batch-compile', '@OUTPUT@', '@CURRENT_SOURCE_DIR@', '@INPUT@'],
)

adw_web_sources += gnome.compile_resources('adw-web-resources',
  'adw-web.gresource.xml',
  c_name: 'adw_web',
  dependencies: blueprints
)

executable('adw-web', adw_web_sources,
  dependencies: adw_web_deps,
       install: true,
)
