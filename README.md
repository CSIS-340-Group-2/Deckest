# Deckest
The deckest deck estimator to ever est a deck

## Licenses
Files specifically authored by this project, which includes all files in this repository
with the exception of those in `include/sqlite_orm/`, are released into the public domain.

This project depends on including and/or linking to the following open source 
libraries at build time.
* [sqlite3](https://sqlite.org/copyright.html), which is public domain
* [sqlite_orm](https://github.com/fnc12/sqlite_orm) inside `include/sqlite_orm/sqlite_orm.h`,
  which is governed by the BSD 3-Clause License ([found here](include/sqlite_orm/LICENSE))
  * sqlite_orm's header is included directly in this project for ease of development,
    but its original license is preserved as `include/sqlite_orm/LICENSE`.
* [GTKmm](https://www.gtkmm.org/en/license.html), which is under the LGPL.

Due to the above, any binary redistribution of this project must be licensed under the LGPL.
