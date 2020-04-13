// wdkg 2009-2010
//XXX allow gui to supply bitmap
//XXX use point specific get_bbox

#include <stdexcept>
#include <GL/gl.h>
#include "ew/Bbox3.h"
#include "ew/DataflowForm3.h"
#include "ew/DataflowNetwork.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/ErrorLogic.h"
#include "ew/ErrorRuntime.h"
#include "ew/Form3.h"
#include "ew/View3Item.h"
#include "ew/View3Landmarks.h"
#include "ew/View3Widget.h"

namespace {
  const char ClassName[] = "ew::View3Landmarks";
  const char InstanceName[] = "landmarks";
  const GLsizei MarkersN[] = {9, 7, 5};
  const GLfloat MarkersO[] = {4.0, 3.0, 2.0};
  const GLubyte MarkersD[][18] = {
    {
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00,
      0xff, 0x80,
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00,
      0x08, 0x00
    },
    {
      0x38,
      0x44,
      0x82,
      0x82,
      0x82,
      0x44,
      0x38,
    },
    {
      0x70,
      0xf8,
      0xf8,
      0xf8,
      0x70
    }
  };

  template<typename T>
  inline int
  VectorSize(const std::vector<T> &v)
  {
    return static_cast<int>(v.size());
  }
}

/// The names of the #symbol_t
/// choices.

const char *const ew::View3Landmarks::symbol_names[] = {
  "cross",
  "circle",
  "dot",
  0
};

/// This creates an item.
/// @param view_i
///   The view widget the item should belong to.
/// @param type_i
///   The initializer for ew::View3Item::type.

ew::View3Landmarks::View3Landmarks(ew::View3Widget *i_view, int i_type) :
 ew::View3Item(i_view, i_type, InstanceName),
 dbg(ClassName, ew::View3Item::dbg),
 form(0),
 form_checked_version(0),
 symbol(SYMBOL_CROSS),
 symbol_size(8),
 cross_symbol_data(0),
 dlist(0),
 highlight_ps(-1),
 highlight_i(-1)
{
  color[0] = color[2] = 0;
  color[1] = 255;
  recreate_cross_symbol();
}

ew::View3Landmarks::~View3Landmarks()
{
  if (form != 0) {
    form->ew::DataflowNode::decr_ref_count();
  }
//XXX need to ensure ctxt active
  if (dlist) {
    glDeleteLists(dlist, 1);
  }
  if(cross_symbol_data != 0)
      delete [] cross_symbol_data;
}

/// @param frm
///   A pointer to the new form node to display, or 0.

