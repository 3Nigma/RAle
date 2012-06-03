#include <gtkmm/application.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

class FereastraPrincipala : public Gtk::Window {
public:
  FereastraPrincipala() {
  }

  void laClicProgrameaza() {
  }
  void laClicExemple() {
  }
  void laClicInfo() {
  }
  void laClickIesire() {
  }
};

int main(int argc, char *argv[]) {
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  FereastraPrincipala ferPrin;

  //Shows the window and returns when it is closed.
  return app->run(ferPrin);
}
