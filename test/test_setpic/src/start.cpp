#include "start.h"
#include "getpic.h"
//#include "server.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <list>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include <gtk/gtk.h>
#include <cairo.h>

char* _UTF8(const char* c)
{
    return (g_locale_to_utf8(c,-1,0,0,0));
}

static const int __top_window_width(900);//900);
static const int __top_window_height(550);//550);
static const int __top_window_min_config_area_width(280);
static const int __top_window_max_config_area_width(320);
static UIFrame          _g_hwnd;
static cv::Mat img0;
static cv::Mat img1;


int __create_tcp_client(const char* server_ip_address,uint port)
{
    struct sockaddr_in client_addr;
    std::memset(&client_addr,0x00,sizeof(client_addr));
    client_addr.sin_family=AF_INET;
    client_addr.sin_addr.s_addr=htons(INADDR_ANY);
    client_addr.sin_port=htons(0);
    int client_socket=socket(AF_INET,SOCK_STREAM,0);
    if(client_socket<0)
    {
        std::fprintf(stderr, "%s\n","Create Socket Failed!\n");
        return -1;
    }
    if(bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    {
        std::fprintf(stderr, "%s\n","Client Bind Port Failed!\n");
        return -1;
    }
    struct sockaddr_in server_addr;
    std::memset(&server_addr,0x00,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    if(0==inet_aton(server_ip_address,&server_addr.sin_addr))
    {
        std::fprintf(stderr, "%s\n","Server IP Address Error!\n");
        return -1;
    }
    server_addr.sin_port=htons(port);
    socklen_t server_addr_length=sizeof(server_addr);
    if(connect(client_socket,(struct sockaddr*)&server_addr,server_addr_length)<0)
    {
        std::fprintf(stderr, "Can Not Connect to \" %s \" \n",server_ip_address);
        return -1;
    }
    return client_socket;
}

void __destroy_tcp_client(int __socket)
{
    close(__socket);
}

/*
* SIGNAL & CALLBACK
*/

void __destroy(GtkWidget* widget,gpointer* data)
{
    gtk_main_quit();
}

/*
* UI Frame
*/

void __init_cairo_config(UIFrame* hWnd)
{
    if(NULL==hWnd) return;
    hWnd->cairo_config.enable   =false;
    hWnd->cairo_config.current  =-1;
    hWnd->cairo_config.offset_x =0.0f;
    hWnd->cairo_config.offset_y =0.0f;
    hWnd->cairo_config.scale_x  =1.0f;
    hWnd->cairo_config.scale_y  =1.0f;
    hWnd->cairo_config.image_jessie=0;
    hWnd->cairo_config.image_wheezy=0;
};

void __init_uiframe(UIFrame* hWnd,const char* window_title)
{
    // Init Top Window
    hWnd->TopWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_keep_above(GTK_WINDOW(hWnd->TopWindow),(gboolean)true);
    gtk_window_set_title(GTK_WINDOW(hWnd->TopWindow),(const gchar*)window_title);
    gtk_window_set_position(GTK_WINDOW(hWnd->TopWindow),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(hWnd->TopWindow),(gboolean)false);
    gtk_window_set_default_size(GTK_WINDOW(hWnd->TopWindow),__top_window_width,__top_window_height);
    gtk_widget_set_size_request(GTK_WIDGET(hWnd->TopWindow),__top_window_width,__top_window_height);
    gtk_window_resize(GTK_WINDOW(hWnd->TopWindow),__top_window_width,__top_window_height);

    // Init Main Page Container
    hWnd->MainPageContainer=gtk_notebook_new();

    {
        hWnd->MainPage=gtk_vbox_new((gboolean)true,5);
        {
            GtkWidget* hbox=gtk_hbox_new((gboolean)false,5);
            hWnd->EventBox=gtk_event_box_new();
            {
                hWnd->DrawingArea=gtk_drawing_area_new();
                gtk_container_add(GTK_CONTAINER(hWnd->EventBox),GTK_WIDGET(hWnd->DrawingArea));
                gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(hWnd->EventBox),(gboolean)true,(gboolean)true,5);
            }

            hWnd->SettingPageContainer=gtk_notebook_new();
            {
                GtkWidget* _home_home_vbox=gtk_vbox_new((gboolean)true,5);
                GtkWidget* _home_home_lblHome=gtk_label_new((const gchar*)"Home");
                {
                    GdkColor color_message;
                    gdk_color_parse("#006400",&color_message);

                    GtkWidget* _home_home_lblSCommand=gtk_label_new((const gchar*)"S-Command");
                    GtkWidget* _home_home_lblSMode   =gtk_label_new((const gchar*)"S-Mode");
                    GtkWidget* _home_home_lblSParam  =gtk_label_new((const gchar*)"S-Param");
                    GtkWidget* _home_home_lblSMessage=gtk_label_new((const gchar*)"S-Message");

                    gtk_label_set_justify(GTK_LABEL(_home_home_lblSCommand),GTK_JUSTIFY_RIGHT);
                    gtk_label_set_justify(GTK_LABEL(_home_home_lblSMode),   GTK_JUSTIFY_RIGHT);
                    gtk_label_set_justify(GTK_LABEL(_home_home_lblSParam),  GTK_JUSTIFY_RIGHT);
                    gtk_label_set_justify(GTK_LABEL(_home_home_lblSMessage),GTK_JUSTIFY_RIGHT);

                    hWnd->_home_home_tbxSCommand=gtk_entry_new();
                    hWnd->_home_home_tbxSMode   =gtk_entry_new();
                    hWnd->_home_home_tbxSParam  =gtk_entry_new();
                    hWnd->_home_home_tbxSMessage=gtk_entry_new();

                    GtkWidget* _home_home_lblRCommand=gtk_label_new((const gchar*)"R-Command");
                    GtkWidget* _home_home_lblRMode   =gtk_label_new((const gchar*)"R-Mode");
                    GtkWidget* _home_home_lblRParam  =gtk_label_new((const gchar*)"R-Param");
                    GtkWidget* _home_home_lblRMessage=gtk_label_new((const gchar*)"R-Message");

                    gtk_label_set_justify(GTK_LABEL(_home_home_lblRCommand),GTK_JUSTIFY_RIGHT);
                    gtk_label_set_justify(GTK_LABEL(_home_home_lblRMode),   GTK_JUSTIFY_RIGHT);
                    gtk_label_set_justify(GTK_LABEL(_home_home_lblRParam),  GTK_JUSTIFY_RIGHT);
                    gtk_label_set_justify(GTK_LABEL(_home_home_lblRMessage),GTK_JUSTIFY_RIGHT);


                    hWnd->_home_home_tbxRCommand=gtk_entry_new();
                    hWnd->_home_home_tbxRMode   =gtk_entry_new();
                    hWnd->_home_home_tbxRParam  =gtk_entry_new();
                    hWnd->_home_home_tbxRMessage=gtk_entry_new();

                    gtk_widget_set_sensitive(GTK_WIDGET(hWnd->_home_home_tbxRCommand),(gboolean)false);
                    gtk_widget_set_sensitive(GTK_WIDGET(hWnd->_home_home_tbxRMode),   (gboolean)false);
                    gtk_widget_set_sensitive(GTK_WIDGET(hWnd->_home_home_tbxRParam),  (gboolean)false);
                    gtk_widget_set_sensitive(GTK_WIDGET(hWnd->_home_home_tbxRMessage),(gboolean)false);

                    GtkWidget* _home_home_table=gtk_table_new(11,3,(gboolean)false);
                    {
                        GtkWidget* _home_home_lblSend=gtk_label_new((const gchar*)"Send Message:");
                        gtk_widget_modify_fg(GTK_WIDGET(_home_home_lblSend),GTK_STATE_NORMAL,&color_message);
                        gtk_label_set_justify(GTK_LABEL(_home_home_lblSend),GTK_JUSTIFY_LEFT);
                        gtk_misc_set_alignment(GTK_MISC(_home_home_lblSend),0,0.5);
                        gtk_label_set_width_chars(GTK_LABEL(_home_home_lblSend),25);

                        gtk_table_set_col_spacing(GTK_TABLE(_home_home_table),2,10);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblSend),0,2,0,1,GTK_SHRINK,GTK_FILL,2,2);

                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblSCommand),0,1,1,2,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblSMode),   0,1,2,3,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblSParam),  0,1,3,4,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblSMessage),0,1,4,5,GTK_SHRINK,GTK_FILL,2,2);

                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxSCommand),1,2,1,2,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxSMode),   1,2,2,3,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxSParam),  1,2,3,4,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxSMessage),1,2,4,5,GTK_SHRINK,GTK_FILL,2,2);

                        GtkWidget* _home_home_lblReceive=gtk_label_new((const gchar*)"Receive Message:");
                        gtk_widget_modify_fg(GTK_WIDGET(_home_home_lblReceive),GTK_STATE_NORMAL,&color_message);
                        gtk_label_set_justify(GTK_LABEL(_home_home_lblReceive),GTK_JUSTIFY_LEFT);
                        gtk_misc_set_alignment(GTK_MISC(_home_home_lblReceive),0,0.5);
                        gtk_label_set_width_chars(GTK_LABEL(_home_home_lblReceive),25);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblReceive), 0,2,5,6,GTK_SHRINK,GTK_FILL,2,2);

                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblRCommand),0,1,6,7,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblRMode),   0,1,7,8,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblRParam),  0,1,8,9,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(_home_home_lblRMessage),0,1,9,10,GTK_SHRINK,GTK_FILL,2,2);

                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxRCommand),1,2,6,7,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxRMode),   1,2,7,8,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxRParam),  1,2,8,9,GTK_SHRINK,GTK_FILL,2,2);
                        gtk_table_attach(GTK_TABLE(_home_home_table),GTK_WIDGET(hWnd->_home_home_tbxRMessage),1,2,9,10,GTK_SHRINK,GTK_FILL,2,2);


                    }

                    GtkWidget* _home_home_bottom_vbox=gtk_vbox_new((gboolean)false,5);
                    {
                        GtkWidget* _home_home_bottom_hbox=gtk_hbox_new((gboolean)true,5);

                        hWnd->_home_home_btnConfig=gtk_button_new_with_label((const gchar*)"Config");
                        hWnd->_home_home_btnClear=gtk_button_new_with_label((const gchar*)"Clear");
                        hWnd->_home_home_btnSend=gtk_button_new_with_label((const gchar*)"Send");

                        gtk_box_pack_start(GTK_BOX(_home_home_bottom_hbox),GTK_WIDGET(hWnd->_home_home_btnConfig),false,true,10);
                        gtk_box_pack_start(GTK_BOX(_home_home_bottom_hbox),GTK_WIDGET(hWnd->_home_home_btnClear),false,true,10);
                        gtk_box_pack_start(GTK_BOX(_home_home_bottom_hbox),GTK_WIDGET(hWnd->_home_home_btnSend),false,true,10);

                        gtk_box_pack_end(GTK_BOX(_home_home_bottom_vbox),GTK_WIDGET(_home_home_bottom_hbox),false,true,10);
                    }
                    gtk_box_pack_end(GTK_BOX(_home_home_vbox),GTK_WIDGET(_home_home_bottom_vbox),true,true,5);

                    gtk_container_add(GTK_CONTAINER(_home_home_vbox),GTK_WIDGET(_home_home_table));

                }
                gtk_notebook_append_page(GTK_NOTEBOOK(hWnd->SettingPageContainer),GTK_WIDGET(_home_home_vbox),_home_home_lblHome);
                gtk_notebook_set_show_tabs(GTK_NOTEBOOK(hWnd->SettingPageContainer),(gboolean)false);


                //gtk_notebook_set_tab_pos(GTK_NOTEBOOK(hWnd->SettingPageContainer),GTK_POS_BOTTOM );
                gtk_box_pack_end(GTK_BOX(hbox),GTK_WIDGET(hWnd->SettingPageContainer),(gboolean)true,(gboolean)true,5);
                gtk_widget_set_size_request(GTK_WIDGET(hWnd->SettingPageContainer),(gint)((__top_window_width-40)*0.34),-1);
                {
                    gint _width(0),_height(0);
                    gtk_widget_get_size_request(GTK_WIDGET(hWnd->SettingPageContainer),&_width,&_height);
                    if(_width>__top_window_max_config_area_width)
                    {
                        gtk_widget_set_size_request(GTK_WIDGET(hWnd->SettingPageContainer),__top_window_max_config_area_width,-1);
                    }
                    if(_width<__top_window_min_config_area_width)
                    {
                        gtk_widget_set_size_request(GTK_WIDGET(hWnd->SettingPageContainer),__top_window_min_config_area_width,-1);
                    }
                }
            }

            gint _width(0),_height(0);
            gtk_widget_get_size_request(GTK_WIDGET(hWnd->SettingPageContainer),&_width,&_height);
            gtk_widget_set_size_request(GTK_WIDGET(hWnd->DrawingArea),__top_window_width-_width-10,__top_window_height-20);
            gtk_widget_get_size_request(GTK_WIDGET(hWnd->DrawingArea),&_width,&_height);
            gtk_drawing_area_size(GTK_DRAWING_AREA(hWnd->DrawingArea),_width,_height);
            gtk_box_pack_start(GTK_BOX(hWnd->MainPage),GTK_WIDGET(hbox),(gboolean)true,(gboolean)true,5);
            //gtk_container_add(GTK_CONTAINER(hWnd->MainPage),);
        }

        hWnd->ConfigPage=gtk_vbox_new((gboolean)true,5);
        {
            //GtkWidget* temp=gtk_widget_new();
            GtkWidget* hbox=gtk_hbox_new((gboolean)false,5);
            {
                //GtkWidget* _menu_vbox=gtk_vbox_new((gboolean)false,0);
                hWnd->ConfigEventBox=gtk_event_box_new();
                {
                    float scale(0.7);
                    //hWnd->_config_page_btnBack=gtk_button_new_with_label((const gchar*)"Back");
                    //hWnd->ConfigEventBox=gtk_event_box_new();
                    gint _menu_height(0),_menu_width(0);
                    gtk_widget_get_size_request(GTK_WIDGET(hWnd->SettingPageContainer),&_menu_width,&_menu_height);
                    //gtk_widget_set_size_request(GTK_WIDGET(hWnd->_config_page_btnBack),(gint)(_menu_width*scale),40);
                    //gint _btn_back_height(0),_btn_back_width(0);
                    //gtk_widget_get_size_request(GTK_WIDGET(hWnd->_config_page_btnBack),&_btn_back_width,&_btn_back_height);
                    gint _drawing_height(0),_drawing_width(0);
                    gtk_widget_get_size_request(GTK_WIDGET(hWnd->DrawingArea),&_drawing_width,&_drawing_height);
                    //gtk_widget_set_size_request(GTK_WIDGET(hWnd->ConfigEventBox),_btn_back_width,_drawing_height-_btn_back_height-20);
                    gtk_widget_set_size_request(GTK_WIDGET(hWnd->ConfigEventBox),_menu_width*scale,_drawing_height);
                    hWnd->ConfigMenu=gtk_drawing_area_new();
                    gtk_container_add(GTK_CONTAINER(hWnd->ConfigEventBox),GTK_WIDGET(hWnd->ConfigMenu));
                    gtk_widget_set_size_request(GTK_WIDGET(hWnd->ConfigMenu),_menu_width*scale,_drawing_height);

                    //gtk_box_pack_start(GTK_BOX(_menu_vbox),GTK_WIDGET(hWnd->_config_page_btnBack),(gboolean)false,(gboolean)false,5);
                    //gtk_box_pack_start(GTK_BOX(_menu_vbox),GTK_WIDGET(hWnd->ConfigEventBox),(gboolean)false,(gboolean)false,0);
                }

                hWnd->ConfigLayout=gtk_notebook_new();
                {
                    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(hWnd->ConfigLayout),(gboolean)false);
                    gint _layout_height(0),_layout_width(0);
                    gtk_widget_get_size_request(GTK_WIDGET(hWnd->ConfigEventBox),&_layout_width,&_layout_height);
                    gtk_widget_set_size_request(GTK_WIDGET(hWnd->ConfigLayout),_layout_width,-1);
                    //GtkWidget* lblTemp=gtk_label_new((const gchar*)"Temp");

                    GtkWidget* vbox=gtk_vbox_new((gboolean)true,0);
                    GtkWidget* config_main=gtk_label_new("Config");
                    gtk_notebook_append_page(GTK_NOTEBOOK(hWnd->ConfigLayout),GTK_WIDGET(vbox),GTK_WIDGET(config_main));
                }
                gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(hWnd->ConfigEventBox),(gboolean)false,(gboolean)false,0);
                gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(hWnd->ConfigLayout),(gboolean)true,(gboolean)true,0);
            }
            gtk_box_pack_start(GTK_BOX(hWnd->ConfigPage),GTK_WIDGET(hbox),(gboolean)true,(gboolean)true,5);
            //gtk_container_add(GTK_CONTAINER(hWnd->ConfigPage),GTK_WIDGET(hbox));
        }

        GtkWidget* lblPage1=gtk_label_new((const gchar*)"Home");
        gtk_notebook_append_page(GTK_NOTEBOOK(hWnd->MainPageContainer),GTK_WIDGET(hWnd->MainPage),GTK_WIDGET(lblPage1));

        GtkWidget* lblPage2=gtk_label_new((const gchar*)"Config");
        gtk_notebook_append_page(GTK_NOTEBOOK(hWnd->MainPageContainer),GTK_WIDGET(hWnd->ConfigPage),GTK_WIDGET(lblPage2));

        gtk_notebook_set_show_tabs(GTK_NOTEBOOK(hWnd->MainPageContainer),(gboolean)false);
    }

    // Add to window
    gtk_container_set_border_width(GTK_CONTAINER(hWnd->TopWindow), 5);
    gtk_container_add(GTK_CONTAINER(hWnd->TopWindow),GTK_WIDGET(hWnd->MainPageContainer));


    __init_cairo_config(hWnd);
}


