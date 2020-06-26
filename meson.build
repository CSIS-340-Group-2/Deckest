project(
  'deckest', 
  'cpp',
  default_options: ['cpp_std=c++17']
)

gtkmm = dependency('gtkmm-3.0')
sqlite3 = dependency('sqlite3')


srcs = [
  'src/main.cpp',
  'src/decks.cpp',
  'src/materials.cpp',
  'src/employees.cpp',
  'src/estimator_ui.cpp',
  'src/about.cpp',
  'src/db.cpp',
]

includes = [
  include_directories('include/'),
]

executable('deckest', srcs, include_directories: [includes], dependencies: [gtkmm, sqlite3])
