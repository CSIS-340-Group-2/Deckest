#include <iostream>

#include <gtkmm-3.0/gtkmm.h>

#include "sqlite_orm/sqlite_orm.h"

using namespace std;
using namespace sqlite_orm;

struct Deck {};

struct Material {};

struct Employee {};

// Keep these synced with main.glade
constexpr int DECKS_PAGE = 0, MATERIALS_PAGE = 1, EMPLOYEES_PAGE = 2, ABOUT_PAGE = 3;

/**
 * Set a notebook's page based on a template from a file
 */
void set_page(Gtk::Notebook* notebook, int n, const char* filename) {
  auto       builder = Gtk::Builder::create_from_file("ui/decks.glade", "root");
  Gtk::Grid* grid;
  builder->get_widget("root", grid);

  auto page = dynamic_cast<Gtk::Alignment*>(notebook->get_nth_page(0));
  page->add(*grid);
}

int main(int argc, char** argv) {
  try {
    auto storage = make_storage("db.sqlite");

    auto app     = Gtk::Application::create(argc, argv, "edu.mnstate.DeckEst");
    auto builder = Gtk::Builder::create_from_file("ui/main.glade");

    Gtk::Window* win;
    builder->get_widget("window", win);

    {
      Gtk::Notebook* notebook;
      builder->get_widget("notebook", notebook);

      // We'll split each tab into its own file so we can easily work independantly
      // Every page's root is a Gtk::Alignment
      // For each file:
      // // The root is a Gtk::Grid
      set_page(notebook, DECKS_PAGE, "ui/decks.glade");
    }
    return app->run(*win);
  } catch (Gtk::BuilderError& e) { cerr << e.what() << std::endl; }
  return -1;
}