void __home_home_tbxSCommand_changed(GtkWidget* _widget,gpointer data)
{
    //UIFrame* hWnd=static_cast<UIFrame*>(data);
    __home_home_clear_receive_message(data);
}

void __home_home_tbxSMode_changed(GtkWidget* _widget,gpointer data)
{
    //UIFrame* hWnd=static_cast<UIFrame*>(data);
    __home_home_clear_receive_message(data);
}

void __home_home_tbxSParam_changed(GtkWidget* _widget,gpointer data)
{
    //UIFrame* hWnd=static_cast<UIFrame*>(data);
    __home_home_clear_receive_message(data);
}

void __home_home_tbxSMessage_changed(GtkWidget* _widget,gpointer data)
{
    //UIFrame* hWnd=static_cast<UIFrame*>(data);
    __home_home_clear_receive_message(data);
}

void __home_home_tbxRCommand_changed(GtkWidget* _widget,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
}

void __home_home_tbxRMode_changed(GtkWidget* _widget,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
}

void __home_home_tbxRParam_changed(GtkWidget* _widget,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
}

void __home_home_tbxRMessage_changed(GtkWidget* _widget,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
}

gboolean __home_home_tbxSCommand_focus_in_event(GtkWidget* _widget,GdkEvent* _event,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    return false;
}

