#include <iostream>

#include <gtkmm-3.0/gtkmm.h>

#include "sqlite_orm.h"

using namespace std;
using namespace sqlite_orm;

struct Deck {
  
};

struct Material {
  
};

struct Employee {
  
};


int main(int argc, char** argv) {

  auto storage = make_storage("db.sqlite"
  );
  
  auto app = Gtk::Application::create(argc, argv, "edu.mnstate.DeckEst");
  auto builder = Gtk::Builder::create_from_file("ui/main.glade");
  Gtk::Window* win;
  builder->get_widget("window", win);


  {
    Gtk::Entry* hello;
    builder->get_widget("hello", hello);
    hello->set_text("Hello, world!");
  }

  return app->run(*win);
}
