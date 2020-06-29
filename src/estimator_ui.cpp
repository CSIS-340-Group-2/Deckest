#include <gtkmm-3.0/gtkmm.h>

#include "util.hpp"

#include "db.hpp"
#include "estimator.hpp"

using namespace std;

/// For displaying the stuff in "Name $price" format
/// Also used for the WoodType combo
struct CompCols: public Gtk::ListStore::ColumnRecord {
 public:
  Gtk::TreeModelColumn<std::string> colName;
  Gtk::TreeModelColumn<int>         colQuant;
  Gtk::TreeModelColumn<std::string> colPrice;

  CompCols() {
    add(colName);
    add(colQuant);
    add(colPrice);
  }
};

struct TypeCols: public Gtk::ListStore::ColumnRecord {
 public:
  Gtk::TreeModelColumn<int>         colID;
  Gtk::TreeModelColumn<std::string> colName;

  TypeCols() {
    add(colID);
    add(colName);
  }
};

void create_estimator(Gtk::Builder* builder, int deckID) {
  static CompCols compCols;
  Gtk::Window*    window;
  builder->get_widget("root", window);

  Gtk::TreeView* treeView;
  auto           model = Gtk::ListStore::create(compCols);
  builder->get_widget("compList", treeView);
  treeView->set_model(model);

  static TypeCols typeCols;
  auto            typeModel = Gtk::ListStore::create(typeCols);
  for (auto type : DB::get_woodtypes()) {
    auto row              = *typeModel->append();
    row[typeCols.colID]   = type.id;
    row[typeCols.colName] = type.name;
  }
  Gtk::ComboBox* woodType;
  builder->get_widget("pickWoodType", woodType);
  woodType->set_id_column(1);
  woodType->set_active(0);
  woodType->set_model(typeModel);

  auto update = [=]() {
    Deck deck = DB::get_deck(deckID);
    estimate(deck);
    auto orders = deck.orders();
    auto type   = DB::get_woodtype(woodType->get_active()->get_value(typeCols.colID));

    for (auto& order : orders) {
      auto comp              = DB::get_component(order.matID);
      auto row               = *model->append();
      row[compCols.colName]  = comp.get_name();
      row[compCols.colQuant] = order.quantity;
      row[compCols.colPrice] = "$" + double_to_string(comp.get_price(type) * order.quantity);
    }
  };

  try {
    update();
  } catch (std::exception& e) { cerr << "Got exception " << e.what() << endl; }

  builder->get_application()->add_window(*window);
  window->show();
}