gboolean __home_home_tbxSCommand_focus_out_event(GtkWidget* _widget,GdkEvent* _event,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    return false;
}


gboolean __home_home_tbxSMode_focus_in_event(GtkWidget* _widget,GdkEvent* _event,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    return false;
}

gboolean __home_home_tbxSMode_focus_out_event(GtkWidget* _widget,GdkEvent* _event,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    return false;
}

void __home_home_DrawingArea_pop_up(GtkWidget* _widget,GdkEvent* _event,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
}

void __home_home_btnConfig_clicked(GtkWidget* _widget,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);

    if(NULL!=hWnd->MainPageContainer)
    {
        gint _index=gtk_notebook_page_num(GTK_NOTEBOOK(hWnd->MainPageContainer),GTK_WIDGET(hWnd->ConfigPage));
        if(-1!=_index)
        {
            gtk_notebook_set_current_page(GTK_NOTEBOOK(hWnd->MainPageContainer),_index);
        }
        else
        {
            std::fprintf(stderr, "%s\n","Failed to find \"Config\" Interface.");
        }
    }
}

void __home_home_btnClear_clicked(GtkWidget* _widget,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    if(hWnd->_home_home_tbxSCommand) gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxSCommand),(const gchar*)"");
    if(hWnd->_home_home_tbxSMode)    gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxSMode),   (const gchar*)"");
    if(hWnd->_home_home_tbxSParam)   gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxSParam),  (const gchar*)"");
    if(hWnd->_home_home_tbxSMessage) gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxSMessage),(const gchar*)"");
    if(hWnd->_home_home_tbxRCommand) gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRCommand),(const gchar*)"");
    if(hWnd->_home_home_tbxRMode)    gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRMode),   (const gchar*)"");
    if(hWnd->_home_home_tbxRParam)   gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRParam),  (const gchar*)"");
    if(hWnd->_home_home_tbxRMessage) gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRMessage),(const gchar*)"");
}


