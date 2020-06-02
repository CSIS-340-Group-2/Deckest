project(
  'deckest', 
  'cpp',
  default_options: ['cpp_std=c++17']
)

gtkmm = dependency('gtkmm-3.0')
sqlite3 = dependency('sqlite3')
# Sqlite_orm is header-only, so we don't need a dep up here.
# We'll just get it from a submodule include

srcs = [
  'src/main.cpp'
]

includes = [
  include_directories('thirdparty/sqlite_orm/include/sqlite_orm/')
]

executable('deckest', srcs, include_directories: includes, dependencies: [gtkmm, sqlite3])
