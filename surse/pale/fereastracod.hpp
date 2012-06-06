#ifndef _FEREASTRACOD_HPP_
#define _FEREASTRACOD_HPP_

#include <gtkmm.h>
//#include <gtksourceviewmm.h>

class FereastraCod : public Gtk::Window {
public:
  enum class LimbajCunoscut {C, ASM};
  enum class StareBtBoxComplet {COLAPSAT, EXPANDAT};

  FereastraCod(LimbajCunoscut lc);
  ~FereastraCod();

protected:
  StareBtBoxComplet mStareBtBox;
  Gtk::TextView mSrcView;
  Gtk::Grid mCodBox;
  Gtk::Button mSagExpandator;
  Gtk::VButtonBox mBtBoxComplet;
  Gtk::VButtonBox mBtBoxMijloc;
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

  Gtk::Image mImgExpand;
  Gtk::Image mImgColaps;
  Gtk::Image mImgScrieAle;
  Gtk::Image mImgSalveaza;
  Gtk::Image mImgReiaCod;
  Gtk::Image mImgCitesteEEPROM;
  Gtk::Image mImgParasesteForm;

  void incarcaImaginiFormular();
  void incarcaButoaneFormular();
  void laClicExpandator();
  void laClicIesire();
};

#endif
