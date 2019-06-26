/*
 * Copyright (C) 2019 Zander Brown <zbrown@gnome.org>
 * Copyright (C) 2019 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#pragma once

#include "gtkbox.h"
#include "gtkhdytype.h"
#include "gtkstack.h"

G_BEGIN_DECLS

#define GTK_TYPE_HDY_VIEW_SWITCHER (gtk_hdy_view_switcher_get_type())

struct _GtkHdyViewSwitcherClass {
  GtkBoxClass parent_class;
};

GTK_HDY_DECLARE_DERIVABLE_TYPE (GtkHdyViewSwitcher, gtk_hdy_view_switcher, GTK, HDY_VIEW_SWITCHER, GtkBox)

typedef enum {
  GTK_HDY_VIEW_SWITCHER_POLICY_AUTO,
  GTK_HDY_VIEW_SWITCHER_POLICY_NARROW,
  GTK_HDY_VIEW_SWITCHER_POLICY_WIDE,
} GtkHdyViewSwitcherPolicy;

GtkHdyViewSwitcher *gtk_hdy_view_switcher_new (void);

GtkHdyViewSwitcherPolicy gtk_hdy_view_switcher_get_policy (GtkHdyViewSwitcher *self);
void                     gtk_hdy_view_switcher_set_policy (GtkHdyViewSwitcher       *self,
                                                           GtkHdyViewSwitcherPolicy  policy);

GtkIconSize gtk_hdy_view_switcher_get_icon_size (GtkHdyViewSwitcher *self);
void        gtk_hdy_view_switcher_set_icon_size (GtkHdyViewSwitcher *self,
                                                 GtkIconSize         icon_size);

PangoEllipsizeMode gtk_hdy_view_switcher_get_narrow_ellipsize (GtkHdyViewSwitcher *self);
void               gtk_hdy_view_switcher_set_narrow_ellipsize (GtkHdyViewSwitcher *self,
                                                               PangoEllipsizeMode  mode);

GtkStack *gtk_hdy_view_switcher_get_stack (GtkHdyViewSwitcher *self);
void      gtk_hdy_view_switcher_set_stack (GtkHdyViewSwitcher *self,
                                           GtkStack           *stack);

G_END_DECLS
