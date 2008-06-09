// Berkeley Open Infrastructure for Network Computing
// http://boinc.berkeley.edu
// Copyright (C) 2005 University of California
//
// This is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation;
// either version 2.1 of the License, or (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// To view the GNU Lesser General Public License visit
// http://www.gnu.org/copyleft/lesser.html
// or write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "DlgItemProperties.h"
#endif

#include "stdwx.h"
#include "DlgItemProperties.h"
#include "BOINCGUIApp.h"
#include "Events.h"
#include "error_numbers.h"

IMPLEMENT_DYNAMIC_CLASS(CDlgItemProperties, wxDialog)

/* Constructor */
CDlgItemProperties::CDlgItemProperties(wxWindow* parent) : CDlgItemPropertiesBase(parent,ID_ANYDIALOG) {
	this->m_current_row=0;
}

// destructor
CDlgItemProperties::~CDlgItemProperties() {
}

// renders infos for a project
void CDlgItemProperties::renderInfos(PROJECT* project) {
	std::string name;
	//collecting infos
	project->get_name(name);
	wxString wxTitle = _("Properties for ");
	wxTitle.append(name.c_str());
	this->SetTitle(wxTitle);
	
	this->addSection(_("general infos"));
	this->addProperty(_("master url:"),wxT(project->master_url.c_str()));
	this->addProperty(_("user name:"),wxT(project->user_name.c_str()));
	this->addProperty(_("team name:"),wxT(project->team_name.c_str()));
	this->addProperty(_("resource share:"),wxString::Format(wxT("%0.0f"),project->resource_share));
	this->addSection(_("credit infos"));
	this->addProperty(_("user total credit:"),wxString::Format(wxT("%0.2f"),project->user_total_credit));
	this->addProperty(_("user avg. credit:"),wxString::Format(wxT("%0.2f"),project->user_expavg_credit));
	this->addProperty(_("host total credit:"),wxString::Format(wxT("%0.2f"),project->host_total_credit));
	this->addProperty(_("host avg. credit:"),wxString::Format(wxT("%0.2f"),project->host_expavg_credit));
	this->addSection(_("disk usage infos"));
	this->addProperty(_("disk usage:"),this->FormatDiskSpace(project->disk_usage));
	this->addSection(_("scheduling infos"));
	this->addProperty(_("short term debt:"),wxString::Format(wxT("%0.2f"),project->short_term_debt));
	this->addProperty(_("long term debt:"),wxString::Format(wxT("%0.2f"),project->long_term_debt));
	this->addProperty(_("duration corr. factor:"),wxString::Format(wxT("%0.4f"),project->duration_correction_factor));
	this->addSection(_("diverse infos"));
	this->addProperty(_("non cpu intensive:"),project->non_cpu_intensive ? _("yes") : _("no"));
	this->addProperty(_("suspended via gui:"),project->suspended_via_gui ? _("yes") : _("no"));
	this->addProperty(_("don't request more work:"),project->dont_request_more_work ? _("yes") : _("no"));
	this->addProperty(_("scheduler call in progress:"),project->scheduler_rpc_in_progress ? _("yes") : _("no"));
	this->addProperty(_("attached via account mgr.:"),project->attached_via_acct_mgr ? _("yes") : _("no"));
	this->addProperty(_("detach when done:"),project->detach_when_done ? _("yes") : _("no"));
	this->addProperty(_("ended:"),project->ended ? _("yes") : _("no"));
	this->m_gbSizer->Layout();
	this->m_scrolledWindow->FitInside();
}

// renders infos for a task/result
void CDlgItemProperties::renderInfos(RESULT* result) {
	wxString wxTitle = _("Properties for ");
	wxTitle.append(result->name.c_str());
	this->SetTitle(wxTitle);

	this->addSection(_("general infos"));
	this->addProperty(_("application name:"),this->FormatApplicationName(result));
	this->addProperty(_("workunit name:"),result->wu_name.c_str());
	this->addSection(_("state infos"));
	this->addProperty(_("state:"),this->FormatStatus(result));
	if(result->active_task) {
		this->addSection(_("calculation infos"));
		this->addProperty(_("checkpoint cpu time:"),FormatTime(result->checkpoint_cpu_time));
		this->addProperty(_("current cpu time:"),FormatTime(result->current_cpu_time));
		this->addProperty(_("est. copu time remaining:"),FormatTime(result->estimated_cpu_time_remaining));
		this->addProperty(_("fraction done:"),wxString::Format(wxT("%.3f %%"),floor(result->fraction_done * 100000)/1000));
	}
	this->m_gbSizer->Layout();
	this->m_scrolledWindow->FitInside();
}

