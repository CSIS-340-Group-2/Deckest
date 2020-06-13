#include "creation_callbacks.hpp"

#include "db.hpp"

#include <iostream>

using namespace std;


class DeckCols: public Gtk::ListStore::ColumnRecord {
 public:
  Gtk::TreeModelColumn<std::string> colName;
  Gtk::TreeModelColumn<int>         colID;

  DeckCols() {
    // ORDER MATTERS HERE
    // The decks.glade has it set to pull from the first col
    // Thus, colName must be first.
    add(colName);
    add(colID);
  }
};

void create_decks(Gtk::Builder* builder) {
  Gtk::TreeView*    deckList;
  Gtk::Entry *      nameInput, *lengthInput, *widthInput, *heightInput;
  Gtk::CheckButton *hasRailing, *hasStairs;
  Gtk::ColorButton* deckColor;

  builder->get_widget("deckList", deckList);
  builder->get_widget("nameInput", nameInput);
  builder->get_widget("lengthInput", lengthInput);
  builder->get_widget("widthInput", widthInput);
  builder->get_widget("heightInput", heightInput);
  builder->get_widget("deckColor", deckColor);
  builder->get_widget("hasRailing", hasRailing);
  builder->get_widget("hasStairs", hasStairs);

  // Steps from here:
  // Populate the deckList
  // Setup handlers for the deckList to automatically pull data into the inputs
  // Setup handlers for the inputs

  // Populate

  // afaik it has to live >= long as the ListStore.
  // I think a static will suffice here.
  static DeckCols cols;
  auto            model = Gtk::ListStore::create(cols);
  deckList->set_model(model);

  // Needs to be extracted so we can call it when the deck name changes
  auto reset_decklist = [=]() {
    // Preserve the currently selected item
    auto curSel = deckList->get_selection()->get_selected();

    auto decks = DB::get_decks();
    for (auto deck : decks) {
      auto row          = *model->append();
      row[cols.colID]   = deck.id;
      row[cols.colName] = deck.name;
    }

    deckList->get_selection()->select(curSel);
  };
  reset_decklist();

  // deckList handlers
  deckList->get_selection()->signal_changed().connect([=]() {
    auto                selected = deckList->get_selection()->get_selected();
    Gtk::TreeModel::Row row      = *selected;
    std::string         deckName;
    int                 deckID;
    row->get_value(0, deckName);
    row->get_value(1, deckID);

    cerr << "Selected deck " << deckName << ", retrieving...\n";

    Deck deck = DB::get_deck(deckID);

    nameInput->set_text(deck.name);
    lengthInput->set_text(std::to_string(deck.length));
    widthInput->set_text(std::to_string(deck.width));
    heightInput->set_text(std::to_string(deck.height));
    deckColor->set_color(Gdk::Color(deck.color));
    cerr << "Color: " << deckColor->get_color().to_string() << std::endl;
  });

  // Input handlers

  nameInput->signal_changed().connect([=]() {
    auto sel = deckList->get_selection()->get_selected();
    if (!sel) return;

    auto deck = DB::get_deck(sel->get_value(cols.colID));

    deck.name = nameInput->get_text();
    // Update the decklist entry
    sel->set_value(cols.colName, deck.name);

    deck.update();
  });

  lengthInput->signal_changed().connect([=]() {
    auto sel = deckList->get_selection()->get_selected();
    if (!sel) return;
    auto deck = DB::get_deck(sel->get_value(cols.colID));
    try {
      deck.length = std::stod(lengthInput->get_text());
    } catch (std::exception& e) {}
    deck.update();
  });
  widthInput->signal_changed().connect([=]() {
    auto sel = deckList->get_selection()->get_selected();
    if (!sel) return;
    auto deck  = DB::get_deck(sel->get_value(cols.colID));
    deck.width = std::stod(widthInput->get_text());
    try {
      deck.width = std::stod(widthInput->get_text());
    } catch (std::exception& e) {}
    deck.update();
  });
  heightInput->signal_changed().connect([=]() {
    auto sel = deckList->get_selection()->get_selected();
    if (!sel) return;
    auto deck = DB::get_deck(sel->get_value(cols.colID));
    try {
      deck.height = std::stod(heightInput->get_text());
    } catch (std::exception& e) {}
    deck.update();
  });
  deckColor->signal_color_set().connect([=]() {
    auto sel = deckList->get_selection()->get_selected();
    if (!sel) return;
    auto deck  = DB::get_deck(sel->get_value(cols.colID));
    deck.color = deckColor->get_color().to_string();
    deck.update();
  });
}
