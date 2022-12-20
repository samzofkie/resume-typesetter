/* Simple example to use pangocairo to render rotated text */

#include <math.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>

static void
draw_text (cairo_t *cr)
{
  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *desc = 
    pango_font_description_from_string("Times new roman,Sans");

  const char *name = "Sam Zofkie!";
  pango_layout_set_text(layout, name, 12); 

  int font_size = 36;
  int device_dpi = 72;
  double scale = 1;
  pango_font_description_set_absolute_size(
      desc,
      font_size * device_dpi * PANGO_SCALE
      / (72.0 / scale) );
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  cairo_set_source_rgb(cr, 0, 0, 0);
  int w, h;
  pango_layout_get_size(layout, &w, &h);
  printf("%d %d\n", w, h);
  cairo_move_to(cr, 1, 1);
  pango_cairo_show_layout(cr, layout);

  g_object_unref(layout);

}

int main (int argc, char **argv)
{
  cairo_t *cr;
  
  cairo_surface_t *surface = 
    cairo_pdf_surface_create ("resume.pdf", 72 * 8.5, 72 * 11);
   
  cr = cairo_create (surface);

  cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
  cairo_paint (cr);
  
  draw_text (cr);
  
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
 
  return 0;
}
