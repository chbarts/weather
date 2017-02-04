#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include <errno.h>

GtkWidget *g_entry1, *g_textview1, *g_statusbar1;

void on_window1_destroy(void)
{
   gtk_main_quit();
}

void set_statusbar(char *type, char *message)
{
   guint cid;
   g_object_set_data(G_OBJECT(g_statusbar1), type, (gpointer) message);
   cid = gtk_statusbar_get_context_id(GTK_STATUSBAR(g_statusbar1), type);
   gtk_statusbar_push(GTK_STATUSBAR(g_statusbar1), cid, message);
}

void on_button1_clicked(void)
{
   int len;
   char *txt, ftext[BUFSIZ];
   GtkTextBuffer *buf;
   GtkTextIter iter;
   GError *error = NULL;
   GSocketConnection *connection;
   GSocketClient *client;
   GInputStream *istream;
   GOutputStream *ostream;

   client = g_socket_client_new();

   txt = gtk_entry_get_text(g_entry1);

   connection = g_socket_client_connect_to_host(client, txt, 80, NULL, &error);
   if (error) {
      snprintf(ftext, BUFSIZ, "error: couldn't open connection to %s: %s", txt, error->message);
      set_statusbar("error", ftext);
      g_error_free(error);
      return;
   }

   g_tcp_connection_set_graceful_disconnect(G_TCP_CONNECTION(connection), TRUE);

   istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
   ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

#define HTTP_STUFF "GET / HTTP/1.1\r\nConnection: close\r\n\r\n"
#define HTTP_LEN   (sizeof(HTTP_STUFF)-1)
   error = NULL;
   if ((len = g_output_stream_write(ostream, HTTP_STUFF, HTTP_LEN, NULL, &error)) != HTTP_LEN) {
      if (error) {
         snprintf(ftext, BUFSIZ, "error: couldn't write to %s: %s", txt, error->message);
         set_statusbar("error", ftext);
         g_error_free(error);
         g_object_unref(connection);
         return;
      } else {
         while ((len = g_output_stream_write(ostream, HTTP_STUFF[len], HTTP_LEN - len, NULL, &error)) != 0) {
            if (error) {
               snprintf(ftext, BUFSIZ, "error: couldn't write to %s: %s", txt, error->message);
               set_statusbar("error", ftext);
               g_error_free(error);
               g_object_unref(connection);
               return;
            }
         }
      }
   }
   
   buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_textview1));
   gtk_text_buffer_set_text(buf, "", -1);
   gtk_text_buffer_get_start_iter(buf, &iter);

   error = NULL;
   while ((len = (int) g_input_stream_read(istream, ftext, BUFSIZ, NULL, &error)) > 0) {
      if (error) {
         snprintf(ftext, BUFSIZ, "error: couldn't read from %s: %s", txt, error->message);
         set_statusbar("error", ftext);
         g_error_free(error);
         g_object_unref(connection);
         return;
      } else {
         gtk_text_buffer_insert(buf, &iter, ftext, len);
      }
   }

   g_object_unref(connection);
   set_statusbar("info", txt);
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
   g_statusbar1 = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar1"));
 
   g_object_unref(builder);
 
   gtk_widget_show(window);                
   gtk_main();
 
   return 0;
}

