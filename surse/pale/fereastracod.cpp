#include "fereastracod.hpp"

FereastraCod::FereastraCod(LimbajCunoscut lc)
  : mStareBtBox(StareBtBoxComplet::COLAPSAT),
    mCodBox(), mSrcView(), mSagExpandator(),
    mBtBoxComplet(), mBtBoxMijloc(Gtk::BUTTONBOX_CENTER), 
    mBtIncarcaPeAle("Încarcă pe Ale"),
    mBtSalveazaLucru("Salveaza lucrul"),
    mBtReiaLucru("Reia Cod"),
    mBtCitesteEEPROM("Citește EEPROM"),
    mBtParasesteForm("Părăsește formular"),
    mStatusStrip(),
    mLblStLegatura("[prezență plăcuță]"),
    mLblStDimCod("[dimens. cod]"),
    mLblStNume("[nume cod]") {
  incarcaImaginiFormular();
  this->set_border_width(5);
  this->set_default_size(400, 200);

  switch(lc) {
  case LimbajCunoscut::C:
    this->set_title("[pAle] Cod C");
    break;
  case LimbajCunoscut::ASM:
    this->set_title("[pAle] Cod ASM");
    break;
  default:
    break;
  }

  mSrcBuffer = Gtk::TextBuffer::create();
  mSrcView.set_buffer(mSrcBuffer);
  mSrcView.set_size_request(300, 400);
  mSrcView.set_hexpand(true);

  incarcaButoaneFormular();

  mStatusStrip.add(mLblStLegatura);
  mStatusStrip.add(mLblStDimCod);
  mStatusStrip.add(mLblStNume);
 
  mCodBox.set_vexpand(true);
  mCodBox.attach(mSrcView, 0, 0, 1, 1);
  mCodBox.attach_next_to(mSagExpandator, mSrcView, Gtk::POS_RIGHT, 1, 1);
  mCodBox.attach_next_to(mBtBoxComplet, mSagExpandator, Gtk::POS_RIGHT, 1, 1);
  mCodBox.attach(mStatusStrip, 0, 1, 3, 1);
  this->add(mCodBox); 
  this->show_all();

  mBtBoxComplet.hide();
}

FereastraCod::~FereastraCod() {
}

void FereastraCod::incarcaImaginiFormular() {
  mImgExpand.set("./media/bt_icoana_expandeaza.png");
  mImgExpand.set(mImgExpand.get_pixbuf()->scale_simple(16, 16, Gdk::INTERP_HYPER));
  mImgColaps.set("./media/bt_icoana_colapseaza.png");
  mImgColaps.set(mImgColaps.get_pixbuf()->scale_simple(16, 16, Gdk::INTERP_HYPER));
  mImgScrieAle.set("./media/bt_icoana_trimite_la_ale.png");
  mImgScrieAle.set(mImgScrieAle.get_pixbuf()->scale_simple(16, 16, Gdk::INTERP_HYPER));
  mImgSalveaza.set("./media/bt_icoana_salveaza.png");
  mImgSalveaza.set(mImgSalveaza.get_pixbuf()->scale_simple(16, 16, Gdk::INTERP_HYPER));
  mImgReiaCod.set("./media/bt_icoana_reia.png");
  mImgReiaCod.set(mImgReiaCod.get_pixbuf()->scale_simple(16, 16, Gdk::INTERP_HYPER));
  mImgCitesteEEPROM.set("./media/bt_icoana_eeprom.png");
  mImgCitesteEEPROM.set(mImgCitesteEEPROM.get_pixbuf()->scale_simple(16, 16, Gdk::INTERP_HYPER));
  mImgParasesteForm.set("./media/bt_icoana_paraseste.png");
  mImgParasesteForm.set(mImgParasesteForm.get_pixbuf()->scale_simple(16, 16, Gdk::INTERP_HYPER));
}

void FereastraCod::incarcaButoaneFormular() {
  mSagExpandator.set_image(mImgExpand);
  mSagExpandator.set_relief(Gtk::RELIEF_NONE);
  mSagExpandator.set_size_request(10, 50);
  mSagExpandator.set_vexpand(true);
  mSagExpandator.signal_clicked().connect(sigc::mem_fun(*this, &FereastraCod::laClicExpandator));

  mBtIncarcaPeAle.set_image(mImgScrieAle);
  
  mBtSalveazaLucru.set_image(mImgSalveaza);
  
  mBtReiaLucru.set_image(mImgReiaCod);

  mBtCitesteEEPROM.set_image(mImgCitesteEEPROM);

  mBtParasesteForm.set_image(mImgParasesteForm);

  mBtBoxComplet.add(mBtIncarcaPeAle);
  mBtBoxMijloc.add(mBtSalveazaLucru);
  mBtBoxMijloc.add(mBtReiaLucru);
  mBtBoxMijloc.add(mBtCitesteEEPROM);
  mBtBoxComplet.add(mBtBoxMijloc);
  mBtBoxComplet.add(mBtParasesteForm);
}

void FereastraCod::laClicExpandator() {
  if(mStareBtBox == StareBtBoxComplet::COLAPSAT) {
    mBtBoxComplet.show();
    mSagExpandator.set_image(mImgColaps);
    mStareBtBox = StareBtBoxComplet::EXPANDAT;
  } else if(mStareBtBox == StareBtBoxComplet::EXPANDAT) {
    mBtBoxComplet.hide();
    mSagExpandator.set_image(mImgExpand);
    mStareBtBox = StareBtBoxComplet::COLAPSAT;
  }
}
