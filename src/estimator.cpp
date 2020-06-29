#include <vector>
#include <cmath>

#include "db.hpp"

using namespace std;

// To summarize things:
// Each deck has an angle. 0 degrees is parallel to the house
// Each deck has width (along the house) and length (away from house) and height (from ground)
// Each deck may or may not have stairs and/or a railing
// Boards of 6/8/10/12 feet in length, each of which is one of...
// 2x4, 2x2, 2x8, 4x6, 2x12, 4x4, or 5/4 x 6 in width/height
// For stair stringers, we'll assume the user cuts their own from a board of proper size

/// The stuff we need to ensure is in the DB
const Component ingredients[] = {
  // 2 x 4 - 10 Foot                                            2           each
  Component{ 20000001, 0, ComponentType::Board, "", Length::Ten, Size::S2x4 },
  // 2 x 8 - 10 Foot                                            12         each
  Component{ 20000002, 0, ComponentType::Board, "", Length::Ten, Size::S2x8 },
  // 2 x 4 - 8 Foot                                              6           each
  Component{ 20000004, 0, ComponentType::Board, "", Length::Eight, Size::S2x4 },
};


/// I gave it a massive ID offset since I'm assuming the user will never hit it.
/// Bad programming, I know, but ya do what ya gotta do to hit a class deadline
static int NEXT_ID = 2000000;

// The boards on top of the railings
const Component RAILS[] = {
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Six, Size::S2x8 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Eight, Size::S2x8 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Ten, Size::S2x8 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Twelve, Size::S2x8 },
};

const Component JOISTS[] = {
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Six, Size::S2x12 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Eight, Size::S2x12 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Ten, Size::S2x12 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Twelve, Size::S2x12 },
};

const Component DECKING_BOARDS[] = {
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Six, Size::S5_4x6 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Eight, Size::S5_4x6 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Ten, Size::S5_4x6 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Twelve, Size::S5_4x6 },
};

const Component POLES[] = {
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Six, Size::S4x4 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Eight, Size::S4x4 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Ten, Size::S4x4 },
  Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Twelve, Size::S4x4 },
};

const auto BALUSTER
    = Component{ NEXT_ID++, 0, ComponentType::Board, "", Length::Baluster, Size::S2x2 };
const auto STRINGER
    = Component{ NEXT_ID++, 0, ComponentType::Board, "Stringer", Length::Ten, Size::S2x12 };
const auto NAILS
    = Component{ NEXT_ID++, 0, ComponentType::Nail, "1lb of Nails", Length::Null, Size::Null };
const auto CONCRETE = Component{
  NEXT_ID++, 0, ComponentType::Concrete, "80lb of Concrete", Length::Null, Size::Null
};

void ensure_components() {
  auto& db = DB::get_db();
  db.transaction([&]() {
    for (auto comp : POLES) db.replace<Component>(comp);
    for (auto comp : RAILS) db.replace<Component>(comp);
    for (auto comp : DECKING_BOARDS) db.replace<Component>(comp);
    for (auto comp : JOISTS) db.replace<Component>(comp);
    db.replace<Component>(BALUSTER);
    db.replace<Component>(NAILS);
    db.replace<Component>(STRINGER);
    db.replace<Component>(CONCRETE);
    return true;
  });
}

/// Clamp to the next highest of 6/8/10/12
/// Higher than 12 is an error
int clamp_len(double len) {
  if (len <= 6.25)
    return 6;
  else if (len <= 8.25)
    return 8;
  else if (len <= 10.25)
    return 10;
  else
    return 12;
}

/// e.g for floarboards. Given a [len]gth we need to go, decide on
/// 6ft/8ft/10ft/12ft and how many of each are needed
/// @return <size_index, qnt>, where size_index works for any of the component arrays above
std::pair<int, int> clamp_len_qnt(double len) {
  auto l = clamp_len(len);
  // size_index === 6/8/10/12 -> 3/4/5/6 -> 0/1/2/3
  return { l / 2 - 3, ceil(len / l) };
}

/// Update the Orders for a deck. We're going to simplify things and just scale the
/// amounts he provided by length/width.
/// TODO: This function is very rough and very ugly. In a normal job, we'd need to
/// have another sit down with the client and go over exactly what is needed for a deck.
/// Unfortunately, we're kinda late in the sprint and need to just have something to show.
/// @pre Deck must exist.
/// @post The DB has all appropriate board types required and the Orders table is populated
///       properly for the deck being estimated.
void estimate(Deck deck) {
  ensure_components();
  deck.clear_orders();

  // Handle poles...

  // ...for holding it up
  auto hpIndex = clamp_len(deck.height) / 2 - 3;
  deck.order(POLES[hpIndex], ceil(deck.width * 4 / 10.0));

  // ...for framing
  auto fp = clamp_len_qnt(deck.width + deck.length * 2);
  deck.order(POLES[fp.first], fp.second);

  // Handle floarboards
  // N floorboards in width(X) and length(Y)
  int floorX = 0, floorY = 0;
  int floorIndex = 0;
  switch (deck.angle) {
  case 0:
    // Deck boards are parallel to the house's wall. Use width
    // All floorboards are 6 inches in width, so we can use that here
    std::tie(floorX, floorIndex) = clamp_len_qnt(deck.width);
    floorY                       = ceil(deck.length / 0.5);
    break;
  case 90:
    // Deck boards perpendicular to house's wall
    std::tie(floorY, floorIndex) = clamp_len_qnt(deck.width);
    floorY                       = ceil(deck.width / 0.5);
    break;
  case 45:
    // Deck boards 45 to house's wall
    // This is a somewhat special case compared to the other two.
    // TODO
    break;
  }
  deck.order(DECKING_BOARDS[floorIndex], floorX * floorY);

  // 62 balusters for a 10x10ft deck. ~2.0667 per foot. Round up.
  if (deck.hasRail) deck.order(BALUSTER, ceil(62.0 * (deck.width + deck.length * 2) / 30.0));

  // 1 10ft stringer on each side for every 6 feet the deck is off the ground, rounding up
  if (deck.hasStairs) deck.order(STRINGER, ceil(deck.height / 6.0 * 1) * 2);

  // Nails. Eyeballing the amount needed based on his materials list
  deck.order(NAILS, ceil(deck.width * deck.height * deck.length / 200.0));
  // Concrete. Same as with nails, we eyeball it
  deck.order(CONCRETE, ceil(deck.width * deck.height * deck.length * (400.0 / 600.0)));
}

/// @pre Deck must exist.
double estimate_time(Deck deck) { return 0.0; }


/// Gets all orders associated with deck and sums up their prices
/// @pre material and deck must exist in DB
double get_cost(WoodType material, Deck deck) {
  double res    = 0.0;
  auto   orders = deck.orders();
  for (auto order : orders) {
    auto comp = DB::get_component(order.matID);
    res += comp.get_price(material) * order.quantity;
  }

  return res;
}
