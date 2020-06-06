#include "creation_callbacks.hpp"

#include <iostream>

using namespace std;

void create_decks(Gtk::Builder* builder) {
  Gtk::Button* btn;
  builder->get_widget("button", btn);
  btn->signal_clicked().connect([]() { cerr << "Hello, world!\n"; });
}
