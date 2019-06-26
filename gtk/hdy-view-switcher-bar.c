/*
 * Copyright (C) 2019 Zander Brown <zbrown@gnome.org>
 * Copyright (C) 2019 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include "gtkactionbar.h"
#include "gtkcssprovider.h"
#include "gtkenums.h"
#include "gtkprivatetypebuiltins.h"
#include "gtkstylecontext.h"
#include "gtkrevealer.h"
#include "hdy-style-private.h"
#include "hdy-view-switcher-bar-private.h"
#include "hdy-view-switcher-bar-private.h"

/**
 * SECTION:hdy-view-switcher-bar
 * @short_description: An view switcher action bar
 * @title: GtkHdyViewSwitcherBar
 *
 * An action bar letting you switch between multiple views offered by a
 * #GtkStack, via a #GtkHdyViewSwitcher. It is designed to be put at the bottom of
 * a window and to be revealed only on really narrow windows e.g. on mobile
 * phones.
 *
 * Since: 0.0.10
 */

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GtkCssProvider, g_object_unref)

enum {
  PROP_0,
  PROP_POLICY,
  PROP_ICON_SIZE,
  PROP_STACK,
  PROP_REVEAL,
  LAST_PROP,
};

typedef struct {
  GtkActionBar *action_bar;
  GtkRevealer *revealer;
  GtkHdyViewSwitcher *view_switcher;

  GtkHdyViewSwitcherPolicy policy;
  GtkIconSize icon_size;
  gboolean reveal;
} GtkHdyViewSwitcherBarPrivate;

static GParamSpec *props[LAST_PROP];

G_DEFINE_TYPE_WITH_CODE (GtkHdyViewSwitcherBar, gtk_hdy_view_switcher_bar, GTK_TYPE_BIN,
                         G_ADD_PRIVATE (GtkHdyViewSwitcherBar))

