/* === S Y N F I G ========================================================= */
/*!	\file dialog_preview.h
**	\brief Preview dialog Header
**
**	$Id: dialog_preview.h,v 1.1.1.1 2005/01/07 03:34:36 darco Exp $
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

#ifndef __SYNFIG_GTKMM_DIALOG_PREVIEW_H
#define __SYNFIG_GTKMM_DIALOG_PREVIEW_H

/* === H E A D E R S ======================================================= */

#include <gtkmm/adjustment.h>
#include <gtkmm/dialog.h>
#include <gtkmm/menu.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/dialogsettings.h>

#include "preview.h"
#include <gtkmm/widget_time.h>

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */


/* === C L A S S E S & S T R U C T S ======================================= */

namespace studio {

struct PreviewInfo
{
	float zoom,fps,begintime,endtime;
	bool overbegin,overend;
};

class Dialog_Preview : public Gtk::Dialog
{
	Widget_Preview 	preview;
	DialogSettings	settings;
	
	//etl::handle<synfig::Canvas> canvas;
			
public:
	Dialog_Preview();
	~Dialog_Preview();

    void set_preview(etl::handle<Preview> prev);

	Widget_Preview &get_widget() {return preview;}
	const Widget_Preview &get_widget() const {return preview;}

	virtual void on_hide();
	//other forwarding functions...
}; // END of Dialog_Preview

class Dialog_PreviewOptions : public Gtk::Dialog
{
	//all the info needed to construct a render description...	
	Gtk::Adjustment	adj_zoom;	// factor at which to resize the window...
	
	Gtk::Adjustment	adj_fps;	// how often to take samples of the animation
	
	studio::Widget_Time time_begin;
	studio::Widget_Time time_end;
			
	Gtk::CheckButton check_overbegin;
	Gtk::CheckButton check_overend;
	
	DialogSettings	settings;
	
	float	globalfps;
	
	// for finishing
	void on_ok_pressed();
	
	//for ui stuff
	void on_overbegin_toggle();
	void on_overend_toggle();
	
	sigc::signal<void,const PreviewInfo &>	signal_finish_;
public:
	Dialog_PreviewOptions();
	~Dialog_PreviewOptions();

	float get_zoom() const { return adj_zoom.get_value(); }
	void set_zoom(float z) { adj_zoom.set_value(z); }
	
	float get_fps() const { return adj_fps.get_value(); }
	void set_fps(float z) { adj_fps.set_value(z); }
	
	float get_global_fps() const { return globalfps; }
	void set_global_fps(float f);
	
	synfig::Time get_begintime() const { return time_begin.get_value(); }
	void set_begintime(const synfig::Time &t) { time_begin.set_value(t); }
	
	synfig::Time get_endtime() const { return time_end.get_value(); }
	void set_endtime(const synfig::Time &t) { time_end.set_value(t); }
	
	bool get_begin_override() const { return check_overbegin.get_active(); }
	void set_begin_override(bool o) { check_overbegin.set_active(o); }
	
	bool get_end_override() const { return check_overend.get_active(); }
	void set_end_override(bool o) { check_overend.set_active(o); }
	
	sigc::signal<void,const PreviewInfo &>	&signal_finish() {return signal_finish_;}
};

}; // END of namespace studio

/* === E N D =============================================================== */

#endif
