#include "fereastracod.hpp"

FereastraCod::FereastraCod(LimbajCunoscut lc)
  : mCodBox(), mSrcView(), mSagExpandator("<"),
    mBtBox(Gtk::BUTTONBOX_CENTER), 
    mBtIncarcaPeAle("Incarcă pe Ale"),
    mBtSalveazaLucru("Salveaza lucrul"),
    mBtReiaLucru("Reia Cod"),
    mBtCitesteEEPROM("Citește EEPROM"),
    mBtParasesteForm("Părăsește formular"),
    mStatusStrip(),
    mLblStLegatura("[prezență plăcuță]"),
    mLblStDimCod("[dimens. cod]"),
    mLblStNume("[nume cod]") {
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

  //Gtk::Image expanImg(Gtk::Stock::GO_UP, Gtk::ICON_SIZE_BUTTON);
  //mSagExpandator.set_image(expanImg);
  mSagExpandator.set_size_request(10, 50);
  mSagExpandator.set_vexpand(true);
  mSagExpandator.signal_clicked().connect(sigc::mem_fun(*this, &FereastraCod::laClicExpandator));

  mBtBox.add(mBtIncarcaPeAle);
  mBtBox.add(mBtSalveazaLucru);
  mBtBox.add(mBtReiaLucru);
  mBtBox.add(mBtCitesteEEPROM);
  mBtBox.add(mBtParasesteForm);

  mStatusStrip.add(mLblStLegatura);
  mStatusStrip.add(mLblStDimCod);
  mStatusStrip.add(mLblStNume);
 
  mCodBox.set_vexpand(true);
  mCodBox.attach(mSrcView, 0, 0, 1, 1);
  mCodBox.attach_next_to(mSagExpandator, mSrcView, Gtk::POS_RIGHT, 1, 1);
  mCodBox.attach_next_to(mBtBox, mSagExpandator, Gtk::POS_RIGHT, 1, 1);
  mCodBox.attach(mStatusStrip, 0, 1, 3, 1);
  this->add(mCodBox); 
  this->show_all();

  mBtBox.hide();
}

FereastraCod::~FereastraCod() {
}

void FereastraCod::laClicExpandator() {
  if(mSagExpandator.get_label() == "<") {
    mBtBox.show();
    mSagExpandator.set_label(">");
  } else if(mSagExpandator.get_label() == ">") {
    mBtBox.hide();
    mSagExpandator.set_label("<");
  }
}
