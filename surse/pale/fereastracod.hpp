#ifndef _FEREASTRACOD_HPP_
#define _FEREASTRACOD_HPP_

#include <gtkmm.h>
//#include <gtksourceviewmm.h>

class FereastraCod : public Gtk::Window {
public:
  enum class LimbajCunoscut {C, ASM};

  FereastraCod(LimbajCunoscut lc);
  ~FereastraCod();

protected:
  Gtk::TextView mSrcView;
  Gtk::Grid mCodBox;
  Gtk::Button mSagExpandator;
  Gtk::VButtonBox mBtBox;
  Gtk::Button mBtIncarcaPeAle;
  Gtk::Button mBtSalveazaLucru;
  Gtk::Button mBtReiaLucru;
  Gtk::Button mBtCitesteEEPROM;
  Gtk::Button mBtParasesteForm;
  Gtk::HBox mStatusStrip;
  Gtk::Label mLblStLegatura;
  Gtk::Label mLblStDimCod;
  Gtk::Label mLblStNume;
  Glib::RefPtr<Gtk::TextBuffer> mSrcBuffer;

  void laClicExpandator();
};

#endif
