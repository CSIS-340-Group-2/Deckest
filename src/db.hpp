#pragma once
#include <string>

#include "sqlite_orm/sqlite_orm.h"

// unique_ptrs are used to indicate nullability.


struct Material {
  int    id;
  double pricePerUnit;
  std::string
      /// Is it a board/nail/baluster/misc/etc?
      type,
      /// What is it made out of? If misc: What's its name
      kind;

  std::unique_ptr<double> length, width, height;
};

struct Order {
  int deckID, matID, quantity;
};

struct Deck {
  int         id;
  std::string name, color;
  double      length, width, height;
};

struct Work {
  int deckID, empID, hours;
};

struct Employee {
  int                          id;
  std::string                  firstName, lastName;
  double                       wages;
  std::unique_ptr<std::string> phone;
};

class DB {
 public:
 private:
  static auto make_db() {
    using namespace sqlite_orm;
    return make_storage("deckest.db",
      make_table("deck",
        make_column("id", &Deck::id, autoincrement(), primary_key()),
        make_column("name", &Deck::name),
        make_column("color", &Deck::color),
        make_column("length", &Deck::length),
        make_column("width", &Deck::width),
        make_column("height", &Deck::height)
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
		make_column("hours", &Work::hours)
	),
	make_table("Order",
		make_column("deckID", &Order::deckID),
		make_column("matID", &Order::matID),
		make_column("quantity", &Order::quantity)
	),
	make_table("Material", 
		make_column("id", &Material::id, autoincrement(), primary_key()),
		make_column("pricePerUnit", &Material::pricePerUnit),
		make_column("type", &Material::type),
		make_column("kind", &Material::kind),
		make_column("length", &Material::length),
		make_column("width", &Material::width),
		make_column("height", &Material::height)
	)
    );
  }
};
