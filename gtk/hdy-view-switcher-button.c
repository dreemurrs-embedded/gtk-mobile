/*
 * Copyright (C) 2019 Zander Brown <zbrown@gnome.org>
 * Copyright (C) 2019 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include "gtkbox.h"
#include "gtkcssprovider.h"
#include "gtkimage.h"
#include "gtklabel.h"
#include "gtkorientable.h"
#include "gtkstack.h"
#include "gtkstylecontext.h"
#include "gtkstyleprovider.h"
#include "hdy-style-private.h"
#include "hdy-view-switcher-button-private.h"


/**
 * PRIVATE:hdy-view-switcher-button
 * @short_description: Button used in #HdyViewSwitcher
 * @title: GtkHdyViewSwitcherButton
 * @See_also: #HdyViewSwitcher
 * @stability: Private
 *
 * #GtkHdyViewSwitcherButton represents an application's view. It is design to be
 * used exclusively internally by #HdyViewSwitcher.
 *
 * Since: 0.0.10
 */

enum {
  PROP_0,
  PROP_ICON_SIZE,
  PROP_ICON_NAME,
  PROP_NEEDS_ATTENTION,

  /* Overridden properties */
  PROP_LABEL,
  PROP_ORIENTATION,

  LAST_PROP = PROP_NEEDS_ATTENTION + 1,
};

typedef struct {
  GtkBox *horizontal_box;
  GtkImage *horizontal_image;
  GtkLabel *horizontal_label_active;
  GtkLabel *horizontal_label_inactive;
  GtkStack *horizontal_label_stack;
  GtkStack *stack;
  GtkBox *vertical_box;
  GtkImage *vertical_image;
  GtkLabel *vertical_label_active;
  GtkLabel *vertical_label_inactive;
  GtkStack *vertical_label_stack;

  gchar *icon_name;
  GtkIconSize icon_size;
  gchar *label;
  GtkOrientation orientation;
} GtkHdyViewSwitcherButtonPrivate;

static GParamSpec *props[LAST_PROP];

G_DEFINE_TYPE_WITH_CODE (GtkHdyViewSwitcherButton, gtk_hdy_view_switcher_button, GTK_TYPE_RADIO_BUTTON,
                         G_ADD_PRIVATE (GtkHdyViewSwitcherButton)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_ORIENTABLE, NULL))

static void
on_active_changed (GtkHdyViewSwitcherButton *self)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self));

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self))) {
    gtk_stack_set_visible_child (priv->horizontal_label_stack, GTK_WIDGET (priv->horizontal_label_active));
    gtk_stack_set_visible_child (priv->vertical_label_stack, GTK_WIDGET (priv->vertical_label_active));
  } else {
    gtk_stack_set_visible_child (priv->horizontal_label_stack, GTK_WIDGET (priv->horizontal_label_inactive));
    gtk_stack_set_visible_child (priv->vertical_label_stack, GTK_WIDGET (priv->vertical_label_inactive));
  }
}

static GtkOrientation
get_orientation (GtkHdyViewSwitcherButton *self)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self), GTK_ORIENTATION_HORIZONTAL);

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  return priv->orientation;
}

static void
set_orientation (GtkHdyViewSwitcherButton *self,
                 GtkOrientation         orientation)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self));

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  if (priv->orientation == orientation)
    return;

  priv->orientation = orientation;

  gtk_stack_set_visible_child (priv->stack,
                               GTK_WIDGET (priv->orientation == GTK_ORIENTATION_VERTICAL ?
                                             priv->vertical_box :
                                             priv->horizontal_box));
}

