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
Order DB::get_order(int id) { return DB::get_db().get<Order>(id); }
Work DB::get_work(int id) { return DB::get_db().get<Work>(id); }

std::vector<Material> DB::get_materials() { return DB::get_db().get_all<Material>(); }
std::vector<Employee> DB::get_employees() { return DB::get_db().get_all<Employee>(); }
std::vector<Deck>     DB::get_decks() { return DB::get_db().get_all<Deck>(); }

std::vector<Material> get_mats_of_kind(const std::string& kind) {
  return DB::get_db().get_all<Material>(where(is_equal(&Material::kind, kind)));
}

std::vector<Material> get_mats_of_type(const std::string& type) {
  return DB::get_db().get_all<Material>(where(is_equal(&Material::type, type)));
}

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

Deck DB::new_deck() {
  Deck newDeck = {
    .id        = -1,
    .name      = "New Deck",
    .color     = "#ff0000",
    .length    = 10,
    .width     = 10,
    .height    = 6,
    .hasRail   = true,
    .hasStairs = true,
  };

  return DB::get_deck(DB::get_db().insert(newDeck));
}
Employee DB::new_employee() {
  Employee newEmployee = {
    .id        = -1,
    .firstName = "New",
    .lastName  = "Employee",
    .wages     = 17.0,
    .phone     = nullptr,
  };
  return DB::get_employee(DB::get_db().insert(newEmployee));
}
Material DB::new_material() {
  Material newMaterial = { .id           = -1,
                           .pricePerUnit = 0,
                           .type         = "Misc",
                           .kind         = "New Material",
                           .length       = nullptr,
                           .size         = nullptr };

  return DB::get_material(DB::get_db().insert(newMaterial));
}
Work DB::new_work(int empID, int deckID) {
  Work newWork = { .deckID = deckID, .empID = empID, .hours = 0 };
  return DB::get_work(DB::get_db().insert(newWork));
}
Order DB::new_order(int matID, int deckID) {
  Order newOrder = { .deckID = deckID, .matID = matID, .quantity = 0 };
  return DB::get_order(DB::get_db().insert(newOrder));
}
