
#include "InputImpl.h"

InputImpl::InputImpl( wxWindow *parent)
    : wxBoxSizer(wxVERTICAL)
{
    this->channels = CONTROL_CHANNELS;
    this->switches = SWITCHES;

    sliderIDs = new wxWindowID[channels];
    chValues = new channelValue_t[channels];

    for( int i=0; i<channels; i++) {
        chValues[i] = 0;
    }

    switchIDs = new wxWindowID[switches];
    swValues = new switchState_t[switches];

    for( int i=0; i<switches; i++) {
        swValues[i] = SW_STATE_DONTCARE;
    }

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

    for( int channel=0; channel<channels; channel++) {
        sliderIDs[channel] = wxWindow::NewControlId();
        wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
        wxSlider *slider = new wxSlider(parent, sliderIDs[channel],0,-1000,1000, wxDefaultPosition, wxSize(-1,200), wxSL_VERTICAL);
        vbox->Add( slider);
        wxString str;
        str.Printf(wxT("CH%d"), channel+1);
        vbox->Add( new wxStaticText(parent, wxID_ANY, str));
        hbox->Add( vbox);
        hbox->AddSpacer(10);
        slider->Bind( wxEVT_SCROLL_THUMBTRACK, &InputImpl::OnScroll, this, sliderIDs[channel]);
    }

    Add( hbox);
    AddSpacer(10); 

    hbox = new wxBoxSizer(wxHORIZONTAL);
    wxString choices[] = {wxT("0"), wxT("1"), wxT("2")};
    for( int sw = 0; sw < switches; sw++) {
        switchConf_t conf = Controls::switchConfGet( SWITCH_CONFIGURATION, sw);

        wxRadioBox *swtch = nullptr;
        switchIDs[sw] = wxWindow::NewControlId();
        wxString str;

        switch( conf) {
            case SW_CONF_2STATE:
                str.Printf(wxT("SW%d"), sw+1);
                swtch = new wxRadioBox(parent, switchIDs[sw], str, wxDefaultPosition, wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_COLS);
                break;

            case SW_CONF_3STATE:
                str.Printf(wxT("SW%d"), sw+1);
                swtch = new wxRadioBox(parent, switchIDs[sw], str, wxDefaultPosition, wxDefaultSize, 3, choices, 1, wxRA_SPECIFY_COLS);
                break;

            default:
                continue;
        }

        hbox->Add(swtch);
        hbox->AddSpacer(10);
        swtch->Bind( wxEVT_RADIOBOX, &InputImpl::OnSwitch, this, switchIDs[sw]);
    }

    Add( hbox);
    AddSpacer(10);
}

InputImpl::~InputImpl( void) {

    if( sliderIDs != NULL) {
        delete [] sliderIDs;
        delete [] chValues;
        sliderIDs = NULL;
        chValues = NULL;
        delete [] switchIDs;
        delete [] swValues;
        switchIDs = NULL;
        swValues = NULL;
    }
}

void InputImpl::init( switchSetConf_t conf) {

    /* This is a no-op as all the initialization
     * has already be done in the constructor.
     */
}

channel_t InputImpl::GetChannels() {

    return channels;
}

switch_t InputImpl::GetSwitches() {

    return switches;
}

channelValue_t InputImpl::GetChannelValue( int ch) {

    return chValues[ch];
}

switchState_t InputImpl::GetSwitchValue( int sw) {

    return swValues[sw];
}

void InputImpl::OnScroll( wxScrollEvent& event) {

    for( int i=0; i<channels; i++) {
        if( sliderIDs[i] == event.GetId()) {
            chValues[i] = -event.GetPosition();
//            printf("HandleScroll %d %d\n", event.GetId(), values[i]);
            break;
        }
    }
    
}

void InputImpl::OnSwitch( wxCommandEvent& event) {

    for( int i=0; i<switches; i++) {
        if( switchIDs[i] == event.GetId()) {
            switch( event.GetSelection()) {
                case 0:
                    swValues[i] = SW_STATE_0;
                    break;

                case 1:
                    swValues[i] = SW_STATE_1;
                    break;

                case 2:
                    swValues[i] = SW_STATE_2;
                    break;

                default:
                    swValues[i] = SW_STATE_DONTCARE;
            }
//            printf("HandleSwitch %d %d\n", i, swValues[i]);
            break;
        }
    }
}
