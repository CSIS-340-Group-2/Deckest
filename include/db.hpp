#pragma once
#include <string>
#include <iostream>

#include "sqlite_orm/sqlite_orm.h"

// unique_ptrs are used to indicate nullability.

struct Order;
struct Work;

// Bind enums into sqlite_orm
// This kind of ugliness is why I'm writing Ligi.
#define BIND_ENUM(E)                                                                               \
  namespace sqlite_orm {                                                                           \
    template<>                                                                                     \
    struct type_printer<E>: public integer_printer {};                                             \
    template<>                                                                                     \
    struct statement_binder<E> {                                                                   \
      int bind(sqlite3_stmt* stmt, int index, const E& val) {                                      \
        return statement_binder<int>().bind(stmt, index, (int)val);                                \
      }                                                                                            \
    };                                                                                             \
    template<>                                                                                     \
    struct field_printer<E> {                                                                      \
      int operator()(const E& val) const { return (int)val; }                                      \
    };                                                                                             \
    template<>                                                                                     \
    struct row_extractor<E> {                                                                      \
      E extract(int rowVal) {                                                                      \
        if (rowVal < (int)E::_MAX and rowVal >= 0)                                                 \
          return (E)rowVal;                                                                        \
        else                                                                                       \
          throw std::runtime_error("Incorrect value for " #E " in DB!");                           \
      }                                                                                            \
      E extract(sqlite3_stmt* stmt, int colIndex) {                                                \
        auto val = sqlite3_column_int(stmt, colIndex);                                             \
        return this->extract(val);                                                                 \
      }                                                                                            \
    };                                                                                             \
  };  // namespace sqlite_orm


enum class Size : int {
  Null,
  S2x4,
  S2x2,
  S2x8,
  S4x6,
  S2x12,
  S4x4,
  /// 5/4 x 6
  S5_4x6,

  /// Never to be instantiated. Just here for row_extractor
  _MAX
};
BIND_ENUM(Size)

enum class Length : int {
  Null     = 0,
  Six      = 6,
  Eight    = 8,
  Ten      = 10,
  Twelve   = 12,
  Baluster = 42,

  _MAX,
};
BIND_ENUM(Length)

enum class ComponentType : int {
  Board,
  Nail,
  Concrete,
  Misc,

  _MAX
};
BIND_ENUM(ComponentType)

struct WoodType {
  int         id;
  std::string name, desc;
  double      pricePerBF;

  void update();
  void remove();
};

struct Component {
  int    id;
  double pricePerUnit;

  ComponentType type;

  /// A name = "" means to derive it as needed
  std::string name;

  // These two are for boards and the like
  Length length;
  Size   size;

  void               update();
  void               remove();
  std::vector<Order> orders() const;

  // These two provided because some component types have odd handling
  std::string get_name();
  double      get_price(WoodType type);
};

struct Order {
  int deckID, matID, quantity;

  void update();
  void remove();
};

struct Deck {
  int                id;
  std::string        name, color;
  double             length, width, height;
  bool               hasRail, hasStairs;
  int                angle;
  void               update();
  void               remove();
  std::vector<Order> orders() const;
  void               clear_orders();
  Order              order(const Component& comp, int quant);
  std::vector<Work>  works() const;
};

struct Work {
  int deckID, empID, hours;

  void update();
  void remove();
};

struct Employee {
  int                          id;
  std::string                  firstName, lastName;
  double                       wages;
  std::unique_ptr<std::string> phone;

  void              update();
  void              remove();
  std::vector<Work> works();
};

/// Just used to store global prices for certain things
struct CommonComps {
  ComponentType type;
  double        avgPricePerUnit;
};

struct DB {
  //

  static std::vector<Deck>      get_decks();
  static std::vector<Component> get_components();
  static std::vector<Employee>  get_employees();
  static std::vector<WoodType>  get_woodtypes();

  static std::vector<Component> get_comps_of_type(ComponentType type);


  static Component get_component(int id);
  static Order     get_order(int id);
  static Deck      get_deck(int id);
  // I've solved unemployment!
  static Work      get_work(int id);
  static Employee  get_employee(int id);
  static WoodType  get_woodtype(int id);
  static double    get_avgprice(ComponentType ty);
  static void      set_avgprice(ComponentType ty, double price);
  static Deck      new_deck();
  static Employee  new_employee();
  static Component new_component();
  static Work      new_work(int empID, int deckID);
  static Order     new_order(int matID, int deckID);
  static WoodType  new_woodtype();

  static auto init_db() {
    using namespace sqlite_orm;
    // clang-format off
    auto storage = make_storage("deckest.db",
      make_table("Deck",
        make_column("id", &Deck::id, autoincrement(), primary_key()),
        make_column("name", &Deck::name),
        make_column("color", &Deck::color),
        make_column("length", &Deck::length),
        make_column("width", &Deck::width),
        make_column("height", &Deck::height),
        make_column("hasRail", &Deck::hasRail),
        make_column("hasStairs", &Deck::hasStairs),
        make_column("angle", &Deck::angle)
        
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
        foreign_key(&Order::matID).references(&Component::id)
      ),
      make_table("Component", 
        make_column("id", &Component::id, autoincrement(), primary_key()),
        make_column("pricePerUnit", &Component::pricePerUnit),
        make_column("type", &Component::type),
        make_column("name", &Component::name),
        make_column("length", &Component::length),
        make_column("size", &Component::size)
      ),
      make_table("WoodType",
        make_column("id", &WoodType::id, primary_key()),
        make_column("name", &WoodType::name),
        make_column("desc", &WoodType::desc),
        make_column("pricePerBF", &WoodType::pricePerBF)
      ),
      make_table("CommonComps",
        make_column("type", &CommonComps::type, primary_key()),
        make_column("avgPricePerUnit", &CommonComps::avgPricePerUnit, default_value(0.0))
      )
    );
    // clang-format on
    std::cerr << "Making storage...\n";

    return storage;
  }

  static auto& get_db() {
    static auto storage = init_db();
    return storage;
  }
};
