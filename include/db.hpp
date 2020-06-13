#pragma once
#include <string>

#include "sqlite_orm/sqlite_orm.h"

// unique_ptrs are used to indicate nullability.

struct Order;
struct Work;


enum class Size : int {
  S2x4,
  S2x2,
  S2x8,
  S4x6,
  S2x12,
  S4x4,
  /// 5/4 x 6
  S5_4x6,

  /// Never to be instantiated. Just here for row_extractor
  MAX
};

// Bind Size into sqlite_orm
// This kind of ugliness is why I'm writing Ligi.
namespace sqlite_orm {
template<>
struct type_printer<Size>: public integer_printer {};

template<>
struct statement_binder<Size> {
  int bind(sqlite3_stmt* stmt, int index, const Size& val) {
    return statement_binder<int>().bind(stmt, index, (int)val);
  }
};

template<>
struct field_printer<Size> {
  int operator()(const Size& val) const { return (int)val; }
};

template<>
struct row_extractor<Size> {
  Size extract(int rowVal) {
    if (rowVal < (int)Size::MAX and rowVal > 0)
      return (Size)rowVal;
    else
      throw new std::runtime_error("Incorrect Size value in DB!");
  }

  Size extract(sqlite3_stmt* stmt, int colIndex) {
    auto val = sqlite3_column_int(stmt, colIndex);
    return this->extract(val);
  }
};

};  // namespace sqlite_orm


struct Material {
  int    id;
  double pricePerUnit;
  std::string
      /// Is it a board/nail/baluster/misc/etc?
      type,
      /// What is it made out of? If misc: What's its name
      kind;

  // These two are for boards and the like

  /// One of the discrete lengths 6/8/10/12
  std::unique_ptr<int>  length;
  std::unique_ptr<Size> size;

  void               update();
  std::vector<Order> orders();
};

struct Order {
  int deckID, matID, quantity;

  void update();
};

struct Deck {
  int         id;
  std::string name, color;
  double      length, width, height;
  bool hasRail, hasStairs;
  void               update();
  std::vector<Order> orders();
  std::vector<Work>  works();
};

struct Work {
  int deckID, empID, hours;

  void update();
};

struct Employee {
  int                          id;
  std::string                  firstName, lastName;
  double                       wages;
  std::unique_ptr<std::string> phone;

  void              update();
  std::vector<Work> works();
};

struct DB {
  //

  static std::vector<Deck>     get_decks();
  static std::vector<Material> get_materials();
  static std::vector<Employee> get_employees();

  static std::vector<Material> get_mats_of_kind(const std::string& kind);
  static std::vector<Material> get_mats_of_type(const std::string& type);


  static Material get_material(int id);
  static Order    get_order(int id);
  static Deck     get_deck(int id);
  // I've solved unemployment!
  static Work     get_work(int id);
  static Employee get_employee(int id);

  static Deck new_deck();
  static Employee new_employee();
  static Material new_material();
  static Work new_work(int empID, int deckID);
  static Order new_order(int matID, int deckID);

  static auto& get_db() {
    using namespace sqlite_orm;
    // clang-format off
    static auto storage = make_storage("deckest.db",
      make_table("Deck",
        make_column("id", &Deck::id, autoincrement(), primary_key()),
        make_column("name", &Deck::name),
        make_column("color", &Deck::color),
        make_column("length", &Deck::length),
        make_column("width", &Deck::width),
        make_column("height", &Deck::height),
        make_column("hasRail", &Deck::hasRail),
        make_column("hasStairs", &Deck::hasStairs)
        
      ), 
      make_table("Employee", 
        make_column("id", &Employee::id, autoincrement(), primary_key()),
        make_column("firstName", &Employee::firstName),
        make_column("lastName", &Employee::lastName),
        make_column("wages", &Employee::wages),
        make_column("phone", &Employee::phone)
      ),
      make_table("Work",
        make_column("deckID", &Work::deckID),
        make_column("empID", &Work::empID),
        make_column("hours", &Work::hours),
        foreign_key(&Work::deckID).references(&Deck::id),
        foreign_key(&Work::empID).references(&Employee::id)
      ),
      make_table("Order",
        make_column("deckID", &Order::deckID),
        make_column("matID", &Order::matID),
        make_column("quantity", &Order::quantity),
        foreign_key(&Order::deckID).references(&Deck::id),
        foreign_key(&Order::matID).references(&Material::id)
      ),
      make_table("Material", 
        make_column("id", &Material::id, autoincrement(), primary_key()),
        make_column("pricePerUnit", &Material::pricePerUnit),
        make_column("type", &Material::type),
        make_column("kind", &Material::kind),
        make_column("length", &Material::length),
        make_column("size", &Material::size)
      )
    );
    // clang-format on
    return storage;
  }
};
