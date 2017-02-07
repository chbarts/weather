#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <glib/glib.h>
#include <gio/gio.h>

static bool seen_hend = false;
static int ncrs = 0, nnls = 0;

void reset(GConverter *converter)
{
   seen_hend = false;
   ncrs = 0;
   nnls = 0;
}

GConverterResult beheader(GConverter *converter,
                          const void *inbuf,
                          gsize       inbuf_size,
                          void       *outbuf,
                          gsize       outbuf_size,
                          GConverterFlags flags,
                          gsize      *bytes_read,
                          gsize      *bytes_written,
                          GError    **error)
{
   const guint8 *in, *in_end;
   gsize txfersize, remsize;
   guint8 v, *out;

   if (seen_hend) {
      txfersize = ((inbuf_size < outbuf_size) ? inbuf_size : outbuf_size);
      memcpy(outbuf, inbuf, txfersize);
      *bytes_read = txfersize;
      *bytes_written = txfersize;
      if (flags & G_CONVERTER_INPUT_AT_END) {
         return G_CONVERTER_FINISHED;
      } else {
         return G_CONVERTER_CONVERTED;
      }
   }

   in = inbuf;
   out = outbuf;
   in_end = in + inbuf_size;

   while (in < in_end) {
      if (seen_hend) {
         remsize = in_end - in;
         txfersize = ((remsize < outbuf_size) ? remsize : outbuf_size);
         memcpy(outbuf, in, txfersize);
         *bytes_read = txfersize;
         *bytes_written = txfersize;
         if (flags & G_CONVERTER_INPUT_AT_END) {
            return G_CONVERTER_FINISHED;
         } else {
            return G_CONVERTER_CONVERTED;
         }
      }
      
      v = *in;

      if ('\r' == v) {
         ncrs++;
      } else if ('\n' == v) {
         nnls++;
      } else {
         ncrs = 0;
         nnls = 0;
      }

      in++;

      if ((2 == ncrs) && (2 == nnls)) {
         seen_hend = true;
      }
   }

   return G_CONVERTER_CONVERTED;
}
