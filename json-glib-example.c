#include <stdlib.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

int main(int argc, char *argv[])
{
   JsonGenerator *generator;
   JsonParser *parser;
   JsonPath *path;
   JsonNode *result;
   GError *error;
   char *str;

   if (argc != 3) {
      g_print("Usage: json-glib-example json-path filename.json\n");
      exit(EXIT_FAILURE);
   }

   parser = json_parser_new();

   error = NULL;
   json_parser_load_from_file(parser, argv[1], &error);
   if (error) {
      g_print("Unable to parse file '%s': %s\n", argv[1], error->message);
      g_error_free(error);
      g_object_unref(parser);
      exit(EXIT_FAILURE);
   }

   error = NULL;
   path = json_path_new();
   json_path_compile(path, argv[2], &error);
   if (error) {
      g_print("Unable to parse path '%s': %s\n", argv[2], error->message);
      g_error_free(error);
      g_object_unref(parser);
      exit(EXIT_FAILURE);
   }

   result = json_path_match(path, json_parser_get_root (parser));
   generator = json_generator_new();
   json_generator_set_root(generator, result);
   str = json_generator_to_data(generator, NULL);
   g_print("%s\n", str);

   g_free(str);
   g_object_unref(parser);
   exit(EXIT_SUCCESS);
}