void __home_home_clear_receive_message(gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    if(hWnd->_home_home_tbxRCommand) gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRCommand),(const gchar*)"");
    if(hWnd->_home_home_tbxRMode)    gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRMode),   (const gchar*)"");
    if(hWnd->_home_home_tbxRParam)   gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRParam),  (const gchar*)"");
    if(hWnd->_home_home_tbxRMessage) gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRMessage),(const gchar*)"");
}

void __home_home_btnSend_clicked(GtkWidget* _widget,gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    int sk=__create_tcp_client("127.0.0.1",9600);

    tcp_st sd;
    char* stopflag=(char*)"";
    sd.id=strtol(gtk_entry_get_text(GTK_ENTRY(hWnd->_home_home_tbxSCommand)),&stopflag,0);
    sd.mode=strtol(gtk_entry_get_text(GTK_ENTRY(hWnd->_home_home_tbxSMode)),&stopflag,0);
    sd.ret=strtol(gtk_entry_get_text(GTK_ENTRY(hWnd->_home_home_tbxSParam)),&stopflag,0);
    const gchar* sm=gtk_entry_get_text(GTK_ENTRY(hWnd->_home_home_tbxSMessage));
    std::memset(sd.msg,0x00,sizeof(sd.msg));
    std::memmove(sd.msg,sm,sizeof(sd.msg));
    if(-1==send(sk,(void*)&sd,sizeof(sd),0))
    {
        perror("Error:");
    }

    tcp_st re;
    std::memset(&re,0x00,sizeof(tcp_st));
/*
    int total(0);
label_receive:
    int recv_byte=recv(sk,((char*)&re)+total,sizeof(re)-total,0);
    if(recv_byte<=0)
    {
        perror("Error Recv:");
        goto label_destroy;
    }
    else
    {
        total+=recv_byte;
        goto label_receive;
    }
*/
    char szBuf[256];
    std::memset(szBuf,0x00,sizeof(szBuf));
    std::snprintf(szBuf,sizeof(szBuf),"%d", (int)re.id);
    gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRCommand),(const gchar*)_UTF8(std::string(szBuf).c_str()));
    std::memset(szBuf,0x00,sizeof(szBuf));
    std::snprintf(szBuf,sizeof(szBuf),"%d", (int)re.mode);
    gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRMode),(const gchar*)_UTF8(std::string(szBuf).c_str()));
    std::memset(szBuf,0x00,sizeof(szBuf));
    std::snprintf(szBuf,sizeof(szBuf),"%d", (int)re.ret);
    gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRParam),(const gchar*)_UTF8(std::string(szBuf).c_str()));
    std::memset(szBuf,0x00,sizeof(szBuf));
    std::snprintf(szBuf,sizeof(szBuf),"%s", std::string(re.msg).c_str());
    gtk_entry_set_text(GTK_ENTRY(hWnd->_home_home_tbxRMessage),(const gchar*)_UTF8(std::string(re.msg).c_str()));
    usleep(1000000);
