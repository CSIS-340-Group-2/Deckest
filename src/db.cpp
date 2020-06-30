#include "db.hpp"

using namespace sqlite_orm;

void Component::update() { DB::get_db().replace(*this); }
void Order::update() { DB::get_db().replace(*this); }
void Deck::update() { DB::get_db().replace(*this); }
void Work::update() { DB::get_db().replace(*this); }
void Employee::update() { DB::get_db().replace(*this); }
void WoodType::update() { DB::get_db().replace(*this); }

void Deck::remove() { DB::get_db().remove<Deck>(this->id); }
void Component::remove() { DB::get_db().remove<Component>(this->id); }
// These 2 are TODO
// void Order::remove() { DB::get_db().remove<Order>(*this); }
// void Work::remove() { DB::get_db().remove<Work>(*this); }
void Employee::remove() { DB::get_db().remove<Employee>(this->id); }
void WoodType::remove() { DB::get_db().remove<WoodType>(this->id); }

void  Deck::clear_orders() { DB::get_db().remove_all<Order>(where(c(&Order::deckID) == this->id)); }
Order Deck::order(const Component& comp, int quant) {
  DB::get_db().replace(Order{ .deckID = this->id, .matID = comp.id, .quantity = quant });
  return DB::get_db().get_all<Order>(
      where(c(&Order::deckID) == this->id && c(&Order::matID) == comp.id))[0];
}

Component DB::get_component(int id) { return DB::get_db().get<Component>(id); }
Deck      DB::get_deck(int id) { return DB::get_db().get<Deck>(id); }
Employee  DB::get_employee(int id) { return DB::get_db().get<Employee>(id); }
Order     DB::get_order(int id) { return DB::get_db().get<Order>(id); }
Work      DB::get_work(int id) { return DB::get_db().get<Work>(id); }
WoodType  DB::get_woodtype(int id) { return DB::get_db().get<WoodType>(id); }
double    DB::get_avgprice(ComponentType ty) {
  auto results
      = DB::get_db().select(&CommonComps::avgPricePerUnit, where(c(&CommonComps::type) == ty));
  if (results.size() > 0)
    return results[0];
  else
    return 0.0;
}
void DB::set_avgprice(ComponentType ty, double price) {
  DB::get_db().replace(CommonComps{ ty, price });
}

std::vector<Component> DB::get_components() { return DB::get_db().get_all<Component>(); }
std::vector<Employee>  DB::get_employees() { return DB::get_db().get_all<Employee>(); }
std::vector<Deck>      DB::get_decks() { return DB::get_db().get_all<Deck>(); }
std::vector<WoodType>  DB::get_woodtypes() { return DB::get_db().get_all<WoodType>(); }

std::vector<Component> DB::get_comps_of_type(ComponentType type) {
  return DB::get_db().get_all<Component>(where(is_equal(&Component::type, type)));
}

std::vector<Order> Component::orders() const {
  return DB::get_db().get_all<Order>(where(is_equal(&Order::matID, this->id)));
}

std::vector<Order> Deck::orders() const {
  return DB::get_db().get_all<Order>(where(is_equal(&Order::deckID, this->id)));
}

std::vector<Work> Deck::works() const {
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
Component DB::new_component() {
  Component newComponent = { .id           = -1,
                             .pricePerUnit = 0,
                             .type         = ComponentType::Misc,
                             .name         = "New Material",
                             .length       = Length::Null,
                             .size         = Size::Null };

  return DB::get_component(DB::get_db().insert(newComponent));
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

std::string Component::get_name() {
  switch (this->type) {
    // No other way for a misc
  case ComponentType::Misc: return this->name;
  case ComponentType::Board: {
    std::string res = this->name + "( ";
    switch (this->size) {
    case Size::Null: res += "NULL"; break;
    case Size::S2x4: res += "2x4"; break;
    case Size::S2x2: res += "2x2"; break;
    case Size::S2x8: res += "2x8"; break;
    case Size::S4x6: res += "4x6"; break;
    case Size::S2x12: res += "2x12"; break;
    case Size::S4x4: res += "4x4"; break;
    case Size::S5_4x6: res += "5/4x6"; break;
    default: exit(-2663);
    }
    res += "in ";
    switch (this->length) {
    case Length::Baluster: res += " 42in"; break;
    default: res += std::to_string((int)this->length); break;
    }
    res += " Board )";
    return res;
  }
  case ComponentType::Nail: return this->name + "(Pounds of Nails)";
  case ComponentType::Concrete: return this->name + "(Pounds of Concrete)";
  default: exit(-2663);
  }
  return "@@@@@";
}
// type only needs to be valid if this->type == #Board
double Component::get_price(WoodType type) {
  switch (this->type) {
  case ComponentType::Misc: return this->pricePerUnit;
  case ComponentType::Board: {
    // Need to get the board-footage of the board, then mul by type->pricePerBF
    double bf = (double)this->length;
    switch (this->size) {
    case Size::S2x4: bf *= (2 * 4) / (12.0 * 12.0); break;
    case Size::S2x2: bf *= (2 * 2) / (12.0 * 12.0); break;
    case Size::S2x8: bf *= (2 * 8) / (12.0 * 12.0); break;
    case Size::S4x6: bf *= (4 * 6) / (12.0 * 12.0); break;
    case Size::S2x12: bf *= (2 * 12) / (12.0 * 12.0); break;
    case Size::S4x4: bf *= (4 * 4) / (12.0 * 12.0); break;
    case Size::S5_4x6: bf *= (5.0 / 4 * 6) / (12.0 * 12.0); break;
    default: exit(-2663);
    }
    return bf * type.pricePerBF;
  }
  case ComponentType::Nail:
  case ComponentType::Concrete: return DB::get_avgprice(this->type);
  default: exit(-2663);
  }
}
