/****************************************************************
 * file window_qrps.cc
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *
 *      It holds the Qt5 code related to the Qt5 top windows
 *
 * Author(s):
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *      Nimesh Neema <nimeshneema@gmail.com>
 *
 *      © Copyright 2019 - 2020 The Reflective Persistent System Team
 *      team@refpersys.org & http://refpersys.org/
 *
 * License:
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "refpersys.hh"
#include "qthead_qrps.hh"


extern "C" const char rps_window_gitid[];
const char rps_window_gitid[]= RPS_GITID;

extern "C" const char rps_window_date[];
const char rps_window_date[]= __DATE__;


RpsQPixMap* RpsQPixMap::m_instance = nullptr;


RpsQWindow::RpsQWindow (QWidget *parent)
  : QMainWindow(parent),
    m_menu_bar(this)

{
  qApp->setAttribute (Qt::AA_DontShowIconsInMenus, false);

  auto vbox = new QVBoxLayout();
  vbox->setSpacing(1);
  vbox->addWidget(menuBar());

  setup_debug_widget();
  vbox->addWidget(&m_debug_widget);
} // end RpsQWindow::RpsQWindow


void
RpsQWindow::setup_debug_widget()
{
  m_debug_widget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_debug_widget.setReadOnly(true);
  m_debug_widget.setTextInteractionFlags(
    m_debug_widget.textInteractionFlags() | Qt::TextSelectableByKeyboard
  );
}


void
RpsQWindow::setup_debug_timer()
{
  connect(&m_debug_timer, SIGNAL(timeout()), this, SLOT(update_debug_widget()));
}


void
RpsQWindow::update_debug_widget()
{
  QFile log ("_refpersys.log");

  if (log.open (QFile::ReadOnly | QFile::Text))
    {
      m_debug_widget.setPlainText (log.readAll());
      m_debug_widget.show();
      log.close();
    }

  else
    {
      qDebug() << "Failed to open debug log";
      return;
    }
}


RpsQMenuAction::RpsQMenuAction(
  RpsQWindow* parent,
  RpsQWindowMenu menu,
  std::string icon,
  std::string title,
  std::string shortcut
)
  : m_parent(parent)
{
  auto pix = RpsQPixMap::instance()->get(icon);
  auto action = new QAction(pix, title.c_str(), m_parent);
  action->setShortcut(tr(shortcut.c_str()));

  auto item = m_parent->menuBar()->findChildren<QMenu*>().at(menu);
  item->addAction(action);
  m_parent->connect(
    action,
    &QAction::triggered,
    this,
    &RpsQMenuAction::on_trigger
  );
}


void RpsQMenuHelpAbout::on_trigger()
{
  std::ostringstream msg;
  msg << "RefPerSys Git ID: " << RpsColophon::git_id()
      << "\nBuild Date: " << RpsColophon::timestamp()
      << "\nMD5 Sum of Source: " << RpsColophon::source_md5()
      << "\nLast Git Commit: " << RpsColophon::last_git_commit()
      << "\nRefPerSys Top Directory: " << RpsColophon::top_directory()
      << "\n\nSee " << RpsColophon::website();

  QMessageBox::information (window(), "About RefPerSys", msg.str().c_str());
}


void RpsQMenuHelpDebug::on_trigger()
{
  auto wnd = window();
  wnd->m_debug_timer.start(1000);
  wnd->update_debug_widget();
}


void RpsQMenuAppQuit::on_trigger()
{
  auto msg = QString("Are you sure you want to quit without dumping?");
  auto btn = QMessageBox::Yes | QMessageBox::No;
  auto reply = QMessageBox::question(window(), "RefPerSys", msg, btn);

  if (reply == QMessageBox::Yes)
    QApplication::quit();
}


void RpsQMenuAppExit::on_trigger()
{
  rps_dump_into();
  QApplication::quit();
}


void RpsQMenuAppClose::on_trigger()
{
  auto app = window()->application();

  if (app->getWindowCount() > 1)
    {
      app->lowerWindowCount();
      window()->close();
    }
  else
    {
      auto msg = QString("Are you sure you want to quit without dumping?");
      auto btn = QMessageBox::Yes | QMessageBox::No;
      auto reply = QMessageBox::question(window(), "RefPerSys", msg, btn);

      if (reply == QMessageBox::Yes)
        QApplication::quit();
    }
}


void RpsQMenuAppDump::on_trigger()
{
  rps_dump_into();
}


void RpsQMenuAppGC::on_trigger()
{
  rps_garbage_collect();
}


void RpsQMenuAppNew::on_trigger()
{
  window()->application()->add_new_window();
}


void
RpsQMenuCreateClass::on_trigger()
{
  auto dia = new RpsQCreateClassDialog(window());
  dia->show();
}

void
RpsQMenuCreateSymbol::on_trigger()
{
  auto dia = new RpsQCreateSymbolDialog(window());
  dia->show();
}


RpsQWindowMenuBar::RpsQWindowMenuBar(RpsQWindow* parent)
  : menubar_parent(parent)
{
  auto app_menu = menubar_parent->menuBar()->addMenu("&App");
  m_menu_app_dump = std::make_shared<RpsQMenuAppDump>(menubar_parent);
  m_menu_app_gc = std::make_shared<RpsQMenuAppGC>(menubar_parent);
  m_menu_app_new = std::make_shared<RpsQMenuAppNew>(menubar_parent);
  app_menu->addSeparator();
  m_menu_app_close = std::make_shared<RpsQMenuAppClose>(menubar_parent);
  m_menu_app_quit = std::make_shared<RpsQMenuAppQuit>(menubar_parent);
  m_menu_app_exit = std::make_shared<RpsQMenuAppExit>(menubar_parent);

  menubar_parent->menuBar()->addMenu("&Create");
  m_menu_create_class = std::make_shared<RpsQMenuCreateClass>(menubar_parent);
  m_menu_create_symbol = std::make_shared<RpsQMenuCreateSymbol>(menubar_parent);

  menubar_parent->menuBar()->addMenu("&Help");
  m_menu_help_about = std::make_shared<RpsQMenuHelpAbout>(menubar_parent);
  m_menu_help_debug = std::make_shared<RpsQMenuHelpDebug>(menubar_parent);

  menubar_parent->menuBar()->setSizePolicy(
    QSizePolicy::Expanding,
    QSizePolicy::Expanding
  );
}// end of RpsQWindowMenuBar::RpsQWindowMenuBar



////////////////////////////////////////////////////////////////
//// the dialog to create RefPerSys classes

RpsQCreateClassDialog::RpsQCreateClassDialog(RpsQWindow* parent)
  : QDialog(parent),
    dialog_vbox(),
    superclass_hbox(),
    superclass_label("superclass:", this),
    superclass_linedit("", "super", this),
    classname_hbox(),
    classname_label("class name:", this),
    classname_linedit(this),
    button_hbox(),
    ok_button("Create Class", this),
    cancel_button("cancel", this)
{
  // set widget names, useful for debugging, and later for style sheets.
  setObjectName("RpsQCreateClassDialog");
  dialog_vbox.setObjectName("RpsQCreateClassDialog_dialog_vbox");
  superclass_hbox.setObjectName("RpsQCreateClassDialog_dialog_vbox");
  superclass_label.setObjectName("RpsQCreateClassDialog_superclass_label");
  superclass_linedit.setObjectName("RpsQCreateClassDialog_superclass_linedit");
  classname_hbox.setObjectName("RpsQCreateClassDialog_classname_hbox");
  classname_label.setObjectName("RpsQCreateClassDialog_classname_label");
  classname_linedit.setObjectName("RpsQCreateClassDialog_classname_linedit");
  button_hbox.setObjectName("RpsQCreateClassDialog_button_hbox");
  ok_button.setObjectName("RpsQCreateClassDialog_ok_button");
  cancel_button.setObjectName("RpsQCreateClassDialog_cancel_button");
  RPS_INFORMOUT("RpsQCreateClassDialog @" << this);
  // set fonts of labels and linedits
  {
    auto labfont = QFont("Arial", 12);
    superclass_label.setFont(labfont);
    classname_label.setFont(labfont);
    auto editfont = QFont("Courier", 12);
    superclass_linedit.setFont(editfont);
    classname_linedit.setFont(editfont);
  }
  // ensure layout; maybe we should use style sheets?
  {
    dialog_vbox.addLayout(&superclass_hbox);
    superclass_hbox.addWidget(&superclass_label);
    superclass_hbox.addSpacing(2);
    superclass_hbox.addWidget(&superclass_linedit);
    dialog_vbox.addLayout(&classname_hbox);
    classname_hbox.addWidget(&classname_label);
    classname_hbox.addSpacing(2);
    classname_hbox.addWidget(&classname_linedit);
    dialog_vbox.addLayout(&button_hbox);
    button_hbox.addWidget(&ok_button);
    button_hbox.addSpacing(3);
    button_hbox.addWidget(&cancel_button);
    setLayout(&dialog_vbox);
  }
  // define behavior
  {
    connect(
      &ok_button,
      &QAbstractButton::clicked,
      this,
      &RpsQCreateClassDialog::on_ok_trigger
    );
    connect(
      &cancel_button,
      &QAbstractButton::clicked,
      this,
      &RpsQCreateClassDialog::on_cancel_trigger
    );
  }
} // end of RpsQCreateClassDialog::RpsQCreateClassDialog




RpsQCreateClassDialog::~RpsQCreateClassDialog()
{
} // end RpsQCreateClassDialog::~RpsQCreateClassDialog


void RpsQCreateClassDialog::on_ok_trigger()
{
  RPS_LOCALFRAME(Rps_ObjectRef(nullptr),//descriptor
                 nullptr,//parentframe
                 Rps_ObjectRef obsymb;
                 Rps_ObjectRef obnewclass;
                 Rps_ObjectRef obsuperclass;
                );
  // create new class
  std::string strsuperclass = superclass_linedit.text().toStdString();
  std::string strclassname = classname_linedit.text().toStdString();
  RPS_WARNOUT("untested RpsQCreateClassDialog::on_ok_trigger strsuperclass="
              << strsuperclass
              << ", strclassname=" << strclassname);
  try
    {
      _.obsuperclass = Rps_ObjectRef::find_object(&_, strsuperclass);
      RPS_INFORMOUT("RpsQCreateClassDialog::on_ok_trigger obsuperclass=" << _.obsuperclass);
      _.obnewclass = Rps_ObjectRef::make_named_class(&_, _.obsuperclass, strclassname);
      RPS_INFORMOUT("RpsQCreateClassDialog::on_ok_trigger obnewclass=" << _.obnewclass);
      std::ostringstream outs;
      outs << "created new class " << _.obnewclass << " named " << strclassname
           << " of superclass " << _.obsuperclass;
      std::string msg = outs.str();
      QMessageBox::information(parentWidget(), "Created Class", msg.c_str());
    }
  catch (const std::exception& exc)
    {
      RPS_WARNOUT("RpsQCreateClassDialog::on_ok_trigger exception " << exc.what());
      std::ostringstream outs;
      outs << "failed to create class named "
           << strclassname << " with superclass " << strsuperclass
           << std::endl;
      outs<< exc.what();
      QMessageBox::warning(parentWidget(), "Failed class creation", outs.str().c_str());
    }
  deleteLater(); // was close()
}		 // end RpsQCreateClassDialog::on_ok_trigger


void
RpsQCreateClassDialog::on_cancel_trigger()
{
  deleteLater(); // was close()
} // end RpsQCreateClassDialog::on_cancel_trigger



////////////////////////////////////////////////////////////////

/// the dialog to create RefPerSys symbols
RpsQCreateSymbolDialog::RpsQCreateSymbolDialog(RpsQWindow* parent)
  : QDialog(parent),
    sydialog_vbox(),
    syname_hbox(),
    syname_label("new symbol name:", this),
    syname_linedit(this),
    syname_weakchkbox("weak?", this),
    button_hbox(),
    ok_button("Create Symbol", this),
    cancel_button("cancel", this)
{
  // set widget names, useful for debugging, and later for style sheets.
  setObjectName("RpsQCreateSymbolDialog");
  sydialog_vbox.setObjectName("RpsQCreateSymbolDialog_sydialog_vbox");
  syname_hbox.setObjectName("RpsQCreateSymbolDialog_syname_hbox");
  syname_label.setObjectName("RpsQCreateSymbolDialog_syname_hbox");
  syname_linedit.setObjectName("RpsQCreateSymbolDialog_syname_linedit");
  syname_weakchkbox.setObjectName("RpsQCreateSymbolDialog_syname_weakchkbox");
  button_hbox.setObjectName("RpsQCreateSymbolDialog_button_hbox");
  ok_button.setObjectName("RpsQCreateSymbolDialog_ok_button");
  cancel_button.setObjectName("RpsQCreateSymbolDialog_cancel_button");
  RPS_INFORMOUT("RpsQCreateSymbolDialog @" << this);
  // set fonts of labels and linedits
  {
    auto labfont = QFont("Arial", 12);
    syname_label.setFont(labfont);
    auto editfont = QFont("Courier", 12);
    syname_linedit.setFont(editfont);
  }
  // ensure layout; maybe we should use style sheets?
  {
    sydialog_vbox.addLayout(&syname_hbox);
    syname_hbox.addWidget(&syname_label);
    syname_hbox.addSpacing(2);
    syname_hbox.addWidget(&syname_linedit);
    syname_hbox.addSpacing(2);
    syname_hbox.addWidget(&syname_weakchkbox);
    sydialog_vbox.addLayout(&button_hbox);
    button_hbox.addWidget(&ok_button);
    button_hbox.addSpacing(3);
    button_hbox.addWidget(&cancel_button);
    setLayout(&sydialog_vbox);
  }
  // define behavior
  {
    connect(
      &ok_button,
      &QAbstractButton::clicked,
      this,
      &RpsQCreateSymbolDialog::on_ok_trigger
    );
    connect(
      &cancel_button,
      &QAbstractButton::clicked,
      this,
      &RpsQCreateSymbolDialog::on_cancel_trigger
    );
  }
} // end RpsQCreateSymbolDialog::RpsQCreateSymbolDialog


RpsQCreateSymbolDialog::~RpsQCreateSymbolDialog()
{
} // end RpsQCreateSymbolDialog::~RpsQCreateSymbolDialog

void
RpsQCreateSymbolDialog::on_ok_trigger()
{
  RPS_LOCALFRAME(Rps_ObjectRef(nullptr),//descriptor
                 nullptr,//parentframe
                 Rps_ObjectRef obsymb;
                );
  std::string strsyname = syname_linedit.text().toStdString();
  RPS_WARNOUT("RpsQCreateSymbolDialog::on_ok_trigger strsyname=" << strsyname);
  try
    {
      bool isweak = syname_weakchkbox.isChecked();
      _.obsymb = Rps_ObjectRef::make_new_symbol(&_, strsyname, isweak);
      RPS_INFORMOUT("RpsQCreateSymbolDialog::on_ok_trigger created symbol " << _.obsymb
                    << " named " << strsyname);
      if (!_.obsymb)
        throw std::runtime_error(std::string("failed to create symbol:") + strsyname);
      if (!isweak)
        rps_add_root_object(_.obsymb);
      _.obsymb->put_space(Rps_ObjectRef::root_space());
      std::ostringstream outs;
      outs << "created new symbol " << _.obsymb << " named " << strsyname;
      std::string msg = outs.str();
      QMessageBox::information(parentWidget(), "Created Symbol", msg.c_str());
    }
  catch (const std::exception& exc)
    {
      RPS_WARNOUT("RpsQCreateSymbolDialog::on_ok_trigger exception " << exc.what());
      std::ostringstream outs;
      outs << "failed to create symbol named "
           << strsyname
           << std::endl;
      outs<< exc.what();
      QMessageBox::warning(parentWidget(), "Failed symbol creation", outs.str().c_str());
    }
  deleteLater();
} // end RpsQCreateSymbolDialog::on_ok_trigger

void
RpsQCreateSymbolDialog::on_cancel_trigger()
{
  deleteLater(); // was close()
} // end RpsQCreateSymbolDialog::on_cancel_trigger

////////////////////////////////////////////////////////////////

///// the completer for RefPerSys objects
RpsQObjectCompleter::RpsQObjectCompleter(QObject*parent)
  : QCompleter(parent),
    qobcompl_strlistmodel(this)
{
  setModel(&qobcompl_strlistmodel);
#warning incomplete RpsQObjectCompleter::RpsQObjectCompleter
  RPS_WARN("incomplete RpsQObjectCompleter::RpsQObjectCompleter");
} // end RpsQObjectCompleter::RpsQObjectCompleter

void
RpsQObjectCompleter::update_for_text(const QString&qstr)
{
  std::string str = qstr.toStdString();
  qobcompl_strlistmodel.setStringList(QStringList{});
  if (str.size() <= 2)
    {
      return;
    }
  else if (str[0] == '_' && isdigit(str[1]))
    {
      QStringList qslist;
      auto stopfun = [=,&qslist](const Rps_ObjectZone*obz)
      {
        RPS_ASSERT(obz);
        if (qslist.count() > max_nb_autocompletions+2)
          return true;
        Rps_Id obid = obz->oid();
        char idbuf[32];
        memset (idbuf, 0, sizeof(idbuf));
        obid.to_cbuf24(idbuf);
        QString qids(idbuf);
        qslist << qids;
        return false;
      };
      int nbcompl =
        Rps_ObjectZone::autocomplete_oid(str.c_str(), stopfun);
      if (nbcompl <= max_nb_autocompletions)
        qobcompl_strlistmodel.setStringList(qslist);
    }
  else if (isalpha(str[0]))
    {
      QStringList qslist;
      auto stopfun = [=,&qslist](const Rps_ObjectZone*obz, const std::string&nam)
      {
        RPS_ASSERT(obz);
        if (qslist.count() > max_nb_autocompletions+2)
          return true;
        QString qnams(nam.c_str());
        qslist << qnams;
        return false;
      };
      int nbcompl = Rps_PayloadSymbol::autocomplete_name(str.c_str(), stopfun);

      if (nbcompl <= max_nb_autocompletions)
        qobcompl_strlistmodel.setStringList(qslist);
    }
} // end RpsQObjectCompleter::update_for_text


///// the line edit for RefPerSys objects
RpsQObjectLineEdit::RpsQObjectLineEdit(const QString &contents,
                                       const QString& placeholder, QWidget *parent)
  : QLineEdit(contents, parent),
    qoblinedit_completer(nullptr)
{
  setPlaceholderText(placeholder);
  qoblinedit_completer = std::make_unique<RpsQObjectCompleter>(this);
  connect(this, SIGNAL(textEdited(const QString&)),
          qoblinedit_completer.get(), SLOT(update_for_text(const QString&)));
#warning incomplete RpsQObjectLineEdit::RpsQObjectLineEdit
  RPS_WARN("incomplete RpsQObjectLineEdit::RpsQObjectLineEdit");
} // end of RpsQObjectLineEdit::RpsQObjectLineEdit

//////////////////////////////////////// end of file window_qrps.cc