void
ew::View3Landmarks::set_form(const ew::DataflowForm3 *frm)
{
  if (frm == form) {
    return;
  }
  if (frm != 0) {
    if (frm->network != network) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    frm->ew::DataflowNode::incr_ref_count();
  }
  if (form != 0) {
    form->ew::DataflowNode::decr_ref_count();
  }
  form = frm;
  form_checked_version = 0;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

const ew::Bbox3 *
ew::View3Landmarks::get_bbox()
{
  if (form != 0) {
    return form->get_bbox();
  } else {
    return &ew::Bbox3::empty_box;
  }
}

/// @param[in] up
/// true to increase size, false to decrease
void
ew::View3Landmarks::change_size(bool up)
{
  symbol_size += up?8:-8;
  // printf("Size %f\n", size);

  if(symbol_size<=8) symbol_size=8;

  recreate_cross_symbol();

  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

/// @param[in] rgb
///   This points to an array of size 3 containing the new colour for the
///   landmark symbols.

void
ew::View3Landmarks::set_color(const unsigned char *rgb)
{
  if (color[0] == rgb[0] &&
   color[1] == rgb[1] &&
   color[2] == rgb[2]) {
    return;
  }
  color[0] = rgb[0];
  color[1] = rgb[1];
  color[2] = rgb[2];
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

/// @param sym
///   The new landmark symbol.

void
ew::View3Landmarks::set_symbol(int sym)
{
  if (sym == symbol) {
    return;
  }
  if (sym < 0 || sym >= N_SYMBOLS) {
    throw ew::ErrorLogic(__FILE__, __LINE__);
  }
  symbol = sym;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

//XXX finer grain versions
bool
ew::View3Landmarks::check_dataflow()
{
  if (form != 0) {
    unsigned long v = form->ew::DataflowNode::get_version();
    if (form_checked_version < v) {
      form_checked_version = v;
      ew::View3Item::prepared = false;
      return true;
    }
  }
  return false;
}

void
ew::View3Landmarks::prepare()
{
    #if 0
  if (dlist == 0) {
    dlist = glGenLists(1);
    if (dlist == 0) {
      throw ew::ErrorRuntime(
       "A program limitation has been exceeded"
       " (the number of OpenGL display lists).");
    }
  }
  if (form != 0 && form->ew::DataflowNode::get_valid()) {
    glNewList(dlist, GL_COMPILE);
    glShadeModel(GL_FLAT);
    const ew::Form3 *data = form->get_data();
    GLfloat c[3];
    c[0] = color[0] / 255.0;
    c[1] = color[1] / 255.0;
    c[2] = color[2] / 255.0;
    int sy = symbol;
    if (sy < 0 || sy > N_SYMBOLS) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    glColor3fv(c);
    unsigned int u = 0;
    for (int ps = 0; ps < VectorSize(data->pointsets); ps += 1) {
      const ew::Form3PointSet *fps = &data->pointsets[ps];
      if (fps->type == ew::Form3::TYPE_LANDMARK ||
       fps->type == ew::Form3::TYPE_SEMI_LANDMARK) {
        for (int i = 0; i < VectorSize(fps->locations); i += 3) {
          if (ps == highlight_ps && i == highlight_i) {
            const unsigned char *hc =
             ew::View3Item::view->get_highlight_color();
            GLfloat ch[3];
            ch[0] = hc[0] / 255.0;
            ch[1] = hc[1] / 255.0;
            ch[2] = hc[2] / 255.0;
            glColor3fv(ch);
          }
          glLoadName(u << 2);
          glRasterPos3dv(&fps->locations[i]);
          //printf( " %f %f %f\n", fps->locations[i], fps->locations[i+1], fps->locations[i+2] );
          //NOPE! glVertex3dv( &fps->locations[i] ); // doesn't set the raster pos, which is used subsequently by glBitmap(); also don't forget glBegin/glEnd!
          glBitmap(MarkersN[sy], MarkersN[sy], MarkersO[sy], MarkersO[sy], 0.0,
           0.0, MarkersD[sy]);

//           glColor3f( 0.9f, 0.9f, 0.9f );
//           glPointSize( 8.0f );
//           glBegin( GL_POINTS );
//           glVertex3dv( &fps->locations[i] );
//           glEnd();

          u += 1;
          if (ps == highlight_ps && i == highlight_i) {
            glColor3fv(c);
          }
        }
      }
    }
    glEndList();
  } else {
    glNewList(dlist, GL_COMPILE);
    glEndList();
  }
  #endif
  ew::View3Item::prepared = true;
}

//void
//ew::View3Landmarks::render()
//{
//  glCallList(dlist);
//}
void
ew::View3Landmarks::recreate_cross_symbol()
{
    if(cross_symbol_data != 0)
        delete [] cross_symbol_data;
    int N = symbol_size+1;
    int bytes = (N/8) + (N%8>0?1:0);
    cross_symbol_data = new GLubyte[N*bytes];
    for(int i=0; i<N; ++i) //for every row
        for(int b=0; b<bytes; ++b)
            cross_symbol_data[(i*bytes)+b] = (b == (bytes/2)-1)?((bytes%2)?0x1:0x08):0x00;
    //mid line
    for(int b=0; b<bytes; ++b)
        cross_symbol_data[((symbol_size/2)*bytes)+b] = (b == bytes-1)?0x08:0xff;
}

void getUpRightVector(float *up, float *right, float* mv_lastrow)
{
    float modelview[16];

    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    right[0] = modelview[0];
    right[1] = modelview[4];
    right[2] = modelview[8];

    up[0] = modelview[1];
    up[1] = modelview[5];
    up[2] = modelview[9];

    mv_lastrow[0] = modelview[3];
    mv_lastrow[1] = modelview[7];
    mv_lastrow[2] = modelview[11];
    mv_lastrow[3] = modelview[15];
}

void drawLmk(const GLdouble *pos, const GLubyte* cross_data,
             int size, int symbol)
{
    switch(symbol)
    {
    case 0: //cross
//        d /=  mv_lastrow[0]*pos[0] + mv_lastrow[1]*pos[1] +
//              mv_lastrow[2]*pos[2] + mv_lastrow[3];
//        glBegin(GL_LINES);
//          glVertex3d(pos[0]-right[0]*d,pos[1]-right[1]*d,pos[2]-right[2]*d);
//          glVertex3d(pos[0]+right[0]*d,pos[1]+right[1]*d,pos[2]+right[2]*d);
//          glVertex3d(pos[0]-up[0]*d,pos[1]-up[1]*d,pos[2]-up[2]*d);
//          glVertex3d(pos[0]+up[0]*d,pos[1]+up[1]*d,pos[2]+up[2]*d);
//        glEnd();
//        glPixelZoom(d,d);
        glBitmap(size+1, size+1, ((GLfloat)size/2.), ((GLfloat)size/2.), 0.0,
                   0.0, cross_data);
    break;
    case 1: //square
        glPointSize(size/2.);
        glDisable(GL_POINT_SMOOTH);
        glBegin( GL_POINTS );
         glVertex3dv(pos);
        glEnd();
    break;
    case 2: //dot
        glPointSize(size/2.);
        glEnable(GL_POINT_SMOOTH);
        glBegin( GL_POINTS );
         glVertex3dv(pos);
        glEnd();
    break;
    }
}

void
ew::View3Landmarks::render()
{
  if (form != 0 && form->ew::DataflowNode::get_valid()) {
    glShadeModel(GL_FLAT);
    const ew::Form3 *data = form->get_data();
    GLfloat c[3];
    c[0] = color[0] / 255.0;
    c[1] = color[1] / 255.0;
    c[2] = color[2] / 255.0;
    int sy = symbol;
    if (sy < 0 || sy > N_SYMBOLS) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    glColor3fv(c);
    unsigned int u = 0;
    for (int ps = 0; ps < VectorSize(data->pointsets); ps += 1)
    {
      const ew::Form3PointSet *fps = &data->pointsets[ps];
      if (fps->type == ew::Form3::TYPE_LANDMARK ||
       fps->type == ew::Form3::TYPE_SEMI_LANDMARK)
      {
        for (int i = 0; i < VectorSize(fps->locations); i += 3)
        {
          if (ps == highlight_ps && i == highlight_i) {
            const unsigned char *hc =
             ew::View3Item::view->get_highlight_color();
            GLfloat ch[3];
            ch[0] = hc[0] / 255.0;
            ch[1] = hc[1] / 255.0;
            ch[2] = hc[2] / 255.0;
            glColor3fv(ch);
          }
          glLoadName(u << 2);
          glRasterPos3dv(&fps->locations[i]);
//          glBitmap(MarkersN[sy], MarkersN[sy], MarkersO[sy], MarkersO[sy], 0.0,
//           0.0, MarkersD[sy]);
          drawLmk(&fps->locations[i],cross_symbol_data,symbol_size,sy);
          u += 1;
          if (ps == highlight_ps && i == highlight_i)
            glColor3fv(c);
        }
      }
    }
  }
}

//void
//ew::View3Landmarks::render()
//{
//  if (form != 0 && form->ew::DataflowNode::get_valid()) {
//     glShadeModel(GL_FLAT);
//    const ew::Form3 *data = form->get_data();
//    GLfloat c[3];
//    c[0] = color[0] / 255.0;
//    c[1] = color[1] / 255.0;
//    c[2] = color[2] / 255.0;
//    int sy = symbol;
//    if (sy < 0 || sy > N_SYMBOLS) {
//      throw ew::ErrorLogic(__FILE__, __LINE__);
//    }
//    glColor3fv(c);
//    unsigned int u = 0;
//    for (int ps = 0; ps < VectorSize(data->pointsets); ps += 1) {
//      const ew::Form3PointSet *fps = &data->pointsets[ps];
//      if (fps->type == ew::Form3::TYPE_LANDMARK ||
//       fps->type == ew::Form3::TYPE_SEMI_LANDMARK) {
//        for (int i = 0; i < VectorSize(fps->locations); i += 3) {
//          if (ps == highlight_ps && i == highlight_i) {
//            const unsigned char *hc =
//             ew::View3Item::view->get_highlight_color();
//            GLfloat ch[3];
//            ch[0] = hc[0] / 255.0;
//            ch[1] = hc[1] / 255.0;
//            ch[2] = hc[2] / 255.0;
//            glColor3fv(ch);
//          }
//          glLoadName(u << 2);
//          glRasterPos3dv(&fps->locations[i]);
//          //printf( " %f %f %f\n", fps->locations[i], fps->locations[i+1], fps->locations[i+2] );
//          //NOPE! glVertex3dv( &fps->locations[i] ); // doesn't set the raster pos, which is used subsequently by glBitmap(); also don't forget glBegin/glEnd!
//          glBitmap(MarkersN[sy], MarkersN[sy], MarkersO[sy], MarkersO[sy], 0.0,
//           0.0, MarkersD[sy]);
//
////           glColor3f( 0.9f, 0.9f, 0.9f );
////           glPointSize( 8.0f );
////           glBegin( GL_POINTS );
////           glVertex3dv( &fps->locations[i] );
////           glEnd();
//
//          u += 1;
//          if (ps == highlight_ps && i == highlight_i) {
//            glColor3fv(c);
//          }
//        }
//      }
//    }
//
//  }
//  ew::View3Item::prepared = true;
//}

void
ew::View3Landmarks::renderColorCoded()
{
  if (form != 0 && form->ew::DataflowNode::get_valid()) {
    glShadeModel(GL_FLAT);
    const ew::Form3 *data = form->get_data();
    GLfloat c[3];
    c[0] = color[0] / 255.0;
    c[1] = color[1] / 255.0;
    c[2] = color[2] / 255.0;
    int sy = symbol;
    if (sy < 0 || sy > N_SYMBOLS) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }
    //glColor3fv(c);
    GLfloat oldPointSize;
    glGetFloatv( GL_POINT_SIZE, &oldPointSize );
    printf( "oldPointSize = %f\n", oldPointSize );
    //glPointSize( 8.0f );
    //glBegin( GL_POINTS );
    unsigned int u = 0;
    for (int ps = 0; ps < VectorSize(data->pointsets); ps += 1) {
      const ew::Form3PointSet *fps = &data->pointsets[ps];
      if (fps->type == ew::Form3::TYPE_LANDMARK ||
       fps->type == ew::Form3::TYPE_SEMI_LANDMARK) {
        for (int i = 0; i < VectorSize(fps->locations); i += 3) {
//          if (ps == highlight_ps && i == highlight_i) {
//            const unsigned char *hc =
//             ew::View3Item::view->get_highlight_color();
//            GLfloat ch[3];
//            ch[0] = hc[0] / 255.0;
//            ch[1] = hc[1] / 255.0;
//            ch[2] = hc[2] / 255.0;
//            glColor3fv(ch);
//          }

          //glLoadName(u << 2);

          GLubyte colorCode[3];
          colorCode[0] = ( u & 0xff );
          colorCode[1] = ( ( u & 0xff00 ) >> 8u );
          colorCode[2] = ( ( u & 0xff0000 ) >> 16u );
          //glColor3ubv( colorCode );
          const GLfloat factor = 1.0f / 255.0f;
          //const GLfloat factor = 8.0f / 255.0f; // better for debugging, otherwise LMs appear nearly black on black BG


//          //! glColor3f( factor * colorCode[0], factor * colorCode[1], factor * colorCode[2] );
//          glColor3f( 0.9f, 0.9f, 0.9f );
//
//            printf( "renderColorCoded: [0]=%u, [1]=%u, [2]=%u; as GLuint: %u\n", colorCode[0], colorCode[1], colorCode[2], u );
//          glRasterPos3dv(&fps->locations[i]);
//          //NOPE! glVertex3dv( &fps->locations[i] ); // doesn't set the raster pos, which however is used subsequently by glBitmap()
//          glBitmap(MarkersN[sy], MarkersN[sy], MarkersO[sy], MarkersO[sy], 0.0,
//           0.0, MarkersD[sy]);


//           glColor3f( 0.9f, 0.9f, 0.9f );
//           glVertex3dv( &fps->locations[i] );


           //glColor3f( 0.9f, 0.9f, 0.9f );
           glColor3f( factor * colorCode[0], factor * colorCode[1], factor * colorCode[2] );
           glPointSize( 10.0f );
           //printf( "u = %u\n", u );
           //printf( " %f %f %f\n", fps->locations[i], fps->locations[i+1], fps->locations[i+2] );
           glBegin( GL_POINTS );
           //glVertex3dv( &fps->locations[i] );
           glVertex3d( fps->locations[i], fps->locations[i + 1], fps->locations[i + 2] );
           //glVertex3f( 0.5f, 0.5f, 0.5f );
           glEnd();


//            glMatrixMode( GL_PROJECTION );
//            glPushMatrix();
//            glLoadIdentity();
//            glMatrixMode( GL_MODELVIEW );
//            glPushMatrix();
//            glLoadIdentity();
//            glPointSize( 8.0f );
//            glColor3f( 2.0f, 0.5f, 1.0f );
//            glBegin( GL_POINTS );
//            glVertex3f( 0.5f, 0.5f, 0.5f );
//            glEnd();
//            glPopMatrix();
//            glMatrixMode( GL_PROJECTION );
//            glPopMatrix();
//            glMatrixMode( GL_MODELVIEW );


          u += 1;
//          if (ps == highlight_ps && i == highlight_i) {
//            glColor3fv(c);
//          }
        }
      }
    }
    //glEnd();
    //glPointSize( oldPointSize );
  }
  //ew::View3Item::prepared = true;
}


bool ew::View3Landmarks::pick(  const double *const mvpMatrix,
                                const float ndcX,
                                const float ndcY,
                                const float ndcToleranceX,
                                const float ndcToleranceY,
                                int &lmID ) const
{

  if (form != 0 && form->ew::DataflowNode::get_valid()) {

    const ew::Form3 *data = form->get_data();

    int sy = symbol;
    if (sy < 0 || sy > N_SYMBOLS) {
      throw ew::ErrorLogic(__FILE__, __LINE__);
    }

//    unsigned int u = 0;
    int u = 0;
    int bestU = -1;
    double currDepth = 1.0; // acutally z in window coords in [0;1] and in ndc in [-1;1], thus max dist equals 1 in both cases

    // left/right, bottom/top tolerance
    float ndcNx = ndcX - ndcToleranceX;
    float ndcPx = ndcX + ndcToleranceX;
    float ndcNy = ndcY - ndcToleranceY;
    float ndcPy = ndcY + ndcToleranceY;

    for (int ps = 0; ps < VectorSize(data->pointsets); ps += 1) {
      const ew::Form3PointSet *fps = &data->pointsets[ps];
      if (fps->type == ew::Form3::TYPE_LANDMARK ||
       fps->type == ew::Form3::TYPE_SEMI_LANDMARK) {
        for (int i = 0; i < VectorSize(fps->locations); i += 3) {

            //glVertex3d( fps->locations[i], fps->locations[i + 1], fps->locations[i + 2] );
            // transform by matrix, then check if close enough to desired pos and closer than any eligible LM visited before
            const double x = fps->locations[i];
            const double y = fps->locations[i + 1];
            const double z = fps->locations[i + 2];
            double tx = mvpMatrix[0] * x + mvpMatrix[4] * y + mvpMatrix[ 8] * z + mvpMatrix[12];
            double ty = mvpMatrix[1] * x + mvpMatrix[5] * y + mvpMatrix[ 9] * z + mvpMatrix[13];
            double tz = mvpMatrix[2] * x + mvpMatrix[6] * y + mvpMatrix[10] * z + mvpMatrix[14];
            double tw = mvpMatrix[3] * x + mvpMatrix[7] * y + mvpMatrix[11] * z + mvpMatrix[15];

            // inside view frustum?
            if (    ( -tw <= tx ) && ( tx <= tw ) &&
                    ( -tw <= ty ) && ( ty <= tw ) &&
                    ( -tw <= tz ) && ( tz <= tw ) ) {
                // to NDC
                tx /= tw;
                ty /= tw;
                tz /= tw;

                // close enough to 2D pick pos?
                if (    ( ndcNx <= tx ) && ( tx <= ndcPx ) &&
                        ( ndcNy <= ty ) && ( ty <= ndcPy ) ) {
                    if ( tz < currDepth ) {
                        currDepth = tz;
                        bestU = u;
                    }
                }
            }

            ++u;

        }
      }
    }

    if ( bestU >= 0 ) {
        lmID = bestU;
        return true;
    }

  }

   lmID = -1;
   return false;
}


/// @param ps
///   The pointset of the landmark to highlight.
/// @param i
///   The index of the landmark within the pointset of the landmark to
///   highlight.

void
ew::View3Landmarks::set_highlight(int ps, int i)
{
  if (highlight_ps == ps && highlight_i == i) {
    return;
  }
  ew::View3Item::set_this_highlight_item();
  highlight_ps = ps;
  highlight_i = i;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}

void
ew::View3Landmarks::clear_highlight()
{
  if (highlight_ps == -1 && highlight_i == -1) {
    return;
  }
  highlight_ps = -1;
  highlight_i = -1;
  ew::View3Item::prepared = false;
  if (ew::View3Item::get_state()) {
    ew::View3Item::redraw_view_later();
  }
}