label_destroy:
    __destroy_tcp_client(sk);
}



static bool __S_g_is_above_drawing_area(false);
static std::list<DrawingRect>   __S_g_list_rect;
static DrawingCenter __S_g_pos={-1,-1};

gboolean __home_home_DrawingArea_enter_notify_event(GtkWidget* _widget,GdkEventCrossing* event,gpointer data)
{
    //std::fprintf(stderr, "%s\n","enter-notify-event");
    __S_g_is_above_drawing_area=true;
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    if(hWnd->DrawingArea)
    {
        gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(hWnd->DrawingArea)),gdk_cursor_new(GDK_CROSS));
    }
    if(-1==__S_g_pos.x || -1==__S_g_pos.y)
    {
        __S_g_pos.x=event->x;
        __S_g_pos.y=event->y;
    }
    return false;
}

gboolean __home_home_DrawingArea_leave_notify_event(GtkWidget* _widget,GdkEventCrossing* event,gpointer data)
{
    //std::fprintf(stderr, "%s\n","leave-notify-event");
    __S_g_is_above_drawing_area=false;
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    if(hWnd->DrawingArea)
    {
        gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(hWnd->DrawingArea)),NULL);
    }
    return false;
}


gboolean __home_home_DrawingArea_motion_notify_event(GtkWidget* _widget,GdkEventMotion* event,gpointer data)
{
    //std::fprintf(stderr, "%s = %d , %d \n","motion-notify-event",(int)event->x,(int)event->y);
    __S_g_pos.x=static_cast<float>(event->x);
    __S_g_pos.y=static_cast<float>(event->y);
    //gdk_event_request_motions(event);
    //std::fprintf(stderr, "%s = %d , %d \n","motion-notify-event => ",event->x,event->y);
    return false;
}