static void
gtk_hdy_view_switcher_button_get_property (GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
  GtkHdyViewSwitcherButton *self = GTK_HDY_VIEW_SWITCHER_BUTTON (object);

  switch (prop_id) {
  case PROP_ICON_NAME:
    g_value_set_string (value, gtk_hdy_view_switcher_button_get_icon_name (self));
    break;
  case PROP_ICON_SIZE:
    g_value_set_int (value, gtk_hdy_view_switcher_button_get_icon_size (self));
    break;
  case PROP_NEEDS_ATTENTION:
    g_value_set_boolean (value, gtk_hdy_view_switcher_button_get_needs_attention (self));
    break;
  case PROP_LABEL:
    g_value_set_string (value, gtk_hdy_view_switcher_button_get_label (self));
    break;
  case PROP_ORIENTATION:
    g_value_set_enum (value, get_orientation (self));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gtk_hdy_view_switcher_button_set_property (GObject      *object,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
  GtkHdyViewSwitcherButton *self = GTK_HDY_VIEW_SWITCHER_BUTTON (object);

  switch (prop_id) {
  case PROP_ICON_NAME:
    gtk_hdy_view_switcher_button_set_icon_name (self, g_value_get_string (value));
    break;
  case PROP_ICON_SIZE:
    gtk_hdy_view_switcher_button_set_icon_size (self, g_value_get_int (value));
    break;
  case PROP_NEEDS_ATTENTION:
    gtk_hdy_view_switcher_button_set_needs_attention (self, g_value_get_boolean (value));
    break;
  case PROP_LABEL:
    gtk_hdy_view_switcher_button_set_label (self, g_value_get_string (value));
    break;
  case PROP_ORIENTATION:
    set_orientation (self, g_value_get_enum (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gtk_hdy_view_switcher_button_finalize (GObject *object)
{
  GtkHdyViewSwitcherButton *self = GTK_HDY_VIEW_SWITCHER_BUTTON (object);
  GtkHdyViewSwitcherButtonPrivate *priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  g_free (priv->icon_name);
  g_free (priv->label);

  G_OBJECT_CLASS (gtk_hdy_view_switcher_button_parent_class)->finalize (object);
}

static void
gtk_hdy_view_switcher_button_class_init (GtkHdyViewSwitcherButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = gtk_hdy_view_switcher_button_get_property;
  object_class->set_property = gtk_hdy_view_switcher_button_set_property;
  object_class->finalize = gtk_hdy_view_switcher_button_finalize;

  g_object_class_override_property (object_class,
                                    PROP_LABEL,
                                    "label");

  g_object_class_override_property (object_class,
                                    PROP_ORIENTATION,
                                    "orientation");

  /**
   * GtkHdyViewSwitcherButton:icon-name:
   *
   * The icon name representing the view, or %NULL for no icon.
   *
   * Since: 0.0.10
   */
  props[PROP_ICON_NAME] =
    g_param_spec_string ("icon-name",
                         _("Icon Name"),
                         _("Icon name for image"),
                         "text-x-generic-symbolic",
                         G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE);

  /**
   * GtkHdyViewSwitcherButton:icon-size:
   *
   * The icon size.
   *
   * Since: 0.0.10
   */
  props[PROP_ICON_SIZE] =
    g_param_spec_int ("icon-size",
                      _("Icon Size"),
                      _("Symbolic size to use for named icon"),
                      0, G_MAXINT, GTK_ICON_SIZE_BUTTON,
                      G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE);

  /**
   * GtkHdyViewSwitcherButton:needs-attention:
   *
   * Sets a flag specifying whether the view requires the user attention. This
   * is used by the GtkHdyViewSwitcher to change the appearance of the
   * corresponding button when a view needs attention and it is not the current
   * one.
   *
   * Since: 0.0.10
   */
  props[PROP_NEEDS_ATTENTION] =
  g_param_spec_boolean ("needs-attention",
                        _("Needs attention"),
                        _("Hint the view needs attention"),
                        FALSE,
                        G_PARAM_EXPLICIT_NOTIFY | G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, LAST_PROP, props);

  /* We probably should set the class's CSS name to "hdyviewswitcherbutton"
   * here, but it doesn't work because GtkHdyCheckButton hardcodes it to "button"
   * on instanciation, and the functions required to override it are private.
   * In the meantime, we can use the "hdyviewswitcher > button" CSS selector as
   * a fairly safe fallback.
   */

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/org/gtk/libgtk/ui/hdy-view-switcher-button.ui");
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, horizontal_box);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, horizontal_image);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, horizontal_label_active);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, horizontal_label_inactive);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, horizontal_label_stack);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, stack);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, vertical_box);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, vertical_image);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, vertical_label_active);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, vertical_label_inactive);
  gtk_widget_class_bind_template_child_private (widget_class, GtkHdyViewSwitcherButton, vertical_label_stack);
  gtk_widget_class_bind_template_callback (widget_class, on_active_changed);
}

