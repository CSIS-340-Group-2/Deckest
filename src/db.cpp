#include "db.hpp"

using namespace sqlite_orm;

void Material::update() { DB::get_db().replace(*this); }
void Order::update() { DB::get_db().replace(*this); }
void Deck::update() { DB::get_db().replace(*this); }
void Work::update() { DB::get_db().replace(*this); }
void Employee::update() { DB::get_db().replace(*this); }


Material DB::get_material(int id) { return DB::get_db().get<Material>(id); }
Deck     DB::get_deck(int id) { return DB::get_db().get<Deck>(id); }
Employee DB::get_employee(int id) { return DB::get_db().get<Employee>(id); }

std::vector<Material> DB::get_materials() { return DB::get_db().get_all<Material>(); }
std::vector<Employee> DB::get_employees() { return DB::get_db().get_all<Employee>(); }
std::vector<Deck>     DB::get_decks() { return DB::get_db().get_all<Deck>(); }

std::vector<Order> Material::orders() {
  return DB::get_db().get_all<Order>(where(is_equal(&Order::matID, this->id)));
}

std::vector<Order> Deck::orders() {
  return DB::get_db().get_all<Order>(where(is_equal(&Order::deckID, this->id)));
}

std::vector<Work> Deck::works() {
  return DB::get_db().get_all<Work>(where(is_equal(&Work::deckID, this->id)));
}

std::vector<Work> Employee::works() {
  return DB::get_db().get_all<Work>(where(is_equal(&Work::empID, this->id)));
}
