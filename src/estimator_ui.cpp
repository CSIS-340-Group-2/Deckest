#include <gtkmm-3.0/gtkmm.h>

/// For displaying the stuff in "Name      :  $price" format
struct CompCols: public Gtk::ListStore::ColumnRecord {
 public:
  Gtk::TreeModelColumn<std::string> colName;
  Gtk::TreeModelColumn<std::string> colPrice;

  CompCols() {
    add(colName);
    add(colPrice);
  }
};

void create_estimator(Gtk::Builder* builder, int deckID) {
  static CompCols compCols;
  Gtk::Window* window;
  builder->get_widget("root", window);

  Gtk::TreeView* treeView;
  builder->get_widget("compList", treeView);
  auto model = treeView->get_model();



  
  builder->get_application()->add_window(*window);
  window->show();
}