void __create_image_from_file(gpointer data,const gchar* filename)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    hWnd->cairo_config.image_jessie=cairo_image_surface_create_from_png(filename);
    if(NULL!=hWnd->cairo_config.image_jessie)
    {
        int image_width=cairo_image_surface_get_width(hWnd->cairo_config.image_jessie);
        int image_height=cairo_image_surface_get_height(hWnd->cairo_config.image_jessie);
        hWnd->cairo_config.scale_x=hWnd->cairo_config.scale_y=1.0f;
        if(NULL!=hWnd->DrawingArea)
        {
            gint window_width(0),window_height(0);
            gtk_widget_get_size_request(GTK_WIDGET(hWnd->DrawingArea),&window_width,&window_height);
            hWnd->cairo_config.scale_x=static_cast<float>(window_width)/image_width;
            hWnd->cairo_config.scale_y=static_cast<float>(window_height)/image_height;
        }

    }

}




void __destroy_image_from_file(gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);

}

void __create_image_from_memory(gpointer data)
{

}
void __empty_image_render(gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    cairo_t* cr=gdk_cairo_create(gtk_widget_get_window(GTK_WIDGET(hWnd->DrawingArea)));
    if(cr)
    {
        gint _width(0),_height(0);
        gtk_widget_get_size_request(GTK_WIDGET(hWnd->DrawingArea),&_width,&_height);
        cairo_text_extents_t extents;
        const char* _text="Setpic";
        cairo_select_font_face(cr,"Sans",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr,90.0);
        cairo_text_extents(cr,_text,&extents);
        cairo_move_to(cr,(_width- extents.width)/2.0-20 ,(_height-extents.height)*0.85);
        cairo_show_text(cr,_text);
        cairo_move_to(cr,(_width- extents.width)/2.0+45,(_height-extents.height)*0.85+(extents.height)*0.6);
        cairo_set_font_size(cr,55.0);
        cairo_text_path(cr,"void function");
        cairo_set_source_rgb(cr,0.5,0.5,1);
        cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr,0,0,0);
        cairo_set_line_width(cr,2.56);
        cairo_stroke(cr);

        cairo_set_source_rgba(cr,0.2,0.6,0.2,0.4);
        cairo_set_line_width(cr,2.0);
        if(!__S_g_is_above_drawing_area)
        {
            cairo_move_to(cr,_width/2.0,0);
            cairo_rel_line_to(cr,0,_height*1.0);
            cairo_move_to(cr,0,_height/2.0);
            cairo_rel_line_to(cr,_width*1.0,0);
        }
        else
        {
            cairo_move_to(cr,__S_g_pos.x,0);
            cairo_rel_line_to(cr,0,_height*1.0);
            cairo_move_to(cr,0,__S_g_pos.y);
            cairo_rel_line_to(cr,_width*1.0,0);
        }
        cairo_stroke(cr);
        cairo_destroy(cr);
    }
}

void __drawing_area_render_impl(gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    if(NULL==hWnd->DrawingArea) return;
    if(0==hWnd->cairo_config.enable || -1==hWnd->cairo_config.current)
    {
        __empty_image_render(data);
        usleep(5000);
    }
    else
    {

        usleep(33000);
    }
    /*
    cairo_t* cr=gdk_cairo_create(gtk_widget_get_window(GTK_WIDGET(hWnd->DrawingArea)));
    if(cr)
    {
        if(0==hWnd->cairo_config.current)
        {

        }
        else
        {

        }

        //cairo_arc (cr, 500.0, 500.0, 256, 0, 2*3.1415926);
        //cairo_clip (cr);
        //cairo_new_path (cr);

        cairo_surface_t *image = cairo_image_surface_create_from_png ("./2.png");
        gint w = cairo_image_surface_get_width (image);
        gint h = cairo_image_surface_get_height (image);

        cairo_scale (cr, 500.0/w, 500.0/h);
        std::fprintf(stderr, "%s w=%d h=%d\n","=>",w,h);
        //      cairo_mask_surface(cr,image,0,0);
        cairo_set_source_surface (cr, image, 0, 0);
        //cairo_mask_surface(cr,image,0,0);
        cairo_paint (cr);

        cairo_surface_destroy (image);

        //cairo_clip(cr);
        //cairo_fill(cr);
        cairo_paint(cr);
        cairo_stroke(cr);
        cairo_destroy(cr);
    }
    */

}