static void
gtk_hdy_view_switcher_button_init (GtkHdyViewSwitcherButton *self)
{
  GtkHdyViewSwitcherButtonPrivate *priv;
  GtkCssProvider *provider = gtk_css_provider_new ();

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);
  priv->icon_size = GTK_ICON_SIZE_BUTTON;

  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_css_provider_load_from_resource (provider, "/org/gtk/libgtk/style/hdy-view-switcher-button.css");
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (self)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->horizontal_box)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->horizontal_image)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->horizontal_label_active)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->horizontal_label_inactive)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->horizontal_label_stack)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->vertical_box)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->vertical_image)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->vertical_label_active)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->vertical_label_inactive)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);
  gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (priv->vertical_label_stack)),
                                  GTK_STYLE_PROVIDER (provider),
                                  GTK_HDY_STYLE_PROVIDER_PRIORITY);

  gtk_stack_set_visible_child (GTK_STACK (priv->stack), GTK_WIDGET (priv->horizontal_box));

  gtk_widget_set_focus_on_click (GTK_WIDGET (self), FALSE);
  /* Make the button look like a regular button and not a radio button. */
  gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (self), FALSE);

  on_active_changed (self);

  g_object_unref (provider);
}

/**
 * gtk_hdy_view_switcher_button_new:
 *
 * Creates a new #GtkHdyViewSwitcherButton widget.
 *
 * Returns: a new #GtkHdyViewSwitcherButton
 *
 * Since: 0.0.10
 */
GtkHdyViewSwitcherButton *
gtk_hdy_view_switcher_button_new (void)
{
  return g_object_new (GTK_TYPE_HDY_VIEW_SWITCHER_BUTTON, NULL);
}

/**
 * gtk_hdy_view_switcher_button_get_icon_name:
 * @self: a #GtkHdyViewSwitcherButton
 *
 * Gets the icon name representing the view, or %NULL is no icon is set.
 *
 * Returns: (transfer none) (nullable): the icon name, or %NULL
 *
 * Since: 0.0.10
 **/
const gchar *
gtk_hdy_view_switcher_button_get_icon_name (GtkHdyViewSwitcherButton *self)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self), NULL);

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  return priv->icon_name;
}

/**
 * gtk_hdy_view_switcher_button_set_icon_name:
 * @self: a #GtkHdyViewSwitcherButton
 * @icon_name: (nullable): an icon name or %NULL
 *
 * Sets the icon name representing the view, or %NULL to disable the icon.
 *
 * Since: 0.0.10
 **/
void
gtk_hdy_view_switcher_button_set_icon_name (GtkHdyViewSwitcherButton *self,
                                        const gchar           *icon_name)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self));

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  if (!g_strcmp0 (priv->icon_name, icon_name))
    return;

  g_free (priv->icon_name);
  priv->icon_name = g_strdup (icon_name);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_ICON_NAME]);
}

/**
 * gtk_hdy_view_switcher_button_get_icon_size:
 * @self: a #GtkHdyViewSwitcherButton
 *
 * Gets the icon size used by @self.
 *
 * Returns: the icon size used by @self
 *
 * Since: 0.0.10
 **/
GtkIconSize
gtk_hdy_view_switcher_button_get_icon_size (GtkHdyViewSwitcherButton *self)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self), GTK_ICON_SIZE_INVALID);

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  return priv->icon_size;
}

/**
 * gtk_hdy_view_switcher_button_set_icon_size:
 * @self: a #GtkHdyViewSwitcherButton
 * @icon_size: the new icon size
 *
 * Sets the icon size used by @self.
 *
 * Since: 0.0.10
 */
void
gtk_hdy_view_switcher_button_set_icon_size (GtkHdyViewSwitcherButton *self,
                                        GtkIconSize            icon_size)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self));

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  if (priv->icon_size == icon_size)
    return;

  priv->icon_size = icon_size;

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_ICON_SIZE]);
}

/**
 * gtk_hdy_view_switcher_button_get_needs_attention:
 * @self: a #GtkHdyViewSwitcherButton
 *
 * Gets whether the view represented by @self requires the user attention.
 *
 * Returns: %TRUE if the view represented by @self requires the user attention, %FALSE otherwise
 *
 * Since: 0.0.10
 **/
gboolean
gtk_hdy_view_switcher_button_get_needs_attention (GtkHdyViewSwitcherButton *self)
{
  GtkStyleContext *context;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self), FALSE);

  context = gtk_widget_get_style_context (GTK_WIDGET (self));

  return gtk_style_context_has_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION);
}

/**
 * gtk_hdy_view_switcher_button_set_needs_attention:
 * @self: a #GtkHdyViewSwitcherButton
 * @needs_attention: the new icon size
 *
 * Sets whether the view represented by @self requires the user attention.
 *
 * Since: 0.0.10
 */
