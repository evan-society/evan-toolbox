// wdkg 2008-2010

#include <climits>
#include "ew/Bbox3.h"
#include "ew/DataflowNetwork.h"
#include "ew/Debug.h"
#include "ew/DebugHelper.h"
#include "ew/String.h"
#include "ew/View3Item.h"
#include "ew/View3Widget.h"

namespace {
  const char ClassName[] = "ew::View3Item";
}

ew::View3Item::View3Item(ew::View3Widget *i_view, int i_type,
 const char *dbg_final_name) :
 network(i_view->network),
 view(i_view),
 type(i_type),
 dbg_n(next_dbg_n()),
 dbg(ClassName, ew::String::ssprintf("%s#%d.%d", dbg_final_name, i_view->dbg_n,
 dbg_n)),
 prepared(false),
 state(true)
{
  dbg.on && dbg.dprintf("%s::%s(...) {", dbg.fn, "CTOR");
  view->add_item(this);
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "CTOR");
}

ew::View3Item::~View3Item()
{
  dbg.on && dbg.dprintf("%s::%s() {", dbg.fn, "DTOR");
  view->del_item(this);
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "DTOR");
}

int
ew::View3Item::next_dbg_n()
{
  if (view->dbg_last_item_n == INT_MAX) {
// unlikely
// tolerate but indicate overflow
    return 0;
  }
  view->dbg_last_item_n += 1;
  return view->dbg_last_item_n;
}

/// @param i
///   The new state of the item.

void
ew::View3Item::set_state(bool i)
{
  dbg.on && dbg.dprintf("%s::%s(%s) {", dbg.fn, "set_state",
   ew::Debug::to_str(i));
  if (state != i) {
    state = i;
    redraw_view_later();
  }
  dbg.on && dbg.dprintf("%s::%s }", dbg.fn, "set_state");
}
