/* === S Y N F I G ========================================================= */
/*!	\file template.h
**	\brief Template Header
**
**	$Id: renderer_dragbox.h,v 1.1.1.1 2005/01/07 03:34:36 darco Exp $
**
**	\legal
**	Copyright (c) 2002 Robert B. Quattlebaum Jr.
**
**	This software and associated documentation
**	are CONFIDENTIAL and PROPRIETARY property of
**	the above-mentioned copyright holder.
**
**	You may not copy, print, publish, or in any
**	other way distribute this software without
**	a prior written agreement with
**	the copyright holder.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_RENDERER_DRAGBOX_H
#define __SYNFIG_RENDERER_DRAGBOX_H

/* === H E A D E R S ======================================================= */

#include "workarearenderer.h"
#include <vector>
#include <synfig/vector.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace studio {

class Renderer_Dragbox : public studio::WorkAreaRenderer
{
	
public:
	~Renderer_Dragbox();
	
	void render_vfunc(const Glib::RefPtr<Gdk::Drawable>& drawable,const Gdk::Rectangle& expose_area	);

	const synfig::Point& get_drag_point()const;
	const synfig::Point& get_curr_point()const;

protected:
	bool get_enabled_vfunc()const;
};

}; // END of namespace studio

/* === E N D =============================================================== */

#endif
