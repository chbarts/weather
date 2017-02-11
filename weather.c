#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

#include <errno.h>

#include "beheader.h"

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

void handle_json(GInputStream *istream, GError **error)
{
   JsonGenerator *generator;
   JsonParser *parser;
   GtkTextBuffer *buf;
   GtkTextIter iter;
   JsonNode *result;
   JsonPath *path;
   char *str;
   
   buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_textview1));
   gtk_text_buffer_set_text(buf, "", -1);
   gtk_text_buffer_get_start_iter(buf, &iter);

   parser = json_parser_new();
   if (!json_parser_load_from_stream(parser, istream, NULL, error)) {
      g_object_unref(parser);
      return;
   }

   *error = NULL;
   path = json_path_new();
   json_path_compile(path, ".cod", error);
   if (*error) {
      g_object_unref(parser);
      return;
   }

   result = json_path_match(path, json_parser_get_root(parser));
   generator = json_generator_new();
   json_generator_set_root(generator, result);
   str = json_generator_to_data(generator, NULL);

   gtk_text_buffer_insert(buf, &iter, str, strlen(str));

   g_free(str);
   json_node_unref(result);
   g_object_unref(parser);
}

#define API_KEY "85a4e3c55b73909f42c6a23ec35b7147"
/* #define URL "http://api.openweathermap.org/data/2.5/$api_cmd?q=$location&units=$units&appid=$api_key" */
/* "http://api.openweathermap.org/data/2.5/weather?q=$location&units=imperial&appid=$api_key" */
#define REQ "data/2.5/weather?q=%s&units=imperial&appid=%s"

void on_button1_clicked(void)
{
   int len, http_len;
   char *txt, *ind, ftext[BUFSIZ], http_stuff[BUFSIZ], host[BUFSIZ], req[BUFSIZ];
   GError *error = NULL;
   GSocketConnection *connection;
   GSocketClient *client;
   GInputStream *istream, *cistream;
   GOutputStream *ostream;
   GConverter *beheader;

   client = g_socket_client_new();
   beheader = g_beheader_new();

   txt = gtk_entry_get_text(g_entry1);
   strncpy(host, txt, BUFSIZ);

   if (ind = strchr(host, ':')) {
      *ind = '\0';
   }

   connection = g_socket_client_connect_to_host(client, "api.openweathermap.org", 80, NULL, &error);
   if (error) {
      snprintf(ftext, BUFSIZ, "error: couldn't open connection to api.openweathermap.org: %s", error->message);
      set_statusbar("error", ftext);
      g_error_free(error);
      g_object_unref(beheader);
      return;
   }

   g_tcp_connection_set_graceful_disconnect(G_TCP_CONNECTION(connection), TRUE);

   istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
   ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

   cistream = g_converter_input_stream_new(istream, beheader);

   snprintf(req, BUFSIZ, REQ, txt, API_KEY);
   http_len = snprintf(http_stuff, BUFSIZ, "GET /%s HTTP/1.1\r\nHost: api.openweathermap.org\r\nConnection: close\r\n\r\n", req);
   error = NULL;
   if ((len = g_output_stream_write(ostream, http_stuff, http_len, NULL, &error)) != http_len) {
      if (error) {
         snprintf(ftext, BUFSIZ, "error: couldn't write to %s: %s", txt, error->message);
         set_statusbar("error", ftext);
         g_error_free(error);
         g_object_unref(connection);
         g_object_unref(beheader);
         return;
      } else {
         while ((len = g_output_stream_write(ostream, http_stuff + len, http_len - len, NULL, &error)) != 0) {
            if (error) {
               snprintf(ftext, BUFSIZ, "error: couldn't write to %s: %s", txt, error->message);
               set_statusbar("error", ftext);
               g_error_free(error);
               g_object_unref(connection);
               g_object_unref(beheader);
               return;
            }
         }
      }
   }

   error = NULL;
   handle_json(cistream, &error);
   if (error) {
      snprintf(ftext, BUFSIZ, "error: couldn't read from %s: %s", txt, error->message);
      set_statusbar("error", ftext);
      g_error_free(error);
      g_object_unref(connection);
      g_object_unref(beheader);
      return;
   }

   g_object_unref(connection);
   g_object_unref(beheader);
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