static void
gtk_hdy_view_switcher_bar_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GtkHdyViewSwitcherBar *self = GTK_HDY_VIEW_SWITCHER_BAR (object);

  switch (prop_id) {
  case PROP_POLICY:
    g_value_set_enum (value, gtk_hdy_view_switcher_bar_get_policy (self));
    break;
  case PROP_ICON_SIZE:
    g_value_set_int (value, gtk_hdy_view_switcher_bar_get_icon_size (self));
    break;
  case PROP_STACK:
    g_value_set_object (value, gtk_hdy_view_switcher_bar_get_stack (self));
    break;
  case PROP_REVEAL:
    g_value_set_boolean (value, gtk_hdy_view_switcher_bar_get_reveal (self));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gtk_hdy_view_switcher_bar_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GtkHdyViewSwitcherBar *self = GTK_HDY_VIEW_SWITCHER_BAR (object);

  switch (prop_id) {
  case PROP_POLICY:
    gtk_hdy_view_switcher_bar_set_policy (self, g_value_get_enum (value));
    break;
  case PROP_ICON_SIZE:
    gtk_hdy_view_switcher_bar_set_icon_size (self, g_value_get_int (value));
    break;
  case PROP_STACK:
    gtk_hdy_view_switcher_bar_set_stack (self, g_value_get_object (value));
    break;
  case PROP_REVEAL:
    gtk_hdy_view_switcher_bar_set_reveal (self, g_value_get_boolean (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gtk_hdy_view_switcher_bar_class_init (GtkHdyViewSwitcherBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = gtk_hdy_view_switcher_bar_get_property;
  object_class->set_property = gtk_hdy_view_switcher_bar_set_property;

  /**
   * GtkHdyViewSwitcherBar:policy:
   *
   * The #GtkHdyViewSwitcherPolicy the #GtkHdyViewSwitcher should use to determine
   * which mode to use.
   *
   * Since: 0.0.10
   */
  props[PROP_POLICY] =
    g_param_spec_enum ("policy",
                       _("Policy"),
                       _("The policy to determine the mode to use"),
                       GTK_TYPE_HDY_VIEW_SWITCHER_POLICY, GTK_HDY_VIEW_SWITCHER_POLICY_NARROW,
                       G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkHdyViewSwitcherBar:icon-size:
   *
   * Use the "icon-size" property to hint the icons to use, you almost certainly
   * want to leave this as %GTK_ICON_SIZE_BUTTON.
   *
   * Since: 0.0.10
   */
  props[PROP_ICON_SIZE] =
    g_param_spec_int ("icon-size",
                      _("Icon Size"),
                      _("Symbolic size to use for named icon"),
                      0, G_MAXINT, GTK_ICON_SIZE_BUTTON,
                      G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkHdyViewSwitcherBar:stack:
   *
   * The #GtkStack the #GtkHdyViewSwitcher controls.
   *
   * Since: 0.0.10
   */
  props[PROP_STACK] =
    g_param_spec_object ("stack",
                         _("Stack"),
                         _("Stack"),
                         GTK_TYPE_STACK,
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * GtkHdyViewSwitcherBar:reveal:
   *
   * Whether the bar should be revealed or hidden.
   *
   * Since: 0.0.10
   */
  props[PROP_REVEAL] =
    g_param_spec_boolean ("reveal",
                         _("Reveal"),
                         _("Whether the view switcher is revealed"),
                         FALSE,
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, LAST_PROP, props);

  gtk_widget_class_set_css_name (widget_class, "hdyviewswitcherbar");

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gtk/libgtk/ui/hdy-view-switcher-bar.ui");
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherBar, action_bar);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherBar, view_switcher);
}

static void
gtk_hdy_view_switcher_bar_init (GtkHdyViewSwitcherBar *self)
{
  GtkHdyViewSwitcherBarPrivate *priv;
  g_autoptr (GtkCssProvider) provider = gtk_css_provider_new ();
  GtkWidget *box;

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  /* This must be initialized before the template so the embedded view switcher
   * can pick up the correct default value.
   */
  priv->policy = GTK_HDY_VIEW_SWITCHER_POLICY_NARROW;
  priv->icon_size = GTK_ICON_SIZE_BUTTON;

  g_type_ensure (GTK_TYPE_HDY_VIEW_SWITCHER);

  gtk_widget_init_template (GTK_WIDGET (self));

  priv->revealer = GTK_REVEALER (gtk_bin_get_child (GTK_BIN (priv->action_bar)));
  g_object_bind_property (self, "reveal", priv->revealer, "reveal-child", G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);
  gtk_revealer_set_transition_type (priv->revealer, GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);

  box = gtk_bin_get_child (GTK_BIN (priv->revealer));

  gtk_css_provider_load_from_resource (provider, "/org/gtk/libgtk/style/hdy-view-switcher-bar-box.css");
  gtk_style_context_add_provider (gtk_widget_get_style_context (box),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
}

/**
 * gtk_hdy_view_switcher_bar_new:
 *
 * Creates a new #GtkHdyViewSwitcherBar widget.
 *
 * Returns: a new #GtkHdyViewSwitcherBar
 *
 * Since: 0.0.10
 */
GtkHdyViewSwitcherBar *
gtk_hdy_view_switcher_bar_new (void)
{
  return g_object_new (GTK_TYPE_HDY_VIEW_SWITCHER_BAR, NULL);
}

/**
 * gtk_hdy_view_switcher_bar_get_policy:
 * @self: a #GtkHdyViewSwitcherBar
 *
 * Gets the policy of @self.
 *
 * Returns: the policy of @self
 *
 * Since: 0.0.10
 */
GtkHdyViewSwitcherPolicy
gtk_hdy_view_switcher_bar_get_policy (GtkHdyViewSwitcherBar *self)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self), GTK_HDY_VIEW_SWITCHER_POLICY_NARROW);

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  return priv->policy;
}

/**
 * gtk_hdy_view_switcher_bar_set_policy:
 * @self: a #GtkHdyViewSwitcherBar
 * @policy: the new policy
 *
 * Sets the policy of @self.
 *
 * Since: 0.0.10
 */
void
gtk_hdy_view_switcher_bar_set_policy (GtkHdyViewSwitcherBar    *self,
                                  GtkHdyViewSwitcherPolicy  policy)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self));

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  if (priv->policy == policy)
    return;

  priv->policy = policy;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_POLICY]);

  gtk_widget_queue_resize (GTK_WIDGET (self));
}

