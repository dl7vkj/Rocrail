/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "timedactions.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "timedactions.h"
#include "actionsctrldlg.h"

////@begin XPM images
////@end XPM images

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ActionList.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/ExtCmd.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/Operator.h"

#include "rocview/public/guiapp.h"

#include "rocs/public/node.h"
#include "rocs/public/list.h"


/*!
 * TimedActions type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TimedActions, wxDialog )


/*!
 * TimedActions event table definition
 */

BEGIN_EVENT_TABLE( TimedActions, wxDialog )

////@begin TimedActions event table entries
    EVT_LIST_ITEM_SELECTED( ID_LISTCTRL, TimedActions::OnListctrlSelected )
    EVT_LIST_COL_CLICK( ID_LISTCTRL, TimedActions::OnListctrlColLeftClick )

    EVT_BUTTON( ID_BT_ADD, TimedActions::OnBtAddClick )

    EVT_BUTTON( ID_BT_DEL, TimedActions::OnBtDelClick )

    EVT_BUTTON( ID_BUTTON_AC_DOC, TimedActions::OnButtonAcDocClick )

    EVT_CHOICE( ID_ACTIONS_TYPE, TimedActions::OnActionsTypeSelected )

    EVT_BUTTON( ID_ACTIONS_EXEC_CMD, TimedActions::OnActionsExecCmdClick )

    EVT_LISTBOX( ID_USELIST, TimedActions::OnUselistSelected )
    EVT_LISTBOX_DCLICK( ID_USELIST, TimedActions::OnUselistDoubleClicked )

    EVT_BUTTON( wxID_OK, TimedActions::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, TimedActions::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, TimedActions::OnApplyClick )

////@end TimedActions event table entries

END_EVENT_TABLE()


/*!
 * TimedActions constructors
 */

TimedActions::TimedActions()
{
}


TimedActions::TimedActions( wxWindow* parent )
{
    Init();
    Create(parent, -1, wxGetApp().getMsg("actiontable") );
    m_Props = NULL;

    initLabels();
    initOutputList();
    initIndex();

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    GetSizer()->Layout();

}

void TimedActions::initLabels() {
  m_ActionBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_ActionBook->SetPageText( 1, wxGetApp().getMsg( "definition" ) );
  m_ActionBook->SetPageText( 2, wxGetApp().getMsg( "usage" ) );

  initList(m_TimedActions, this, false, false, false, false, true);
  m_Doc->SetLabel( wxGetApp().getMsg( "doc_report" ) );

    m_Add->SetLabel( wxGetApp().getMsg( "new" ) );
    m_labCommand->SetLabel( wxGetApp().getMsg( "command" ) );
    m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
    m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
    m_labParam->SetLabel( wxGetApp().getMsg( "parameter" ) );
    m_labDuration->SetLabel( wxGetApp().getMsg( "duration" ) );
    m_labTimer->SetLabel( wxGetApp().getMsg( "timer" ) );
    m_labDescription->SetLabel( wxGetApp().getMsg( "description" ) );
    m_ActivationTimeBox->SetLabel( wxGetApp().getMsg( "activationtime" ) );
    m_labHour->SetLabel( wxGetApp().getMsg( "hour" ) );
    m_labMin->SetLabel( wxGetApp().getMsg( "minute" ) );
    m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
    m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
    m_Apply->SetLabel( wxGetApp().getMsg( "apply" ) );
    m_Del->SetLabel( wxGetApp().getMsg( "delete" ) );
    m_Timed->SetLabel( wxGetApp().getMsg( "use" ) );
    m_Timer->SetLabel( wxGetApp().getMsg( "timer" ) );
    m_Every->SetLabel( wxGetApp().getMsg( "every" ) );
    m_Random->SetLabel( wxGetApp().getMsg( "random" ) );
    m_DoubleQuote->SetLabel( wxGetApp().getMsg( "doublequote" ) );

    m_labDescription->SetLabel( wxGetApp().getMsg( "description" ) );
    m_ExecCmd->Enable(false);

    // "co,ext,sw,st,sys,sg,bk,lc,fn,tt,seltab,tx,fb"
    m_Type->Append( wxGetApp().getMsg( "output" ) );
    m_Type->Append( wxGetApp().getMsg( "program" ) );
    m_Type->Append( wxGetApp().getMsg( "turnout" ) );
    m_Type->Append( wxGetApp().getMsg( "route" ) );
    m_Type->Append( wxGetApp().getMsg( "system" ) );
    m_Type->Append( wxGetApp().getMsg( "signal" ) );
    m_Type->Append( wxGetApp().getMsg( "block" ) );
    m_Type->Append( wxGetApp().getMsg( "locomotive" ) );
    m_Type->Append( wxGetApp().getMsg( "function" ) );
    m_Type->Append( wxGetApp().getMsg( "turntable" ) );
    m_Type->Append( wxGetApp().getMsg( "seltab" ) );
    m_Type->Append( wxGetApp().getMsg( "text" ) );
    m_Type->Append( wxGetApp().getMsg( "sensor" ) );
    m_Type->Append( wxGetApp().getMsg( "stagingblock" ) );
}


