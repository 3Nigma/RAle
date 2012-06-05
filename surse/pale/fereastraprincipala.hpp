#ifndef _FEREASTRAPRINCIPALA_HPP_
#define _FEREASTRAPRINCIPALA_HPP_

#include <gtkmm.h>
#include "fereastracod.hpp"

#ifdef _WIN_BUILD_
#include <windows.h>
#endif

class FereastraPrincipala : public Gtk::Window {
public:
  enum class DlgIdRaspunsIntrebare { DA = 0, NU };
  static const Glib::ustring LimbajProgramator_C;
  static const Glib::ustring LimbajProgramator_ASM;

  FereastraPrincipala();
  ~FereastraPrincipala();

protected:
  Gtk::Image mImgTuscaleLogo;
  Gtk::EventBox mEvBxTuscaleLogo;
  Gtk::Box mMainWinBox;
  Gtk::ComboBoxText mCmbxProgrameaza;
  Gtk::ComboBoxText mCmbxExemple;
  Gtk::Button mBtInfo;
  Gtk::Button mBtIesire;

  virtual bool laClicLogo(GdkEventButton* event);
  virtual void laClicProgrameaza();
  virtual void laClicExemple();
  virtual void laClicInfo();
  virtual void laClicIesire();
};

#endif
