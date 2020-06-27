#include "db.hpp"

using namespace sqlite_orm;

void Component::update() { DB::get_db().replace(*this); }
void Order::update() { DB::get_db().replace(*this); }
void Deck::update() { DB::get_db().replace(*this); }
void Work::update() { DB::get_db().replace(*this); }
void Employee::update() { DB::get_db().replace(*this); }

void Deck::remove() { DB::get_db().remove<Deck>(this->id); }
void Component::remove() { DB::get_db().remove<Component>(this->id); }
// These 2 are TODO
// void Order::remove() { DB::get_db().remove<Order>(*this); }
// void Work::remove() { DB::get_db().remove<Work>(*this); }
void Employee::remove() { DB::get_db().remove<Employee>(this->id); }

Component DB::get_material(int id) { return DB::get_db().get<Component>(id); }
Deck      DB::get_deck(int id) { return DB::get_db().get<Deck>(id); }
Employee  DB::get_employee(int id) { return DB::get_db().get<Employee>(id); }
Order     DB::get_order(int id) { return DB::get_db().get<Order>(id); }
Work      DB::get_work(int id) { return DB::get_db().get<Work>(id); }
WoodType  DB::get_woodtype(int id) { return DB::get_db().get<WoodType>(id); }
double    DB::get_avgprice(ComponentType ty) {
  return DB::get_db().select(&CommonComps::avgPricePerUnit, where(c(&CommonComps::type) == ty))[0];
}
void DB::set_avgprice(ComponentType ty, double price) {
  DB::get_db().update_all(set(c(&CommonComps::avgPricePerUnit) = price),
                          where(c(&CommonComps::type) == ty));
}

std::vector<Component> DB::get_materials() { return DB::get_db().get_all<Component>(); }
std::vector<Employee>  DB::get_employees() { return DB::get_db().get_all<Employee>(); }
std::vector<Deck>      DB::get_decks() { return DB::get_db().get_all<Deck>(); }
std::vector<WoodType>  DB::get_woodtypes() { return DB::get_db().get_all<WoodType>(); }

std::vector<Component> get_mats_of_type(const ComponentType& type) {
  return DB::get_db().get_all<Component>(where(is_equal(&Component::type, type)));
}

std::vector<Order> Component::orders() {
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
    .angle     = 0,
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
Component DB::new_material() {
  Component newComponent = { .id           = -1,
                             .pricePerUnit = 0,
                             .type         = ComponentType::Misc,
                             .name         = "New Material",
                             .length       = Length::Null,
                             .size         = Size::Null };

  return DB::get_material(DB::get_db().insert(newComponent));
}
Work DB::new_work(int empID, int deckID) {
  Work newWork = { .deckID = deckID, .empID = empID, .hours = 0 };
  return DB::get_work(DB::get_db().insert(newWork));
}
Order DB::new_order(int matID, int deckID) {
  Order newOrder = { .deckID = deckID, .matID = matID, .quantity = 0 };
  return DB::get_order(DB::get_db().insert(newOrder));
}
WoodType DB::new_woodtype() {
  WoodType newWoodType
      = { .id = -1, .name = "New WoodType", .desc = "Complete me!", .pricePerBF = 0.0 };
  return DB::get_woodtype(DB::get_db().insert(newWoodType));
}