//
wxString CDlgItemProperties::FormatDiskSpace(double bytes) {
    double         xTera = 1099511627776.0;
    double         xGiga = 1073741824.0;
    double         xMega = 1048576.0;
    double         xKilo = 1024.0;
	wxString strBuffer= wxEmptyString;

    if (bytes >= xTera) {
        strBuffer.Printf(wxT("%0.2f TB"), bytes/xTera);
    } else if (bytes >= xGiga) {
        strBuffer.Printf(wxT("%0.2f GB"), bytes/xGiga);
    } else if (bytes >= xMega) {
        strBuffer.Printf(wxT("%0.2f MB"), bytes/xMega);
    } else if (bytes >= xKilo) {
        strBuffer.Printf(wxT("%0.2f KB"), bytes/xKilo);
    } else {
        strBuffer.Printf(wxT("%0.0f bytes"), bytes);
    }
    return strBuffer;
}

//
wxString CDlgItemProperties::FormatApplicationName(RESULT* result ) {
	wxString strBuffer = wxEmptyString;
    CMainDocument* pDoc = wxGetApp().GetDocument();
    RESULT* state_result = NULL;
    wxString strLocalBuffer;

    wxASSERT(pDoc);
    wxASSERT(wxDynamicCast(pDoc, CMainDocument));

    if (result) {
        state_result = pDoc->state.lookup_result(result->project_url, result->name);
        if (!state_result) {
            pDoc->ForceCacheUpdate();
            state_result = pDoc->state.lookup_result(result->project_url, result->name);
        }
        wxASSERT(state_result);

        wxString strLocale = wxString(setlocale(LC_NUMERIC, NULL), wxConvUTF8);
        setlocale(LC_NUMERIC, "C");
        if (state_result->wup->app->user_friendly_name.size()) {
            strLocalBuffer = wxString(state_result->app->user_friendly_name.c_str(), wxConvUTF8);
        } else {
            strLocalBuffer = wxString(state_result->wup->avp->app_name.c_str(), wxConvUTF8);
        }
        char buf[256];
        if (state_result->wup->avp->plan_class.size()) {
            sprintf(buf, " (%s)", state_result->wup->avp->plan_class.c_str());
        } else {
            strcpy(buf, "");
        }
        strBuffer.Printf(
            wxT(" %s %.2f%s"), 
            strLocalBuffer.c_str(),
            state_result->wup->avp->version_num/100.0,
            buf
        );
        setlocale(LC_NUMERIC, (const char*)strLocale.mb_str());
    }
    return strBuffer;
}


