#include <iostream>

#include <gtkmm-3.0/gtkmm.h>

using namespace std;

int main(int argc, char** argv) {
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
