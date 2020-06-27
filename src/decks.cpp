#include "creation_callbacks.hpp"

#include "db.hpp"

#include <iostream>
#include <optional>

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

template<typename T>
std::optional<double> parse_double(const T& input) {
  std::optional<double> res;
  try {
    res = std::stod(input);
  } catch (std::exception& e) {}

  return res;
}

void create_decks(Gtk::Builder* builder) {
  Gtk::TreeView*    deckList;
  Gtk::Entry *      nameInput, *lengthInput, *widthInput, *heightInput;
  Gtk::CheckButton *hasRailing, *hasStairs;
  Gtk::ColorButton* deckColor;
  Gtk::Button *     addBtn, *delBtn, *estBtn;
  Gtk::ComboBox*    angle;

  builder->get_widget("addDecks", addBtn);
  builder->get_widget("delDecks", delBtn);
  builder->get_widget("deckList", deckList);
  builder->get_widget("nameInput", nameInput);
  builder->get_widget("lengthInput", lengthInput);
  builder->get_widget("widthInput", widthInput);
  builder->get_widget("heightInput", heightInput);
  builder->get_widget("deckColor", deckColor);
  builder->get_widget("hasRailing", hasRailing);
  builder->get_widget("hasStairs", hasStairs);
  builder->get_widget("angle", angle);
  builder->get_widget("estBtn", estBtn);
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

  auto reset_decklist = [=]() {
    auto decks = DB::get_decks();
    model->clear();
    for (auto deck : decks) {
      auto row          = *model->append();
      row[cols.colID]   = deck.id;
      row[cols.colName] = deck.name;
    }
  };

  reset_decklist();

  auto get_selected = [=]() {
    std::optional<Deck> res;
    auto                sel = deckList->get_selection()->get_selected();
    if (sel) { res = DB::get_deck(sel->get_value(cols.colID)); }

    return res;
  };

  // deckList handlers
  deckList->get_selection()->signal_changed().connect([=]() {
    auto selected = get_selected();
    if (!selected.has_value()) return;


    cerr << "Selected deck " << selected->name << ", retrieving...\n";

    nameInput->set_text(selected->name);
    lengthInput->set_text(std::to_string(selected->length));
    widthInput->set_text(std::to_string(selected->width));
    heightInput->set_text(std::to_string(selected->height));
    deckColor->set_color(Gdk::Color(selected->color));
    hasStairs->set_active(selected->hasStairs);
    hasRailing->set_active(selected->hasRail);
    angle->set_active_id(std::to_string(selected->angle));
  });

  // Input handlers

  nameInput->signal_changed().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;

    deck->name = nameInput->get_text();
    // Update the decklist entry
    auto sel = deckList->get_selection()->get_selected();
    sel->set_value(cols.colName, deck->name);

    deck->update();
  });

  lengthInput->signal_changed().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;

    auto val = parse_double(lengthInput->get_text());
    if (val.has_value()) {
      deck->length = val.value();
      deck->update();
    }
  });
  widthInput->signal_changed().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;

    auto val = parse_double(widthInput->get_text());
    if (val.has_value()) {
      deck->length = val.value();
      deck->update();
    }
  });
  heightInput->signal_changed().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;
    auto val = parse_double(heightInput->get_text());
    if (val.has_value()) {
      deck->length = val.value();
      deck->update();
    }
  });
  deckColor->signal_color_set().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;
    deck->color = deckColor->get_color().to_string();
    deck->update();
  });

  hasRailing->signal_toggled().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;
    deck->hasRail = hasRailing->get_active();
    deck->update();
  });

  hasStairs->signal_toggled().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;
    deck->hasStairs = hasStairs->get_active();
    deck->update();
  });

  angle->signal_changed().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;
    deck->angle = std::stoi(angle->get_active_id());
    deck->update();
  });

  addBtn->signal_clicked().connect([=]() {
    auto newDeck = DB::new_deck();
    auto newRow  = *model->append();
    newRow.set_value(cols.colName, newDeck.name);
    newRow.set_value(cols.colID, newDeck.id);
  });
  delBtn->signal_clicked().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;
    deck->remove();
    reset_decklist();
  });
  estBtn->signal_clicked().connect([=]() {
    auto deck = get_selected();
    if (!deck.has_value()) return;
    auto builder = Gtk::Builder::create_from_file("ui/estimator.glade");
    create_estimator(builder.get(), deck->id);
  });
}