gboolean __home_home_DrawingArea_expose_event(GtkWidget* _widget,GdkEventExpose* event,gpointer data)
{
    //std::fprintf(stderr, "%s\n","expose_event");
    __drawing_area_render_impl(data);
    //usleep(33000);
    gtk_widget_queue_draw(_widget);
    return (gboolean)false;
}

/*
***************************************************
*
*
*
***************************************************
*/






void __config_drawing_config_menu(gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    cairo_t* cr=gdk_cairo_create(gtk_widget_get_window(GTK_WIDGET(hWnd->ConfigMenu)));
    if(cr)
    {
        gint _width(0),_height(0);
        gtk_widget_get_size_request(GTK_WIDGET(hWnd->ConfigMenu),&_width,&_height);

        cairo_move_to(cr,0,0);
        cairo_pattern_t* pat=cairo_pattern_create_linear(0,0,_width,_height);
        cairo_pattern_add_color_stop_rgba (pat, 0.2, 0.2, 0.4, 0.6,0.3);
        cairo_pattern_add_color_stop_rgba (pat, 0.4, 0.4, 0.63, 0.8,0.2);
        cairo_pattern_add_color_stop_rgba (pat, 0.6, 0.2, 0.38, 0.6,0.3);
        cairo_pattern_add_color_stop_rgba (pat, 0.8, 0.3, 0.44, 0.7,0.2);
        cairo_pattern_add_color_stop_rgba (pat, 0.9, 0.2, 0.4, 0.6,0.3);
        cairo_pattern_add_color_stop_rgba (pat, 1.0, 0.2, 0.4, 0.7,0.4);

        //cairo_pattern_t* pat2=cairo_pattern_create_linear(20,0,_width-20,_height);
        //cairo_pattern_add_color_stop_rgba(pat2,0.3,0.2,0.2,0.2,0.1);
        //cairo_pattern_add_color_stop_rgba(pat2,0.7,0.3,0.3,0.3,0.1);
        //cairo_pattern_add_color_stop_rgba(pat2,0.9,0.2,0.3,0.2,0.1);

        cairo_rectangle(cr,0,0,_width,_height);

        //cairo_set_source(cr,pat2);
        //cairo_fill (cr);
        cairo_set_source(cr,pat);
        cairo_fill (cr);
        cairo_pattern_destroy(pat);
        //cairo_pattern_destroy(pat2);
        cairo_select_font_face(cr,"Sans",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_source_rgba(cr,0.8,0.8,0.8,0.8);
        cairo_set_font_size(cr,10.0);
        cairo_show_text(cr,"Base Config");

        cairo_set_source_rgba(cr,0.0,0.01,0.0,0.8);
        cairo_text_extents_t extents;
        const char* _text="Setpic";
        cairo_select_font_face(cr,"Sans",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr,40.0);
        cairo_text_extents(cr,_text,&extents);
        cairo_move_to(cr,(_width- extents.width)/2.0-20 ,(_height-extents.height)*0.85);
        cairo_show_text(cr,_text);
        cairo_move_to(cr,(_width- extents.width)/2.0,(_height-extents.height)*0.85+(extents.height)*0.6);
        cairo_set_font_size(cr,28.0);
        cairo_text_path(cr,"void function");
        cairo_set_source_rgb(cr,0.5,0.5,1);
        cairo_fill_preserve(cr);
        cairo_set_source_rgb(cr,0,0,0);
        cairo_set_line_width(cr,0.56);
        cairo_stroke(cr);

        int _step_height=25;
        cairo_move_to(cr,20,_step_height*1);
        cairo_set_font_size(cr,12);
        cairo_show_text(cr,"basic configuration");
        cairo_move_to(cr,20,_step_height*2);
        cairo_show_text(cr,"tcp/ip & modbus");


        cairo_set_font_size(cr,18);
        cairo_move_to(cr,_width/2-15,_height-40);
        cairo_set_source_rgb(cr,0.7,1.0,0.9);
        cairo_show_text(cr,"[ BACK ]");

/*
        cairo_set_source_rgba(cr,0.2,0.6,0.2,0.4);
        cairo_set_line_width(cr,2.0);
        {
            cairo_move_to(cr,_width/2.0,0);
            cairo_rel_line_to(cr,0,_height*1.0);
            cairo_move_to(cr,0,_height/2.0);
            cairo_rel_line_to(cr,_width*1.0,0);
        }
        cairo_stroke(cr);
*/
        cairo_destroy(cr);
    }
}

gboolean __config_menu_DrawingMenu_expose_event(GtkWidget* _widget,GdkEventExpose* event,gpointer data)
{
    __config_drawing_config_menu(data);
    usleep(40000);
    gtk_widget_queue_draw(_widget);
    return false;
}



void __init_uiframe_signal(UIFrame* hWnd)
{
    g_signal_connect(GTK_OBJECT(hWnd->TopWindow),"destroy",G_CALLBACK(__destroy),NULL);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSCommand),"changed",G_CALLBACK(__home_home_tbxSCommand_changed),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSMode),   "changed",G_CALLBACK(__home_home_tbxSMode_changed),   (gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSParam),  "changed",G_CALLBACK(__home_home_tbxSParam_changed),  (gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSMessage),"changed",G_CALLBACK(__home_home_tbxSMessage_changed),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxRCommand),"changed",G_CALLBACK(__home_home_tbxRCommand_changed),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxRMode),   "changed",G_CALLBACK(__home_home_tbxRMode_changed),   (gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxRParam),  "changed",G_CALLBACK(__home_home_tbxRParam_changed),  (gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxRMessage),"changed",G_CALLBACK(__home_home_tbxRMessage_changed),(gpointer)hWnd);

    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSCommand),"focus-in-event",G_CALLBACK(__home_home_tbxSCommand_focus_in_event),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSMode),   "focus-in-event",G_CALLBACK(__home_home_tbxSMode_focus_in_event),   (gpointer)hWnd);

    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSCommand),"focus-out-event",G_CALLBACK(__home_home_tbxSCommand_focus_out_event),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_tbxSMode),   "focus-out-event",G_CALLBACK(__home_home_tbxSMode_focus_out_event),   (gpointer)hWnd);

    g_signal_connect(GTK_OBJECT(hWnd->_home_home_btnConfig),   "clicked",G_CALLBACK(__home_home_btnConfig_clicked),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_btnClear),    "clicked",G_CALLBACK(__home_home_btnClear_clicked), (gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->_home_home_btnSend),     "clicked",G_CALLBACK(__home_home_btnSend_clicked),  (gpointer)hWnd);

    gtk_widget_set_events(GTK_WIDGET(hWnd->DrawingArea),GDK_EXPOSURE_MASK|GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_POINTER_MOTION_MASK|GDK_POINTER_MOTION_HINT_MASK);
    //gtk_widget_set_events(hWnd->EventBox,GDK_LEAVE_NOTIFY_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_POINTER_MOTION_MASK|GDK_POINTER_MOTION_HINT_MASK);
    g_signal_connect(GTK_OBJECT(hWnd->DrawingArea),"expose-event",G_CALLBACK(__home_home_DrawingArea_expose_event),(gpointer)hWnd);

    g_signal_connect(GTK_OBJECT(hWnd->DrawingArea),"enter-notify-event",G_CALLBACK(__home_home_DrawingArea_enter_notify_event),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->DrawingArea),"leave-notify-event",G_CALLBACK(__home_home_DrawingArea_leave_notify_event),(gpointer)hWnd);
    g_signal_connect(GTK_OBJECT(hWnd->DrawingArea),"motion-notify-event",G_CALLBACK(__home_home_DrawingArea_motion_notify_event),(gpointer)hWnd);


    g_signal_connect(GTK_OBJECT(hWnd->ConfigMenu),"expose-event",G_CALLBACK(__config_menu_DrawingMenu_expose_event),(gpointer)hWnd);

}

