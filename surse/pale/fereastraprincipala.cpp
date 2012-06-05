#include "fereastraprincipala.hpp"
#include <iostream>

const Glib::ustring FereastraPrincipala::LimbajProgramator_C = "C_LANG";
const Glib::ustring FereastraPrincipala::LimbajProgramator_ASM = "ASM_LANG";

FereastraPrincipala::FereastraPrincipala()
  : mImgTuscaleLogo(Gdk::Pixbuf::create_from_file("tuscale_small_logo.png")),
    mEvBxTuscaleLogo(),
    mMainWinBox(Gtk::ORIENTATION_VERTICAL),
    mCmbxProgrameaza(), mCmbxExemple(),
    mBtInfo("Ce avem aici?"), mBtIesire("Gata, am ieșit!") {
  this->set_title("pAle");
  this->set_border_width(5);
  this->set_resizable(false);
  this->set_position(Gtk::WIN_POS_CENTER);
  
  // creem imaginea ce la semnal de clic -> deschide pagina tuscale
  mEvBxTuscaleLogo.add(mImgTuscaleLogo);
  mEvBxTuscaleLogo.set_events(Gdk::BUTTON_PRESS_MASK);
  mEvBxTuscaleLogo.signal_button_press_event().connect(sigc::mem_fun(*this, &FereastraPrincipala::laClicLogo));
  
  // cream lista de optiuni pentru programator
  mCmbxProgrameaza.append("Hai la programare...");
  mCmbxProgrameaza.append(FereastraPrincipala::LimbajProgramator_C, "... în C");
  mCmbxProgrameaza.append(FereastraPrincipala::LimbajProgramator_ASM, "... în Asamblare");
  mCmbxProgrameaza.set_active(0);
  mCmbxProgrameaza.signal_changed().connect(sigc::mem_fun(*this, &FereastraPrincipala::laClicProgrameaza));
  
  // adaugam elementele la cutia de elemente grafice
  mMainWinBox.set_spacing(4);
  mMainWinBox.add(mEvBxTuscaleLogo);
  mMainWinBox.add(mCmbxProgrameaza);
  mMainWinBox.add(mCmbxExemple);
  mMainWinBox.add(mBtInfo);
  mMainWinBox.add(mBtIesire); 
  this->add(mMainWinBox);
  
  mBtIesire.signal_clicked().connect(sigc::mem_fun(*this, &FereastraPrincipala::laClicIesire));
  
  mMainWinBox.show_all();   
}

FereastraPrincipala::~FereastraPrincipala() {
}

bool FereastraPrincipala::laClicLogo(GdkEventButton* event) {
#ifdef _WIN_BUILD_
  ShellExecute(NULL, "open", "http://tuscale.ro", NULL, NULL, SW_SHOWNORMAL);
#else
    system("xdg-open 'http://tuscale.ro' &");
#endif
    return true;
}

void FereastraPrincipala::laClicProgrameaza() {
  Gtk::TreeModel::iterator iter = mCmbxProgrameaza.get_active();
  if(iter && mCmbxProgrameaza.get_active_id() != "") {
    FereastraCod *fc;
    if(mCmbxProgrameaza.get_active_id() == FereastraPrincipala::LimbajProgramator_C)
      fc = new FereastraCod(FereastraCod::LimbajCunoscut::C);
    else if(mCmbxProgrameaza.get_active_id() == FereastraPrincipala::LimbajProgramator_ASM)
      fc = new FereastraCod(FereastraCod::LimbajCunoscut::ASM);
  
    fc->show();
    mCmbxProgrameaza.set_active(0);
  }
}

void FereastraPrincipala::laClicExemple() {
}

void FereastraPrincipala::laClicInfo() {
}

void FereastraPrincipala::laClicIesire() {
  Gtk::MessageDialog msgIesire("Chiar doriți să părăsiți aplicația?", false,
			       Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
  
  msgIesire.set_title("Plecați?");
  msgIesire.add_button("Da", static_cast<int>(DlgIdRaspunsIntrebare::DA));
  msgIesire.add_button("Nu, mai stau...", static_cast<int>(DlgIdRaspunsIntrebare::NU));
  if(msgIesire.run() == static_cast<int>(DlgIdRaspunsIntrebare::DA))
    hide();
}
