#ifndef __WANGBIN_SETPIC33_TEST_TEST__SETPIC_INCLUDE_START_H__
#define __WANGBIN_SETPIC33_TEST_TEST__SETPIC_INCLUDE_START_H__

#include <cstdlib>
#include <cstdio>
#include <gtk/gtk.h>

struct tcp_st
{
    int32_t     id;
    int32_t     mode;
    int32_t     ret;
    char    msg[256];
};


struct DrawConfig
{
    int enable;
    int current;
    float offset_x;
    float offset_y;
    float scale_x;
    float scale_y;
    cairo_surface_t* image_jessie;
    cairo_surface_t* image_wheezy;
};

struct DrawingCenter
{
    float x;
    float y;
};

struct DrawingRect
{
    float x;
    float y;
    float x2;
    float y2;
};

struct SharedConfig
{
    char    szPath[260];
    int     section;
};

/*
struct EmptyRC
{
    char szText1[16];
    char szText2[16];
};
*/

struct SetPicImage
{
    int     width;
    int     height;
    int     depth;
    int     channal;
    void*   data;
    int     size;
};

struct UIFrame
{
    GtkWidget*          TopWindow;
    GtkBuilder*         Builder;

    GtkWidget*          MainPageContainer;          // Notebook
    GtkWidget*          MainPage;                   // MainPage ,VBOX

    GtkWidget*          EventBox;               //for DrawingArea
    GtkWidget*          DrawingArea;           // DrawingArea
    GtkWidget*          SettingPageContainer;       // Notebook


    GtkWidget*          _home_home_tbxSCommand;
    GtkWidget*          _home_home_tbxSMode;
    GtkWidget*          _home_home_tbxSParam;
    GtkWidget*          _home_home_tbxSMessage;

    GtkWidget*          _home_home_tbxRCommand;
    GtkWidget*          _home_home_tbxRMode;
    GtkWidget*          _home_home_tbxRParam;
    GtkWidget*          _home_home_tbxRMessage;

    GtkWidget*          _home_home_tbxInfo;

    GtkWidget*          _home_home_btnConfig;
    GtkWidget*          _home_home_btnClear;
    GtkWidget*          _home_home_btnSend;

    GtkWidget*          ConfigPage;                     //Config Page ,VBOX
    GtkWidget*          ConfigEventBox;                 //
    GtkWidget*          ConfigMenu;                     //DrawingArea
    GtkWidget*          ConfigLayout;                    //NoteBook
    GtkWidget*          _config_page_btnBack;
    //GtkWidget*          _config_page_btnApply;



    gint                shared_memory_timer;
    bool                shared_memory_status;

    DrawConfig          cairo_config;
    SharedConfig        shared_config;

    unsigned long long int              ulldTimeStamp;
    //volatile int                        save_buffer_status;
    volatile int                        next_ui_buffer_index;
    volatile unsigned long long int     save_buffer_index;

};

/* Private: */

int __create_tcp_client(const char* server_ip_address,uint port);

void __destroy_tcp_client(int __socket);

void __home_home_clear_receive_message(gpointer data);

void __destroy(GtkWidget* widget,gpointer* data);

void __init_cairo_config(UIFrame* hWnd);

void __init_uiframe(void);

void __home_home_tbxSCommand_changed(GtkWidget* _widget,gpointer data);

void __home_home_tbxSMode_changed(GtkWidget* _widget,gpointer data);

void __home_home_tbxSParam_changed(GtkWidget* _widget,gpointer data);

void __home_home_tbxSMessage_changed(GtkWidget* _widget,gpointer data);

void __home_home_tbxRCommand_changed(GtkWidget* _widget,gpointer data);

void __home_home_tbxRMode_changed(GtkWidget* _widget,gpointer data);

void __home_home_tbxRParam_changed(GtkWidget* _widget,gpointer data);

void __home_home_tbxRMessage_changed(GtkWidget* _widget,gpointer data);

gboolean __home_home_tbxSCommand_focus_in_event(GtkWidget* _widget,GdkEvent* _event,gpointer data);

gboolean __home_home_tbxSCommand_focus_out_event(GtkWidget* _widget,GdkEvent* _event,gpointer data);

gboolean __home_home_tbxSMode_focus_in_event(GtkWidget* _widget,GdkEvent* _event,gpointer data);

gboolean __home_home_tbxSMode_focus_out_event(GtkWidget* _widget,GdkEvent* _event,gpointer data);

void __home_home_DrawingArea_pop_up(GtkWidget* _widget,GdkEvent* _event,gpointer data);

void __home_home_btnConfig_clicked(GtkWidget* _widget,gpointer data);

void __home_home_btnClear_clicked(GtkWidget* _widget,gpointer data);

void __home_home_clear_receive_message(gpointer data);

void __home_home_btnSend_clicked(GtkWidget* _widget,gpointer data);

gboolean __home_home_DrawingArea_enter_notify_event(GtkWidget* _widget,GdkEventCrossing* event,gpointer data);

gboolean __home_home_DrawingArea_leave_notify_event(GtkWidget* _widget,GdkEventCrossing* event,gpointer data);

gboolean __home_home_DrawingArea_motion_notify_event(GtkWidget* _widget,GdkEventMotion* event,gpointer data);

void __create_image_from_file(gpointer data,const gchar* filename);

void __destroy_image_from_file(gpointer data);

void __empty_image_render(gpointer data);

void __drawing_area_render_impl(gpointer data);

gboolean __home_home_DrawingArea_expose_event(GtkWidget* _widget,GdkEventExpose* event,gpointer data);

void __config_drawing_config_menu(gpointer data);

bool __init_shared(const char* lpszPath="./runtime",int section=2);

void __init_uiframe_signal(UIFrame* hWnd);

void init(UIFrame* hWnd);





#endif /* __WANGBIN_SETPIC33_TEST_TEST__SETPIC_INCLUDE_START_H__ */