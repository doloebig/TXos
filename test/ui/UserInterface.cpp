
#include "UserInterface.h"
#include "ModuleManager.h"
#include "Model.h"
#include "SystemConfig.h"
#include "ModelSelect.h"
#include "Buzzer.h"


extern DisplayImpl *displayImpl;
extern ModuleManager moduleManager;
extern SystemConfig systemConfig;
extern ModelSelect modelSelect;
extern Buzzer buzzer;

const buzzerCmd_t SoundClear[] = {
  BUZZER_PLAY( 1),
  BUZZER_PAUSE( 1),
  BUZZER_REPEAT( 0, 2),
  BUZZER_STOP()
};

const buzzerCmd_t SoundAlarm[] = {
  BUZZER_PAUSE( 20),
  BUZZER_PLAY( 1),
  BUZZER_PAUSE( 1),
  BUZZER_REPEAT( 1, 5),
  BUZZER_PAUSE( 20),
  BUZZER_STOP()
};

const char *messageText[] = {
    TEXT_MSG_NONE,
    TEXT_MSG_BAD_SYSCONFIG,
    TEXT_MSG_LOW_BATT,
    TEXT_MSG_MODEL_LOAD_FAILED,
    TEXT_MSG_CONFIG_SIZE
};

void UserInterface::init() {

    vccMonitor = (VccMonitor*)moduleManager.getSystemMenu()->getModuleByType( MODULE_VCC_MONITOR_TYPE);
    phases = (Phases*)moduleManager.getModelMenu()->getModuleByType( MODULE_PHASES_TYPE);
    timer = (Timer*)moduleManager.getModelMenu()->getModuleByType( MODULE_TIMER_TYPE);
    engineCut = (EngineCut*)moduleManager.getModelMenu()->getModuleByType( MODULE_ENGINE_CUT_TYPE);

    module = NULL;
    
    screen[0] = SCREEN_INIT;
    screenPtr = 0;

    message1 = message2 = 0;
    post1 = post2 = 0;

    lastPhase = 255;
    lastVcc = 0;
    lastTime = 0;
    engineSave = false;

    lcd = displayImpl->getLCD();
    
    lcd->clear();
    lcd->setFontSize( 2);
 
    screenWidth = lcd->getColumns();
 
    lcd->println(TEXT_BOOTING);
}

void UserInterface::handle() {

    Event *event = displayImpl->getEvent();

    if( event->key != KEY_NONE) {
        LOGV("\nUserInterface::handle(): Event %d (count=%d)\n", event->key, event->count);
    }
    
    switch (screen[screenPtr]) {
    case SCREEN_INIT:
        switchScreen( SCREEN_HOME);
        break;
    
    case SCREEN_HOME:
        homeScreen( event);
        break;

    case SCREEN_MODEL:
        menuScreen( event, moduleManager.getModelMenu());
        break;

    case SCREEN_SYSTEM:
        menuScreen( event, moduleManager.getSystemMenu());
        break;

    case SCREEN_CONFIG:
        configScreen( event);
        break;

    default:
        break;
    }
}

