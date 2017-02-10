#ifndef BEHEADER_H
#define BEHEADER_H

#include <glib/glib.h>
#include <gio/gio.h>

struct _GBeheaderClass
{
  GObjectClass parent_class;
};

struct _GBeheader
{
  GObject parent_instance;
};

typedef struct _GBeheader       GBeheader;
typedef struct _GBeheaderClass  GBeheaderClass;

GType       g_beheader_get_type (void) G_GNUC_CONST;
GConverter *g_beheader_new      (void);

#define G_TYPE_BEHEADER         (g_beheader_get_type ())
#define G_BEHEADER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), G_TYPE_BEHEADER, GBeheader))
#define G_BEHEADER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), G_TYPE_BEHEADER, GBeheaderClass))
#define G_IS_BEHEADER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), G_TYPE_BEHEADER))
#define G_IS_BEHEADER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), G_TYPE_BEHEADER))
#define G_BEHEADER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), G_TYPE_BEHEADER, GBeheaderClass))

#endif
