#include <TouchDisplayModule.h>
#include <Arduino.h>
#include <ui.h>
#include "lv_xiao_round_screen.h"

const std::string TouchDisplayModule::name()
{
    return "Touch Display";
}

const std::string TouchDisplayModule::version()
{
    return "0.0dev";
}

void TouchDisplayModule::setup()
{
    TouchDisplayModule::doorState = DoorState::UNDEFINED;
    TouchDisplayModule::displayOn = true;
    TouchDisplayModule::lastPressed = 0;

    lv_log_register_print_cb(lv_log);

    lv_init();

    lv_xiao_disp_init();
    lv_xiao_touch_init();

    ui_init();

    lv_obj_add_event_cb(ui_ImgButtonClosed, ui_ImgButtonClosed_event, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(ui_ImgButtonOpen, ui_ImgButtonOpen_event, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(ui_ImgButtonAuto, ui_ImgButtonAuto_event, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(ui_Main, ui_Main_event, LV_EVENT_PRESSED, NULL);

    display_pressed();
    Serial.println("Started.");
}

void TouchDisplayModule::lv_log(const char * buf)
{
    Serial.println(buf);
}

void TouchDisplayModule::display_pressed() {
    TouchDisplayModule::lastPressed = millis();

    if (!TouchDisplayModule::displayOn) {
        Serial.println("Turn display on.");
        TouchDisplayModule::displayOn = true;
        digitalWrite(XIAO_BL, HIGH);
    }
}

void TouchDisplayModule::update_button_states(DoorState newDoorState)
{
    if (TouchDisplayModule::displayOn) {
        TouchDisplayModule::doorState = newDoorState;
        Serial.printf("New door state: %d\n", doorState);
    }

    display_pressed();

    switch (TouchDisplayModule::doorState)
    {
        case DoorState::CLOSED:
            lv_imgbtn_set_state(ui_ImgButtonClosed, LV_IMGBTN_STATE_CHECKED_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonOpen, LV_IMGBTN_STATE_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonAuto, LV_IMGBTN_STATE_RELEASED);
            break;

        case DoorState::OPEN:
            lv_imgbtn_set_state(ui_ImgButtonClosed, LV_IMGBTN_STATE_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonOpen, LV_IMGBTN_STATE_CHECKED_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonAuto, LV_IMGBTN_STATE_RELEASED);
            break;

        case DoorState::AUTO:
            lv_imgbtn_set_state(ui_ImgButtonClosed, LV_IMGBTN_STATE_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonOpen, LV_IMGBTN_STATE_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonAuto, LV_IMGBTN_STATE_CHECKED_RELEASED);
            break;

        default:
            lv_imgbtn_set_state(ui_ImgButtonClosed, LV_IMGBTN_STATE_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonOpen, LV_IMGBTN_STATE_RELEASED);
            lv_imgbtn_set_state(ui_ImgButtonAuto, LV_IMGBTN_STATE_RELEASED);
            break;
    }
}

void TouchDisplayModule::ui_ImgButtonClosed_event(lv_event_t * event)
{
    update_button_states(DoorState::CLOSED);
}

void TouchDisplayModule::ui_ImgButtonOpen_event(lv_event_t * event)
{
    update_button_states(DoorState::OPEN);
}

void TouchDisplayModule::ui_ImgButtonAuto_event(lv_event_t * event)
{
    update_button_states(DoorState::AUTO);
}

void TouchDisplayModule::ui_Main_event(lv_event_t * event)
{
    display_pressed();
    Serial.println("Main pressed.");
}

void TouchDisplayModule::loop()
{
    lv_timer_handler();  //let the GUI do its work 
    delay(5);

    if (TouchDisplayModule::displayOn && (millis() - TouchDisplayModule::lastPressed > DISPLAY_SLEEP_DELAY)) {
        Serial.println("Turn display off.");
        TouchDisplayModule::displayOn = false;
        digitalWrite(XIAO_BL, LOW);
    }
}

void TouchDisplayModule::processInputKo(GroupObject& iKo)
{
}

void TouchDisplayModule::processAfterStartupDelay()
{
}

// void TouchDisplayModule::writeFlash()
// {
//     for (size_t i = 0; i < flashSize(); i++)
//     {
//         //openknx.flash.writeByte(0xd0 + i);
//     }
// }

// void TouchDisplayModule::readFlash(const uint8_t* data, const uint16_t size)
// {
//     // printHEX("RESTORE:", data,  len);
// }

// uint16_t TouchDisplayModule::flashSize()
// {
//     return 10;
// }