void UserInterface::homeScreen( Event *event) {

    if( refresh == REFRESH_FULL) {
        lcd->normalColors();
        lcd->clear();

        lcd->printStr( TEXT_TXOS, 5);
        lcd->print( TXOS_VERSION);

        lcd->setCursor(1, 0);
        lcd->printUInt( modelSelect.getModelID(), 2);
        lcd->print(": ");
        Model *model = (Model*)moduleManager.getModelMenu()->getModuleByType(MODULE_MODEL_TYPE);
        lcd->printStr( model->getModelName(), MODEL_NAME_LEN);
    }

    if( engineCut && (refresh == REFRESH_FULL || engineCut->isSave() != engineSave)) {
        engineSave = engineCut->isSave();

        lcd->okColors();
        lcd->setCursor(2, 10);
        lcd->printStr(engineSave ? TEXT_THR : TEXT_MSG_NONE, 3);
    }

    if( phases &&  (refresh == REFRESH_FULL || phases->getPhase() != lastPhase)) {
        lastPhase = phases->getPhase();

        lcd->normalColors();
        lcd->setCursor(3, 3);
        lcd->printStr(phases->getPhaseName(), TEXT_PHASE_NAME_length);
    }

    if( timer && (refresh == REFRESH_FULL || timer->timeSec() != lastTime)) {

        lastTime = timer->timeSec();

        lcd->normalColors();
        lcd->setCursor(5, 0);
        lcd->printStr( timer->timeStr(), 5);
    }

    if( vccMonitor && (refresh == REFRESH_FULL || vccMonitor->getVcc() != lastVcc)) {

        lastVcc = vccMonitor->getVcc();

        if( vccMonitor->belowAlert()) {
            lcd->alertColors();
            postMessage(0, MSG_LOW_BATT);
            buzzer.playPermanent( SoundAlarm);
        } else if( vccMonitor->belowWarn()) {
            lcd->warnColors();
        } else {
            lcd->okColors();
        }
        lcd->setCursor(5, 7);
        lcd->printFloat16( lastVcc, 5);
        lcd->print( "V");
    }

    if( refresh == REFRESH_FULL || post1 != message1) {
        message1 = post1;
        lcd->warnColors();
        lcd->setCursor(6, 0);
        lcd->printStr(messageText[message1], screenWidth);
    }

    if( refresh == REFRESH_FULL || post2 != message2) {
        message2 = post2;
        lcd->warnColors();
        lcd->setCursor(7, 0);
        lcd->printStr(messageText[message2], screenWidth);
    }

    refresh = REFRESH_OK;

    switch( event->key) {
    case KEY_ENTER:
        pushScreen( SCREEN_MODEL);
        break;
    
    case KEY_CLEAR:
        if( post1 || post2) {
            /* Clear messages and switch off alarm */
            post1 = post2 = 0;
            buzzer.off();
        } else {
            timer->reset();
            buzzer.play( SoundClear);
        }
    }
}

void UserInterface::menuScreen( Event *event, Menu *menu) {

    uint8_t idx;

    if( refresh == REFRESH_FULL) {
        selectList.set( menu, true);
        refresh = REFRESH_OK;
    }

    selectList.process( lcd, event);

    if( event->pending()) {
        LOGV("UserInterface::menuScreen(): event pending %d\n", event->key);
        switch( event->key) {
        case KEY_ENTER:
            idx = selectList.current();
            if( idx == GO_BACK) {
                popScreen();
            } else {
                module = menu->getModule(idx);
                LOGV("UserInterface::menuScreen(): Module %s\n", module ? module->getName() : "NULL");
                pushScreen( SCREEN_CONFIG);
            }
            break;
        }
    }
}

void UserInterface::configScreen( Event *event) {

    uint8_t idx;

    if( module == NULL) {
        LOG("** UserInterface::configScreen(): No module\n");
        popScreen();
        return;
    }

    if( refresh == REFRESH_FULL) {
        selectList.set( module, true);
        refresh = REFRESH_OK;
    }

    selectList.process( lcd, event);

    if( event->pending()) {
        switch( event->key) {
        case KEY_ENTER:
            idx = selectList.current();
            if( idx == GO_BACK) {
                moduleManager.saveModel( modelSelect.getModelID());
                systemConfig.save();
                popScreen();
            }
            break;
        }
    }
}

void UserInterface::cancelEdit( TableEditable *toCancel) {

    selectList.cancelEdit( toCancel);
}

void UserInterface::postMessage( uint8_t line, uint8_t msg) {

    if( msg < TEXT_MSG_count) {
        if( line == 0) {
            post1 = msg;
        } else{
            post2 = msg;
        }
    }
}

void UserInterface::toScreen( uint8_t scr) {

    screenPtr = 0;
    screen[screenPtr] = scr;
    refresh = REFRESH_FULL;
    LOGV("UserInterface::switchScreen(): Switch to %d\n", screen[screenPtr]);
}

void UserInterface::switchScreen( uint8_t scr) {

    screen[screenPtr] = scr;
    refresh = REFRESH_FULL;
    LOGV("UserInterface::switchScreen(): Switch to %d\n", screen[screenPtr]);
}

void UserInterface::pushScreen( uint8_t scr) {

    if( screenPtr < SCREEN_STACK_SIZE-1) screenPtr++;
    screen[screenPtr] = scr;
    refresh = REFRESH_FULL;
    LOGV("UserInterface::pushScreen(): Switch to %d\n", screen[screenPtr]);
}

void UserInterface::popScreen() {

    if( screenPtr > 0) screenPtr--;
    refresh = REFRESH_FULL;
    LOGV("UserInterface::popScreen(): Switch to %d\n", screen[screenPtr]);
}