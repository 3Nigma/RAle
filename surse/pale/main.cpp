#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define _WIN_BUILD_
#endif

#include "fereastraprincipala.hpp"

int main(int argc, char *argv[]) {
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "rale.tuscale.ro");
  FereastraPrincipala ferPrin;

  return app->run(ferPrin);
}