bool __init_shared(const char* lpszPath,int section)
{
    UIFrame* hWnd=&_g_hwnd;
    if(init_getpic(lpszPath) && init_shared_memory(section))
    {
        std::memmove(hWnd->shared_config.szPath,lpszPath,sizeof(hWnd->shared_config.szPath));
        hWnd->shared_config.section=section;
        return hWnd->shared_memory_status=true;
    }
    else
    {
        return hWnd->shared_memory_status=false;
    }

}


void init(UIFrame* hWnd)
{
    __init_uiframe(hWnd,"getpic");
    __init_uiframe_signal(hWnd);
    __init_shared();
}


gint    timeout_callback(gpointer data)
{
    UIFrame* hWnd=static_cast<UIFrame*>(data);
    if(!hWnd->shared_memory_status) ;
    if(0==hWnd->save_buffer_index%2)
    {
        if(get_image_test(img0,hWnd->ulldTimeStamp))
        {
            hWnd->next_ui_buffer_index=0;
            hWnd->save_buffer_index++;
        }
    }
    else
    {
        if(get_image_test(img1,hWnd->ulldTimeStamp))
        {
            hWnd->next_ui_buffer_index=1;
            hWnd->save_buffer_index++;
        }
    }
    return static_cast<gint>(true);
}

int main(int argc,char* argv[])
{
    bool b_success(true);
    /* Auto Init after Version.2.32 */
    /*
    if(!g_thread_supported())
    {
        g_thread_init(NULL);
    }
    */
    gdk_threads_init();
    gdk_threads_enter();
    if(!gtk_init_check(&argc,&argv))
    {
        b_success=false;
        goto label_exit;
    }
    init(&_g_hwnd);
    gtk_widget_show_all(GTK_WIDGET(_g_hwnd.TopWindow));
    _g_hwnd.shared_memory_timer=gtk_timeout_add(20,timeout_callback,&_g_hwnd);
    gtk_main();
    gtk_timeout_remove(_g_hwnd.shared_memory_timer);
    _g_hwnd.shared_memory_timer=-1;
    gdk_threads_leave();
    label_exit:
    if(b_success) return EXIT_SUCCESS;
    else return EXIT_FAILURE;
}