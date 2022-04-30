
#include "InputBox.h"

InputBox::InputBox( wxWindow *parent, int channels, int switches)
    : wxBoxSizer(wxVERTICAL)
{
    this->channels = channels;
    this->switches = switches;

    sliderIDs = new wxWindowID[channels];
    chValues = new int[channels];

    for( int i=0; i<channels; i++) {
        chValues[i] = 0;
    }

    switchIDs = new wxWindowID[switches];
    swValues = new int[switches];

    for( int i=0; i<switches; i++) {
        swValues[i] = 0;
    }

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

    for( int channel=0; channel<channels; channel++) {
        sliderIDs[channel] = wxWindow::NewControlId();
        wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
        wxSlider *slider = new wxSlider(parent, sliderIDs[channel],0,-100,100, wxDefaultPosition, wxSize(-1,200), wxSL_VERTICAL);
        vbox->Add( slider);
        wxString str;
        str.Printf(wxT("CH%d"), channel+1);
        vbox->Add( new wxStaticText(parent, wxID_ANY, str));
        hbox->Add( vbox);
        hbox->AddSpacer(10);
        slider->Bind( wxEVT_SCROLL_THUMBTRACK, &InputBox::OnScroll, this, sliderIDs[channel]);
    }

    Add( hbox);
    AddSpacer(10); 

    hbox = new wxBoxSizer(wxHORIZONTAL);
    wxString choices[] = {wxT("0"), wxT("1"), wxT("2")};
    for( int sw=0; sw<switches;sw++) {
        switchIDs[sw] = wxWindow::NewControlId();
        wxString str;
        str.Printf(wxT("SW%d"), sw+1);
        wxRadioBox *swtch = new wxRadioBox(parent, switchIDs[sw], str, wxDefaultPosition, wxDefaultSize, 3, choices, 1, wxRA_SPECIFY_COLS);
        hbox->Add(swtch);
        hbox->AddSpacer(10);
        swtch->Bind( wxEVT_RADIOBOX, &InputBox::OnSwitch, this, switchIDs[sw]);
    }

    Add( hbox);
    AddSpacer(10);
}

InputBox::~InputBox( void) {

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

int InputBox::GetChannels() {

    return channels;
}

int InputBox::GetSwitches() {

    return switches;
}

int InputBox::GetChannelValue( int ch) {

    return chValues[ch];
}

int InputBox::GetSwitchValue( int sw) {

    return swValues[sw];
}

void InputBox::OnScroll( wxScrollEvent& event) {

    for( int i=0; i<channels; i++) {
        if( sliderIDs[i] == event.GetId()) {
            chValues[i] = -event.GetPosition();
//            printf("HandleScroll %d %d\n", event.GetId(), values[i]);
            break;
        }
    }
    
}

void InputBox::OnSwitch( wxCommandEvent& event) {

    for( int i=0; i<switches; i++) {
        if( switchIDs[i] == event.GetId()) {
            swValues[i] = event.GetSelection();
//            printf("HandleSwitch %d %d\n", event.GetId(), event.GetSelection());
            break;
        }
    }
}