/**
 * gtk_hdy_view_switcher_bar_get_icon_size:
 * @self: a #GtkHdyViewSwitcherBar
 *
 * Get the icon size of the images used in the #GtkHdyViewSwitcher.
 *
 * Returns: the icon size of the images
 *
 * Since: 0.0.10
 */
GtkIconSize
gtk_hdy_view_switcher_bar_get_icon_size (GtkHdyViewSwitcherBar *self)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self), GTK_ICON_SIZE_BUTTON);

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  return priv->icon_size;
}

/**
 * gtk_hdy_view_switcher_bar_set_icon_size:
 * @self: a #GtkHdyViewSwitcherBar
 * @icon_size: the new icon size
 *
 * Change the icon size hint for the icons in a #GtkHdyViewSwitcher.
 *
 * Since: 0.0.10
 */
void
gtk_hdy_view_switcher_bar_set_icon_size (GtkHdyViewSwitcherBar *self,
                                     GtkIconSize         icon_size)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self));

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  if (priv->icon_size == icon_size)
    return;

  priv->icon_size = icon_size;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_ICON_SIZE]);
}

/**
 * gtk_hdy_view_switcher_bar_get_stack:
 * @self: a #GtkHdyViewSwitcherBar
 *
 * Get the #GtkStack being controlled by the #GtkHdyViewSwitcher.
 *
 * Returns: (nullable) (transfer none): the #GtkStack, or %NULL if none has been set
 *
 * Since: 0.0.10
 */
GtkStack *
gtk_hdy_view_switcher_bar_get_stack (GtkHdyViewSwitcherBar *self)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self), NULL);

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  return gtk_hdy_view_switcher_get_stack (priv->view_switcher);
}

/**
 * gtk_hdy_view_switcher_bar_set_stack:
 * @self: a #GtkHdyViewSwitcherBar
 * @stack: (nullable): a #GtkStack
 *
 * Sets the #GtkStack to control.
 *
 * Since: 0.0.10
 */
void
gtk_hdy_view_switcher_bar_set_stack (GtkHdyViewSwitcherBar *self,
                                 GtkStack           *stack)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self));
  g_return_if_fail (stack == NULL || GTK_IS_STACK (stack));

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  if (gtk_hdy_view_switcher_get_stack (priv->view_switcher) == stack)
    return;

  gtk_hdy_view_switcher_set_stack (priv->view_switcher, stack);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_STACK]);
}

/**
 * gtk_hdy_view_switcher_bar_get_reveal:
 * @self: a #GtkHdyViewSwitcherBar
 *
 * Gets whether @self should be revealed or not.
 *
 * Returns: %TRUE if @self is revealed, %FALSE if not.
 *
 * Since: 0.0.10
 */
gboolean
gtk_hdy_view_switcher_bar_get_reveal (GtkHdyViewSwitcherBar *self)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self), FALSE);

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  return priv->reveal;
}

/**
 * gtk_hdy_view_switcher_bar_set_reveal:
 * @self: a #GtkHdyViewSwitcherBar
 * @reveal: %TRUE to reveal @self
 *
 * Sets whether @self should be revealed or not.
 *
 * Since: 0.0.10
 */
void
gtk_hdy_view_switcher_bar_set_reveal (GtkHdyViewSwitcherBar *self,
                                  gboolean            reveal)
{
  GtkHdyViewSwitcherBarPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BAR (self));

  priv = gtk_hdy_view_switcher_bar_get_instance_private (self);

  reveal = !!reveal;

  if (priv->reveal == reveal)
    return;

  priv->reveal = reveal;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_REVEAL]);
}
