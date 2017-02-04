#include <gtk/gtk.h>

GtkWidget *g_entry1, *g_textview1;

void on_window1_destroy(void)
{
   gtk_main_quit();
}

void on_button1_clicked(void)
{
   char *txt;
   GtkTextBuffer *buf;

   txt = gtk_entry_get_text(g_entry1);
   buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_textview1));
   gtk_text_buffer_set_text(buf, txt, -1);
//   gtk_text_view_set_buffer(g_textview1, buf);
}

int main(int argc, char *argv[])
{
   GtkBuilder      *builder; 
   GtkWidget       *window;

   gtk_init(&argc, &argv);
 
   builder = gtk_builder_new();
   gtk_builder_add_from_file (builder, "weather.glade", NULL);
 
   window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
   gtk_builder_connect_signals(builder, NULL);

   g_entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
   g_textview1 = GTK_WIDGET(gtk_builder_get_object(builder, "textview1"));
 
   g_object_unref(builder);
 
   gtk_widget_show(window);                
   gtk_main();
 
   return 0;
}