void TimedActions::initValues() {
  if( m_Props == NULL )
    return;

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("action").mb_str(wxConvUTF8),
      wAction.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // General
  m_ActionID->SetValue( wxString(wAction.getid( m_Props ),wxConvUTF8) );
  m_Description->SetValue( wxString(wAction.getdesc( m_Props ),wxConvUTF8) );

  // "co,ext,sw,st,sys,sg,bk,lc,fn"
  const char* type = wAction.gettype(m_Props);
  if( StrOp.equals( wOutput.name(), type ) )
    m_Type->SetSelection(0);
  else if( StrOp.equals( wExtCmd.name(), type ) )
    m_Type->SetSelection(1);
  else if( StrOp.equals( wSwitch.name(), type ) )
    m_Type->SetSelection(2);
  else if( StrOp.equals( wRoute.name(), type ) )
    m_Type->SetSelection(3);
  else if( StrOp.equals( wSysCmd.name(), type ) )
    m_Type->SetSelection(4);
  else if( StrOp.equals( wSignal.name(), type ) )
    m_Type->SetSelection(5);
  else if( StrOp.equals( wBlock.name(), type ) )
    m_Type->SetSelection(6);
  else if( StrOp.equals( wLoc.name(), type ) )
    m_Type->SetSelection(7);
  else if( StrOp.equals( wFunCmd.name(), type ) )
    m_Type->SetSelection(8);
  else if( StrOp.equals( wTurntable.name(), type ) )
    m_Type->SetSelection(9);
  else if( StrOp.equals( wSelTab.name(), type ) )
    m_Type->SetSelection(10);
  else if( StrOp.equals( wText.name(), type ) )
    m_Type->SetSelection(11);
  else if( StrOp.equals( wFeedback.name(), type ) )
    m_Type->SetSelection(12);
  else if( StrOp.equals( wStage.name(), type ) )
    m_Type->SetSelection(13);

  initOutputList();
  initCommands();

  m_Timed->SetValue(wAction.istimed( m_Props )?true:false);
  m_Hour->SetValue( wAction.gethour( m_Props ) );
  m_Min->SetValue( wAction.getmin( m_Props) );
  m_Random->SetValue(wAction.israndom( m_Props )?true:false);
  m_Every->SetValue(wAction.isevery( m_Props )?true:false);
  m_ActTime->SetValue( wAction.getactiontime( m_Props) );
  m_Command->SetValue( wxString( wAction.getcmd( m_Props), wxConvUTF8) );
  m_Parameter->SetValue( wxString( wAction.getparam( m_Props), wxConvUTF8) );
  m_ID->SetStringSelection( wxString( wAction.getoid( m_Props), wxConvUTF8) );
  m_Timer->SetValue( wAction.gettimer( m_Props) );
  m_DoubleQuote->SetValue( wAction.isdoublequote( m_Props) ? true:false );

  initUse();

}

