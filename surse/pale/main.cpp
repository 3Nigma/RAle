#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define _WIN_BUILD_
#endif

#include "fereastraprincipala.hpp"

int main(int argc, char *argv[]) {
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "rale.tuscale.ro");
  FereastraPrincipala ferPrin;

  // set various app settings
  Gtk::Settings::get_default()->property_gtk_button_images() = true;

  return app->run(ferPrin);
}
