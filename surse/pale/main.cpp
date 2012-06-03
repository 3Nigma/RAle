#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define _WIN_BUILD_
#endif

#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/window.h>
#include <gtkmm/messagedialog.h>

#ifdef _WIN_BUILD_
#include <windows.h>
#endif

class FereastraPrincipala : public Gtk::Window {
public:
  FereastraPrincipala()
    : Gtk::Window(Gtk::WINDOW_POPUP),mImgTuscaleLogo(Gdk::Pixbuf::create_from_file("tuscale_small_logo.png")),
      mEvBxTuscaleLogo(),
      mMainWinBox(Gtk::ORIENTATION_VERTICAL), 
      mBtInfo("Ce avem aici?"), mBtIesire("Gata, am ieșit!") {
    this->set_title("pAle");
    this->set_border_width(5);

    // creem imaginea ce la semnal de clic -> deschide pagina tuscale
    mEvBxTuscaleLogo.add(mImgTuscaleLogo);
    mEvBxTuscaleLogo.set_events(Gdk::BUTTON_PRESS_MASK);
    mEvBxTuscaleLogo.signal_button_press_event().connect(sigc::mem_fun(*this, &FereastraPrincipala::laClicLogo));

    // adaugam elementele la cutia de elemente grafice
    mMainWinBox.set_spacing(4);
    mMainWinBox.add(mEvBxTuscaleLogo);
    mMainWinBox.add(mBtInfo);
    mMainWinBox.add(mBtIesire); 
    this->add(mMainWinBox);

    mBtIesire.signal_clicked().connect(sigc::mem_fun(*this, &FereastraPrincipala::laClicIesire));

    mMainWinBox.show_all();   
  }

  ~FereastraPrincipala() {
  }

protected:
  Gtk::Image mImgTuscaleLogo;
  Gtk::EventBox mEvBxTuscaleLogo;
  Gtk::Box mMainWinBox;
  Gtk::ComboBoxText mCmbxProgrameaza;
  Gtk::ComboBoxText mCmbxExemple;
  Gtk::Button mBtInfo;
  Gtk::Button mBtIesire;

  virtual bool laClicLogo(GdkEventButton* event) {
#ifdef _WIN_BUILD_
    ShellExecute(NULL, "open", "http://tuscale.ro", NULL, NULL, SW_SHOWNORMAL);
#else
    system("xdg-open 'http://tuscale.ro' &");
#endif
    return true;
  }
  virtual void laClicProgrameaza() {
  }
  virtual void laClicExemple() {
  }
  virtual void laClicInfo() {
  }
  virtual void laClicIesire() {
    Gtk::MessageDialog msgInfo("S-a dat click pe imagine!");
    msgInfo.run();
  }
};

int main(int argc, char *argv[]) {
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "rale.tuscale.ro");
  FereastraPrincipala ferPrin;

  return app->run(ferPrin);
}