void TimedActions::initUse() {
  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitUse" );
  m_UseList->Clear();
  // TODO: Update the use tab.
  iONode model = wxGetApp().getModel();
  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "find objects..." );
  for( int i = 0; i < NodeOp.getChildCnt(model); i++ ) {
    iONode list = NodeOp.getChild(model, i);
    for( int n = 0; n < NodeOp.getChildCnt(list); n++ ) {
      iONode child = NodeOp.getChild(list, n);
      iONode ctrl = NodeOp.findNode( child, wActionCtrl.name() );
      while( ctrl != NULL ) {
        if( StrOp.equals( wActionCtrl.getid(ctrl), wAction.getid( m_Props ) ) ) {
          m_UseList->Append( wxString(wItem.getid(child),wxConvUTF8) + _T(" (") + wxString(NodeOp.getName(child),wxConvUTF8) + _T(")"), child );
          break;
        }
        ctrl = NodeOp.findNextNode( child, ctrl );
      }
    }
  }
}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}

bool TimedActions::initIndex() {
  TraceOp.trc( "scdlg", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );

  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode aclist = wPlan.getaclist( model );
    if( aclist != NULL ) {
      fillIndex(aclist);

      if( m_Props != NULL ) {
        setIDSelection(wItem.getid( m_Props ));
        return true;
      }
      else {
        m_Props = setSelection(0);
      }

      if( m_Props != NULL ) {
        initValues();
      }
    }
  }

  return false;
}


bool TimedActions::evaluate() {
  if( m_Props == NULL )
    return false;

  if( m_ActionID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ActionID->SetValue( wxString(wAction.getid( m_Props ),wxConvUTF8) );
    return false;
  }

  wItem.setprev_id(m_Props, wAction.getid( m_Props));
  wAction.setid( m_Props, m_ActionID->GetValue().mb_str(wxConvUTF8) );
  wAction.setdesc( m_Props, m_Description->GetValue().mb_str(wxConvUTF8) );
  wAction.settimed( m_Props, m_Timed->IsChecked() ? True:False );
  wAction.setrandom( m_Props, m_Random->IsChecked() ? True:False );
  wAction.setevery( m_Props, m_Every->IsChecked() ? True:False );
  wAction.sethour( m_Props, m_Hour->GetValue() );
  wAction.setmin( m_Props, m_Min->GetValue() );
  wAction.setcmd( m_Props, m_Command->GetValue().mb_str(wxConvUTF8) );
  wAction.setoid( m_Props, m_ID->GetStringSelection().mb_str(wxConvUTF8) );
  if( StrOp.equals( " ", wAction.getoid( m_Props ) ) )
    wAction.setoid( m_Props, "" );
  wAction.setactiontime( m_Props, m_ActTime->GetValue() );
  wAction.setparam( m_Props, m_Parameter->GetValue().mb_str(wxConvUTF8) );
  wAction.settimer( m_Props, m_Timer->GetValue() );
  wAction.setdoublequote( m_Props, m_DoubleQuote->IsChecked() ? True:False);

  // "co,ext,sw,st,sys,sg,bk,lc,fn"
  int typenr = m_Type->GetSelection();
  switch( typenr ) {
    case 0: wAction.settype(m_Props, wOutput.name()); break;
    case 1: wAction.settype(m_Props, wExtCmd.name()); break;
    case 2: wAction.settype(m_Props, wSwitch.name()); break;
    case 3: wAction.settype(m_Props, wRoute.name()); break;
    case 4: wAction.settype(m_Props, wSysCmd.name()); break;
    case 5: wAction.settype(m_Props, wSignal.name()); break;
    case 6: wAction.settype(m_Props, wBlock.name()); break;
    case 7: wAction.settype(m_Props, wLoc.name()); break;
    case 8: wAction.settype(m_Props, wFunCmd.name()); break;
    case 9: wAction.settype(m_Props, wTurntable.name()); break;
    case 10: wAction.settype(m_Props, wSelTab.name()); break;
    case 11: wAction.settype(m_Props, wText.name()); break;
    case 12: wAction.settype(m_Props, wFeedback.name()); break;
    case 13: wAction.settype(m_Props, wStage.name()); break;
  }

  return true;

}