//
wxString CDlgItemProperties::FormatStatus(RESULT* result) {
	wxString strBuffer= wxEmptyString;
    CMainDocument* doc = wxGetApp().GetDocument();    
    CC_STATUS      status;

    wxASSERT(doc);
    wxASSERT(wxDynamicCast(doc, CMainDocument));

    doc->GetCoreClientStatus(status);
    
	int throttled = status.task_suspend_reason & SUSPEND_REASON_CPU_USAGE_LIMIT;
    switch(result->state) {
    case RESULT_NEW:
        strBuffer = _("New"); 
        break;
    case RESULT_FILES_DOWNLOADING:
        if (result->ready_to_report) {
            strBuffer = _("Download failed");
        } else {
            strBuffer = _("Downloading");
        }
        break;
    case RESULT_FILES_DOWNLOADED:
        if (result->project_suspended_via_gui) {
            strBuffer = _("Project suspended by user");
        } else if (result->suspended_via_gui) {
            strBuffer = _("Task suspended by user");
        } else if (status.task_suspend_reason && !throttled) {
            strBuffer = _("Suspended");
            if (status.task_suspend_reason & SUSPEND_REASON_BATTERIES) {
                strBuffer += _(" - on batteries");
            }
            if (status.task_suspend_reason & SUSPEND_REASON_USER_ACTIVE) {
                strBuffer += _(" - user active");
            }
            if (status.task_suspend_reason & SUSPEND_REASON_USER_REQ) {
                strBuffer += _(" - computation suspended");
            }
            if (status.task_suspend_reason & SUSPEND_REASON_TIME_OF_DAY) {
                strBuffer += _(" - time of day");
            }
            if (status.task_suspend_reason & SUSPEND_REASON_BENCHMARKS) {
                strBuffer += _(" - CPU benchmarks");
            }
            if (status.task_suspend_reason & SUSPEND_REASON_DISK_SIZE) {
                strBuffer += _(" - need disk space");
            }
        } else if (result->active_task) {
            if (result->too_large) {
                strBuffer = _("Waiting for memory");
            } else if (result->needs_shmem) {
                strBuffer = _("Waiting for shared memory");
            } else if (result->scheduler_state == CPU_SCHED_SCHEDULED) {
                if (result->edf_scheduled) {
                    strBuffer = _("Running, high priority");
                } else {
                    strBuffer = _("Running");
                }
            } else if (result->scheduler_state == CPU_SCHED_PREEMPTED) {
                strBuffer = _("Waiting to run");
            } else if (result->scheduler_state == CPU_SCHED_UNINITIALIZED) {
                strBuffer = _("Ready to start");
            }
        } else {
            strBuffer = _("Ready to start");
        }
        break;
    case RESULT_COMPUTE_ERROR:
        strBuffer = _("Computation error");
        break;
    case RESULT_FILES_UPLOADING:
        if (result->ready_to_report) {
            strBuffer = _("Upload failed");
        } else {
            strBuffer = _("Uploading");
        }
        break;
    case RESULT_ABORTED:
        switch(result->exit_status) {
        case ERR_ABORTED_VIA_GUI:
            strBuffer = _("Aborted by user");
            break;
        case ERR_ABORTED_BY_PROJECT:
            strBuffer = _("Aborted by project");
            break;
        default:
            strBuffer = _("Aborted");
        }
        break;
    default:
        if (result->got_server_ack) {
            strBuffer = _("Acknowledged");
        } else if (result->ready_to_report) {
            strBuffer = _("Ready to report");
        } else {
            strBuffer.Format(_("Error: invalid state '%d'"), result->state);
        }
        break;
    }
    return strBuffer;
}

// 
wxString CDlgItemProperties::FormatTime(float fBuffer) {
    wxInt32        iHour = 0;
    wxInt32        iMin = 0;
    wxInt32        iSec = 0;
    wxTimeSpan     ts;
	wxString strBuffer= wxEmptyString;

    if (0 >= fBuffer) {
        strBuffer = wxT("---");
    } else {
        iHour = (wxInt32)(fBuffer / (60 * 60));
        iMin  = (wxInt32)(fBuffer / 60) % 60;
        iSec  = (wxInt32)(fBuffer) % 60;

        ts = wxTimeSpan(iHour, iMin, iSec);

        strBuffer = ts.Format();
    }

    return strBuffer;
}

// adds a title section label to the dialog 
void CDlgItemProperties::addSection(const wxString& title) {
	wxStaticText* staticText = new wxStaticText( m_scrolledWindow, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, 0);
	staticText->Wrap( -1 );
	staticText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );	
	m_gbSizer->Add( staticText, wxGBPosition( m_current_row, 0 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 3);
	m_current_row++;
}

// adds a property row to the dialog 
void CDlgItemProperties::addProperty(const wxString& name, const wxString& value) {
	
	wxStaticText* staticText = new wxStaticText( m_scrolledWindow, wxID_ANY, name, wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	m_gbSizer->Add( staticText, wxGBPosition( m_current_row, 0 ), wxGBSpan( 1, 1 ), wxALL, 3 );
	
	staticText = new wxStaticText( m_scrolledWindow, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, 0 );
	staticText->Wrap( -1 );
	m_gbSizer->Add( staticText, wxGBPosition( m_current_row, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 3 );
	m_current_row++;
}
