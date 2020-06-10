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
      )
    );
  }
};