/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}

void TimedActions::initOutputList() {
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  m_ID->Clear();
  m_ID->Append( _T(" ") );

  if( model != NULL ) {
    iONode colist = wPlan.getcolist( model );
    iONode carlist = NULL;
    iONode operatorlist = NULL;

    // "co,ext,sw,st,sys,sg,bk,lc,fn"
    int typenr = m_Type->GetSelection();
    switch( typenr ) {
      case 0: colist = wPlan.getcolist( model ); break;
      case 1: m_ExecCmd->Enable(true)          ; return;
      case 2: colist = wPlan.getswlist( model ); break;
      case 3: colist = wPlan.getstlist( model ); break;
      case 4: return;
      case 5: colist = wPlan.getsglist( model ); break;
      case 6: colist = wPlan.getbklist( model ); break;
      case 7: colist = wPlan.getlclist( model ); break;
      case 8:
        colist = wPlan.getlclist( model );
        carlist = wPlan.getcarlist( model );
        operatorlist = wPlan.getoperatorlist( model );
        break;
      case 9: colist = wPlan.getttlist( model ); break;
      case 10: colist = wPlan.getseltablist( model ); break;
      case 11: colist = wPlan.gettxlist( model ); break;
      case 12: colist = wPlan.getfblist( model ); break;
      case 13: colist = wPlan.getsblist( model ); break;
    }

    m_ExecCmd->Enable(false);

    if( colist != NULL ) {
      int cnt = NodeOp.getChildCnt( colist );
      for( int i = 0; i < cnt; i++ ) {
        iONode co = NodeOp.getChild( colist, i );
        const char* id = wOutput.getid( co );
        if( id != NULL ) {
          ListOp.add(list, (obj)id);
        }
      }

      if( carlist != NULL ) {
        int cnt = NodeOp.getChildCnt( carlist );
        for( int i = 0; i < cnt; i++ ) {
          iONode car = NodeOp.getChild( carlist, i );
          const char* id = wCar.getid( car );
          if( id != NULL ) {
            ListOp.add(list, (obj)id);
          }
        }
      }

      if( operatorlist != NULL ) {
        int cnt = NodeOp.getChildCnt( operatorlist );
        for( int i = 0; i < cnt; i++ ) {
          iONode opr = NodeOp.getChild( operatorlist, i );
          const char* id = wOperator.getid( opr );
          if( id != NULL ) {
            ListOp.add(list, (obj)id);
          }
        }
      }

      ListOp.sort(list, &__sortStr);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        const char* id = (const char*)ListOp.get( list, i );
          m_ID->Append( wxString(id,wxConvUTF8) );
      }
    }
  }
  /* clean up the temp. list */
  ListOp.base.del(list);
}


/*!
 * TimedActions creator
 */

bool TimedActions::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TimedActions creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end TimedActions creation
    return true;
}


/*!
 * TimedActions destructor
 */

TimedActions::~TimedActions()
{
////@begin TimedActions destruction
////@end TimedActions destruction
}


/*!
 * Member initialisation
 */

void TimedActions::Init()
{
////@begin TimedActions member initialisation
    m_ActionBook = NULL;
    m_IndexPanel = NULL;
    m_TimedActions = NULL;
    m_ActionID = NULL;
    m_Add = NULL;
    m_Del = NULL;
    m_Doc = NULL;
    m_DefinitionPanel = NULL;
    m_labType = NULL;
    m_Type = NULL;
    m_labID = NULL;
    m_ID = NULL;
    m_labCommand = NULL;
    m_Command = NULL;
    m_ExecCmd = NULL;
    m_DoubleQuote = NULL;
    m_labParam = NULL;
    m_Parameter = NULL;
    m_labDuration = NULL;
    m_ActTime = NULL;
    m_labTimer = NULL;
    m_Timer = NULL;
    m_labDescription = NULL;
    m_Description = NULL;
    m_ActivationTimeBox = NULL;
    m_Timed = NULL;
    m_Every = NULL;
    m_labHour = NULL;
    m_Hour = NULL;
    m_labMin = NULL;
    m_Min = NULL;
    m_Random = NULL;
    m_UsePanel = NULL;
    m_UseList = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
////@end TimedActions member initialisation
}