void
gtk_hdy_view_switcher_button_set_needs_attention (GtkHdyViewSwitcherButton *self,
                                              gboolean               needs_attention)
{
  GtkStyleContext *context;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self));

  needs_attention = !!needs_attention;

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  if (gtk_style_context_has_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION) == needs_attention)
    return;

  if (needs_attention)
    gtk_style_context_add_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION);
  else
    gtk_style_context_remove_class (context, GTK_STYLE_CLASS_NEEDS_ATTENTION);

  g_object_notify_by_pspec (G_OBJECT (self), props[PROP_NEEDS_ATTENTION]);
}

/**
 * gtk_hdy_view_switcher_button_get_label:
 * @self: a #GtkHdyViewSwitcherButton
 *
 * Gets the label representing the view.
 *
 * Returns: (transfer none) (nullable): the label, or %NULL
 *
 * Since: 0.0.10
 **/
const gchar *
gtk_hdy_view_switcher_button_get_label (GtkHdyViewSwitcherButton *self)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_val_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self), NULL);

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  return priv->label;
}

/**
 * gtk_hdy_view_switcher_button_set_label:
 * @self: a #GtkHdyViewSwitcherButton
 * @label: (nullable): a label or %NULL
 *
 * Sets the label representing the view.
 *
 * Since: 0.0.10
 **/
void
gtk_hdy_view_switcher_button_set_label (GtkHdyViewSwitcherButton *self,
                                    const gchar           *label)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self));

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  if (!g_strcmp0 (priv->label, label))
    return;

  g_free (priv->label);
  priv->label = g_strdup (label);

  g_object_notify (G_OBJECT (self), "label");
}

/**
 * gtk_hdy_view_switcher_button_set_narrow_ellipsize:
 * @self: a #GtkHdyViewSwitcherButton
 * @mode: a #PangoEllipsizeMode
 *
 * Set the mode used to ellipsize the text in narrow mode if there is not
 * enough space to render the entire string.
 *
 * Since: 0.0.10
 **/
void
gtk_hdy_view_switcher_button_set_narrow_ellipsize (GtkHdyViewSwitcherButton *self,
                                               PangoEllipsizeMode     mode)
{
  GtkHdyViewSwitcherButtonPrivate *priv;

  g_return_if_fail (GTK_IS_HDY_VIEW_SWITCHER_BUTTON (self));
  g_return_if_fail (mode >= PANGO_ELLIPSIZE_NONE && mode <= PANGO_ELLIPSIZE_END);

  priv = gtk_hdy_view_switcher_button_get_instance_private (self);

  gtk_label_set_ellipsize (priv->vertical_label_active, mode);
  gtk_label_set_ellipsize (priv->vertical_label_inactive, mode);
}

/**
 * gtk_hdy_view_switcher_button_get_size:
 * @self: a #GtkHdyViewSwitcherButton
 * @h_min_width: (out) (nullable): the minimum width when horizontal
 * @h_nat_width: (out) (nullable): the natural width when horizontal
 * @v_min_width: (out) (nullable): the minimum width when vertical
 * @v_nat_width: (out) (nullable): the natural width when vertical
 *
 * Measure the size requests in both horizontal and vertical modes.
 *
 * Since: 0.0.10
 */
void
gtk_hdy_view_switcher_button_get_size (GtkHdyViewSwitcherButton *self,
                                   gint                  *h_min_width,
                                   gint                  *h_nat_width,
                                   gint                  *v_min_width,
                                   gint                  *v_nat_width)
{
  GtkHdyViewSwitcherButtonPrivate *priv = gtk_hdy_view_switcher_button_get_instance_private (self);
  GtkStyleContext *context;
  GtkStateFlags state;
  GtkBorder border;

  /* gtk_widget_get_preferred_width() doesn't accept both its out parameters to
   * be NULL, so we must have guards.
   */
  if (h_min_width != NULL || h_nat_width != NULL)
    gtk_widget_get_preferred_width (GTK_WIDGET (priv->horizontal_box), h_min_width, h_nat_width);
  if (v_min_width != NULL || v_nat_width != NULL)
    gtk_widget_get_preferred_width (GTK_WIDGET (priv->vertical_box), v_min_width, v_nat_width);

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  state = gtk_style_context_get_state (context);
  gtk_style_context_get_border (context, state, &border);
  if (h_min_width != NULL)
    *h_min_width += border.left + border.right;
  if (h_nat_width != NULL)
    *h_nat_width += border.left + border.right;
  if (v_min_width != NULL)
    *v_min_width += border.left + border.right;
  if (v_nat_width != NULL)
    *v_nat_width += border.left + border.right;
}
