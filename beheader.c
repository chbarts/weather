#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "beheader.h"

static void g_beheader_iface_init          (GConverterIface *iface);

G_DEFINE_TYPE_WITH_CODE (GBeheader, g_beheader, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (G_TYPE_CONVERTER,
						g_beheader_iface_init))

static void
g_beheader_class_init (GBeheaderClass *klass)
{
}

static void
g_beheader_init (GBeheader *local)
{
   local->seen_hend = false;
   local->nnls = 0;
   local->ncrs = 0;
}

GConverter *
g_beheader_new (void)
{
  GConverter *conv;

  conv = g_object_new (G_TYPE_BEHEADER, NULL);
  g_beheader_init((GBeheader*)conv);

  return conv;
}

static void g_beheader_reset(GConverter *converter)
{
   GBeheader *self = (GBeheader*) converter;
   self->seen_hend = false;
   self->ncrs = 0;
   self->nnls = 0;
}

static GConverterResult g_beheader_behead(GConverter *converter,
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
   GBeheader *self = (GBeheader*) converter;

   if (self->seen_hend) {
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
      if (self->seen_hend) {
         remsize = in_end - in;
         txfersize = ((remsize < outbuf_size) ? remsize : outbuf_size);
         memcpy(outbuf, in, txfersize);
         *bytes_read = (((void*)in) - inbuf) + txfersize;
         *bytes_written = txfersize;
         if (flags & G_CONVERTER_INPUT_AT_END) {
            return G_CONVERTER_FINISHED;
         } else {
            return G_CONVERTER_CONVERTED;
         }
      }
      
      v = *in;

      if ('\r' == v) {
         self->ncrs++;
      } else if ('\n' == v) {
         self->nnls++;
      } else {
         self->ncrs = 0;
         self->nnls = 0;
      }

      in++;

      if ((2 == self->ncrs) && (2 == self->nnls)) {
         self->seen_hend = true;
      }
   }

   *bytes_read = inbuf_size;
   *bytes_written = 0;
   return G_CONVERTER_CONVERTED;
}

static void
g_beheader_iface_init (GConverterIface *iface)
{
  iface->convert = g_beheader_behead;
  iface->reset = g_beheader_reset;
}
