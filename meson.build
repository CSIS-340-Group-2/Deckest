project(
  'deckest', 
  'cpp',
  default_options: ['cpp_std=c++17']
)

cmake = import('cmake')

gtkmm = dependency('gtkmm-3.0')
sqlite3 = dependency('sqlite3')

sqliteorm_proj = cmake.subproject('sqlite_orm')
sqliteorm = sqliteorm_proj.dependency('sqlite_orm')

srcs = [
  'src/main.cpp'
]

executable('deckest', srcs, dependencies: [gtkmm, sqlite3, sqliteorm])