/*!
 * Control creation for TimedActions
 */

void TimedActions::CreateControls()
{
////@begin TimedActions content construction
    TimedActions* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_ActionBook = new wxNotebook( itemDialog1, ID_ACTIONBOOK, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    m_IndexPanel = new wxPanel( m_ActionBook, ID_INDEXPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_IndexPanel->SetSizer(itemBoxSizer5);

    m_TimedActions = new wxListCtrl( m_IndexPanel, ID_LISTCTRL, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_HRULES );
    itemBoxSizer5->Add(m_TimedActions, 1, wxGROW|wxALL, 5);

    m_ActionID = new wxTextCtrl( m_IndexPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_ActionID, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer8, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    m_Add = new wxButton( m_IndexPanel, ID_BT_ADD, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_Add, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Del = new wxButton( m_IndexPanel, ID_BT_DEL, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_Del, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Doc = new wxButton( m_IndexPanel, ID_BUTTON_AC_DOC, _("Doc"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer8->Add(m_Doc, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ActionBook->AddPage(m_IndexPanel, _("Index"));

    m_DefinitionPanel = new wxPanel( m_ActionBook, ID_DEFINITIONPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxVERTICAL);
    m_DefinitionPanel->SetSizer(itemBoxSizer13);

    wxFlexGridSizer* itemFlexGridSizer14 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer14->AddGrowableCol(1);
    itemBoxSizer13->Add(itemFlexGridSizer14, 0, wxGROW|wxALL, 5);
    m_labType = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labType, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_TypeStrings;
    m_Type = new wxChoice( m_DefinitionPanel, ID_ACTIONS_TYPE, wxDefaultPosition, wxDefaultSize, m_TypeStrings, 0 );
    itemFlexGridSizer14->Add(m_Type, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labID = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_IDStrings;
    m_ID = new wxChoice( m_DefinitionPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_IDStrings, 0 );
    itemFlexGridSizer14->Add(m_ID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labCommand = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Command"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labCommand, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer20 = new wxFlexGridSizer(0, 3, 0, 0);
    itemFlexGridSizer20->AddGrowableCol(0);
    itemFlexGridSizer14->Add(itemFlexGridSizer20, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);
    wxArrayString m_CommandStrings;
    m_Command = new wxComboBox( m_DefinitionPanel, ID_COMBOBOX, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_CommandStrings, wxCB_DROPDOWN );
    itemFlexGridSizer20->Add(m_Command, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_ExecCmd = new wxButton( m_DefinitionPanel, ID_ACTIONS_EXEC_CMD, _("..."), wxDefaultPosition, wxSize(40, 25), 0 );
    itemFlexGridSizer20->Add(m_ExecCmd, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);

    m_DoubleQuote = new wxCheckBox( m_DefinitionPanel, wxID_ANY, _("Double quote"), wxDefaultPosition, wxDefaultSize, 0 );
    m_DoubleQuote->SetValue(false);
    itemFlexGridSizer20->Add(m_DoubleQuote, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);

    m_labParam = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Parameter"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labParam, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Parameter = new wxTextCtrl( m_DefinitionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 80), wxTE_MULTILINE );
    itemFlexGridSizer14->Add(m_Parameter, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labDuration = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Duration"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labDuration, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_ActTime = new wxSpinCtrl( m_DefinitionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer14->Add(m_ActTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labTimer = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Timer"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labTimer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Timer = new wxSpinCtrl( m_DefinitionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 300000, 0 );
    itemFlexGridSizer14->Add(m_Timer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labDescription = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_labDescription, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Description = new wxTextCtrl( m_DefinitionPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer14->Add(m_Description, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ActivationTimeBox = new wxStaticBox(m_DefinitionPanel, wxID_ANY, _("Activation time"));
    wxStaticBoxSizer* itemStaticBoxSizer32 = new wxStaticBoxSizer(m_ActivationTimeBox, wxHORIZONTAL);
    itemBoxSizer13->Add(itemStaticBoxSizer32, 0, wxGROW|wxALL, 5);
    m_Timed = new wxCheckBox( m_DefinitionPanel, wxID_ANY, _("Use"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Timed->SetValue(false);
    itemStaticBoxSizer32->Add(m_Timed, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_Every = new wxCheckBox( m_DefinitionPanel, wxID_ANY, _("every"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Every->SetValue(false);
    itemStaticBoxSizer32->Add(m_Every, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_labHour = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Hour"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer32->Add(m_labHour, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_Hour = new wxSpinCtrl( m_DefinitionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 23, 0 );
    itemStaticBoxSizer32->Add(m_Hour, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_labMin = new wxStaticText( m_DefinitionPanel, wxID_ANY, _("Minute"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer32->Add(m_labMin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_Min = new wxSpinCtrl( m_DefinitionPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(70, -1), wxSP_ARROW_KEYS, 0, 59, 0 );
    itemStaticBoxSizer32->Add(m_Min, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM, 5);

    m_Random = new wxCheckBox( m_DefinitionPanel, wxID_ANY, _("Random"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Random->SetValue(false);
    itemStaticBoxSizer32->Add(m_Random, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_ActionBook->AddPage(m_DefinitionPanel, _("Definition"));

    m_UsePanel = new wxPanel( m_ActionBook, ID_USEPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer41 = new wxBoxSizer(wxVERTICAL);
    m_UsePanel->SetSizer(itemBoxSizer41);

    wxArrayString m_UseListStrings;
    m_UseList = new wxListBox( m_UsePanel, ID_USELIST, wxDefaultPosition, wxDefaultSize, m_UseListStrings, wxLB_SINGLE );
    itemBoxSizer41->Add(m_UseList, 1, wxGROW|wxALL, 5);

    m_ActionBook->AddPage(m_UsePanel, _("Use"));

    itemBoxSizer2->Add(m_ActionBook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer43 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer43, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer43->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer43->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer43->AddButton(m_Apply);

    itemStdDialogButtonSizer43->Realize();

////@end TimedActions content construction
}


/*!
 * Should we show tooltips?
 */

bool TimedActions::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TimedActions::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin TimedActions bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end TimedActions bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TimedActions::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin TimedActions icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end TimedActions icon retrieval
}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LB_TimedActions
 */
/*
void TimedActions::OnLBTimedActionsSelected( wxCommandEvent& event )
{
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode aclist = wPlan.getaclist( model );
    if( aclist != NULL ) {
      int cnt = NodeOp.getChildCnt( aclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode ac = NodeOp.getChild( aclist, i );
        const char* id = wAction.getid( ac );
        if( id != NULL && StrOp.equals( id, m_TimedActions->GetStringSelection().mb_str(wxConvUTF8) ) ) {
          m_Props = ac;
          initValues();
          break;
        }
      }
    }
  }
}
*/

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void TimedActions::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void TimedActions::OnOkClick( wxCommandEvent& event )
{
  OnApplyClick(event);
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void TimedActions::OnApplyClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  if( !evaluate() )
    return;

  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }
  initIndex();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BT_ADD
 */

void TimedActions::OnBtAddClick( wxCommandEvent& event )
{
  int i = findID("NEW");
  if( i == wxNOT_FOUND ) {
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode aclist = wPlan.getaclist( model );
      if( aclist == NULL ) {
        aclist = NodeOp.inst( wActionList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, aclist );
      }
      if( aclist != NULL ) {
        iONode ac = NodeOp.inst( wAction.name(), aclist, ELEMENT_NODE );
        NodeOp.addChild( aclist, ac );
        wAction.setid( ac, "NEW" );
        m_Props = ac;
        appendItem(ac);
        initValues();
      }
    }
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BT_DEL
 */

void TimedActions::OnBtDelClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
    return;

  wxGetApp().pushUndoItem( (iONode)NodeOp.base.clone( m_Props ) );

  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
  wModelCmd.setcmd( cmd, wModelCmd.remove );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode aclist = wPlan.getaclist( model );
    if( aclist != NULL ) {
      NodeOp.removeChild( aclist, m_Props );
      m_Props = selectPrev();
    }
  }

  initIndex();
}


/*!
 * wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_ACTIONS_TYPE
 */

void TimedActions::OnActionsTypeSelected( wxCommandEvent& event )
{
  // TODO: init ID's
  initOutputList();
  initCommands();
}

void TimedActions::initCommands()
{
  m_Command->Clear();
  m_Command->SetValue(_T(""));
  // "co,ext,sw,st,sys,sg,bk,lc,fn"
  int typenr = m_Type->GetSelection();
  switch( typenr ) {
    case 0: // output
      m_Command->Append(wxString( wAction.output_on, wxConvUTF8));
      m_Command->Append(wxString( wAction.output_off, wxConvUTF8));
      m_Command->Append(wxString( wAction.output_active, wxConvUTF8));
      m_Command->Append(wxString( wOutput.flip, wxConvUTF8));
      break;
    case 1: // external command
      break;
    case 2: // switch
      m_Command->Append(wxString( wAction.switch_straight, wxConvUTF8));
      m_Command->Append(wxString( wAction.switch_turnout, wxConvUTF8));
      m_Command->Append(wxString( wAction.switch_left, wxConvUTF8));
      m_Command->Append(wxString( wAction.switch_right, wxConvUTF8));
      m_Command->Append(wxString( wAction.switch_flip, wxConvUTF8));
      break;
    case 3: // route
      m_Command->Append(wxString( wAction.route_set, wxConvUTF8));
      m_Command->Append(wxString( wAction.route_lock, wxConvUTF8));
      m_Command->Append(wxString( wAction.route_lockset, wxConvUTF8));
      m_Command->Append(wxString( wAction.route_unlock, wxConvUTF8));
      break;
    case 4: // system
      m_Command->Append(wxString( wAction.system_stoplocs, wxConvUTF8));
      m_Command->Append(wxString( wAction.system_go, wxConvUTF8));
      m_Command->Append(wxString( wAction.system_stop, wxConvUTF8));
      m_Command->Append(wxString( wAction.system_shutdown, wxConvUTF8));
      break;
    case 5: // signal
      m_Command->Append(wxString( wAction.signal_red, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_yellow, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_green, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_white, wxConvUTF8));
      m_Command->Append(wxString( wSignal.aspect, wxConvUTF8));
      break;
    case 6: // block
      m_Command->Append(wxString( wAction.block_unlock, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_red, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_yellow, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_green, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_white, wxConvUTF8));
      m_Command->Append(wxString( wAction.block_setloc, wxConvUTF8));
      m_Command->Append(wxString( wAction.block_schedule, wxConvUTF8));
      m_Command->Append(wxString( wTour.name(), wxConvUTF8));
      m_Command->Append(wxString( wAction.block_event, wxConvUTF8));
      m_Command->Append(wxString( wAction.block_acceptident, wxConvUTF8));
      m_Command->Append(wxString( wAction.loco_go, wxConvUTF8));
      m_Command->Append(wxString( wBlock.open, wxConvUTF8));
      m_Command->Append(wxString( wBlock.closed, wxConvUTF8));
      break;
    case 7: // loco
      m_Command->Append(wxString( wAction.loco_go, wxConvUTF8));
      m_Command->Append(wxString( wAction.loco_stop, wxConvUTF8));
      m_Command->Append(wxString( wAction.loco_velocity, wxConvUTF8));
      m_Command->Append(wxString( wAction.loco_dispatch, wxConvUTF8));
      m_Command->Append(wxString( wAction.loco_swap, wxConvUTF8));
      m_Command->Append(wxString( wAction.loco_carcount, wxConvUTF8));
      m_Command->Append(wxString( wAction.loco_percent, wxConvUTF8));
      m_Command->Append(wxString( wLoc.min, wxConvUTF8));
      m_Command->Append(wxString( wLoc.mid, wxConvUTF8));
      m_Command->Append(wxString( wLoc.cruise, wxConvUTF8));
      m_Command->Append(wxString( wLoc.max, wxConvUTF8));
      m_Command->Append(wxString( wLoc.consist, wxConvUTF8));
      break;
    case 8: // function
      m_Command->Append(wxString( wAction.fun_on, wxConvUTF8));
      m_Command->Append(wxString( wAction.fun_off, wxConvUTF8));
      m_Command->Append(wxString( wOutput.flip, wxConvUTF8));
      break;
    case 9: // turntable
      m_Command->Append(wxString( wAction.tt_goto, wxConvUTF8));
      m_Command->Append(wxString( wAction.tt_turn180, wxConvUTF8));
      m_Command->Append(wxString( wAction.fun_on, wxConvUTF8));
      m_Command->Append(wxString( wAction.fun_off, wxConvUTF8));
      m_Command->Append(wxString( wTurntable.lighton, wxConvUTF8));
      m_Command->Append(wxString( wTurntable.lightoff, wxConvUTF8));
      break;
    case 10: // fiddle yard
      m_Command->Append(wxString( wAction.tt_goto, wxConvUTF8));
      break;
    case 11: // text
      m_Command->Append(wxString( wAction.text_update, wxConvUTF8));
      break;
    case 12: // sensor
      m_Command->Append(wxString( wAction.output_on, wxConvUTF8));
      m_Command->Append(wxString( wAction.output_off, wxConvUTF8));
      m_Command->Append(wxString( wFeedback.reset, wxConvUTF8));
      break;
    case 13: // stageblock
      m_Command->Append(wxString( wAction.block_unlock, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_red, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_yellow, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_green, wxConvUTF8));
      m_Command->Append(wxString( wAction.signal_white, wxConvUTF8));
      m_Command->Append(wxString( wStage.open, wxConvUTF8));
      m_Command->Append(wxString( wStage.closed, wxConvUTF8));
      m_Command->Append(wxString( wStage.exitopen, wxConvUTF8));
      m_Command->Append(wxString( wStage.exitclosed, wxConvUTF8));
      m_Command->Append(wxString( wStage.compress, wxConvUTF8));
      break;
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONS_EXEC_CMD
 */

void TimedActions::OnActionsExecCmdClick( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("program"));
  if( fdlg->ShowModal() == wxID_OK ) {
    m_Command->SetValue( fdlg->GetPath() );
  }
  fdlg->Destroy();}


/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_USELIST
 */

void TimedActions::OnUselistSelected( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_USELIST in TimedActions.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_USELIST in TimedActions. 
}


/*!
 * wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event handler for ID_USELIST
 */

void TimedActions::OnUselistDoubleClicked( wxCommandEvent& event )
{
  iONode node = (iONode)m_UseList->GetClientData(m_UseList->GetSelection());
  if( node != NULL ) {
    ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, node );

    if( wxID_OK == dlg->ShowModal() ) {
      // TODO: inform
    }

    dlg->Destroy();
  }
}


/*!
 * wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRL
 */

void TimedActions::OnListctrlSelected( wxListEvent& event )
{
  m_Props = getSelection(event.GetIndex());
  initValues();
}


/*!
 * wxEVT_COMMAND_LIST_COL_CLICK event handler for ID_LISTCTRL
 */

void TimedActions::OnListctrlColLeftClick( wxListEvent& event )
{
  sortOnColumn(event.GetColumn());
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_AC_DOC
 */

void TimedActions::OnButtonAcDocClick( wxCommandEvent& event )
{
  doDoc( event, "actions");
}

