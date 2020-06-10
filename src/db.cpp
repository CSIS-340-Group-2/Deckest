#include "db.hpp"

void Material::update() { DB::get_db().replace(*this); }
void Order::update() { DB::get_db().replace(*this); }
void Deck::update() { DB::get_db().replace(*this); }
void Work::update() { DB::get_db().replace(*this); }
void Employee::update() { DB::get_db().replace(*this); }
