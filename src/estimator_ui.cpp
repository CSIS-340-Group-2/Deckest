#include <gtkmm-3.0/gtkmm.h>

void create_estimator(Gtk::Builder* builder, int deckID) {
  Gtk::Window* window;
  builder->get_widget("root", window);
  builder->get_application()->add_window(*window);
  window->show();
}
