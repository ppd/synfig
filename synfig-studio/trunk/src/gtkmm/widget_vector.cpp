/* === S Y N F I G ========================================================= */
/*!	\file widget_vector.cpp
**	\brief Template File
**
**	$Id: widget_vector.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <gtkmm/spinbutton.h>
#include "widget_vector.h"
#include "widget_distance.h"
#include "app.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace synfig;
using namespace studio;

/* === M A C R O S ========================================================= */

#define DIGITS		10

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Widget_Vector::Widget_Vector():
	x_adjustment(0,-100000000,100000000,0.05,0.05,0.05),
	y_adjustment(0,-100000000,100000000,0.05,0.05,0.05)
{
	Gtk::Label *label;
	
	label=manage(new class Gtk::Label("x:"));
	label->show();
	pack_start(*label, Gtk::PACK_SHRINK);

	spinbutton_x=manage(new class Gtk::SpinButton(x_adjustment,0.05,DIGITS));
	spinbutton_x->set_update_policy(Gtk::UPDATE_ALWAYS);
	spinbutton_x->signal_value_changed().connect(sigc::mem_fun(*this,&studio::Widget_Vector::on_value_changed));
	pack_start(*spinbutton_x, Gtk::PACK_EXPAND_WIDGET);

	distance_x=manage(new Widget_Distance());
	distance_x->set_digits(4);
	distance_x->set_update_policy(Gtk::UPDATE_ALWAYS);
	distance_x->signal_value_changed().connect(sigc::mem_fun(*this,&studio::Widget_Vector::on_value_changed));
	pack_start(*distance_x, Gtk::PACK_EXPAND_WIDGET);

	label=manage(new class Gtk::Label("y:"));
	label->show();
	pack_start(*label, Gtk::PACK_SHRINK);

	spinbutton_y=manage(new class Gtk::SpinButton(y_adjustment,0.05,DIGITS));
	spinbutton_y->set_update_policy(Gtk::UPDATE_ALWAYS);
	spinbutton_y->signal_value_changed().connect(sigc::mem_fun(*this,&studio::Widget_Vector::on_value_changed));
	spinbutton_y->signal_activate().connect(sigc::mem_fun(*this,&studio::Widget_Vector::activate));
	pack_start(*spinbutton_y, Gtk::PACK_EXPAND_WIDGET);

	distance_y=manage(new Widget_Distance());
	distance_y->set_digits(4);
	distance_y->set_update_policy(Gtk::UPDATE_ALWAYS);
	distance_y->signal_value_changed().connect(sigc::mem_fun(*this,&studio::Widget_Vector::on_value_changed));
	distance_y->signal_activate().connect(sigc::mem_fun(*this,&studio::Widget_Vector::activate));
	pack_start(*distance_y, Gtk::PACK_EXPAND_WIDGET);

	spinbutton_x->show();
	spinbutton_y->show();
	
	spinbutton_x->signal_activate().connect(sigc::mem_fun(*spinbutton_y,&Gtk::SpinButton::grab_focus));
	distance_x->signal_activate().connect(sigc::mem_fun(*distance_y,&Gtk::SpinButton::grab_focus));
}

Widget_Vector::~Widget_Vector()
{
}

void
Widget_Vector::on_grab_focus()
{
	if(canvas_)
		distance_x->grab_focus();
	else
		spinbutton_x->grab_focus();
}

void
Widget_Vector::set_has_frame(bool x)
{
	if(spinbutton_x)
	{
		spinbutton_x->set_has_frame(x);
		spinbutton_y->set_has_frame(x);
		spinbutton_x->set_size_request(48,-1);
		spinbutton_y->set_size_request(48,-1);
	}

	distance_x->set_has_frame(x);
	distance_y->set_has_frame(x);
	distance_x->set_size_request(48,-1);
	distance_y->set_size_request(48,-1);
}

void
Widget_Vector::set_digits(int x)
{
	if(spinbutton_x)
	{
		spinbutton_x->set_digits(x);
		spinbutton_y->set_digits(x);
		spinbutton_x->set_size_request(48,-1);
		spinbutton_y->set_size_request(48,-1);
	}
	
	distance_x->set_digits(x);
	distance_y->set_digits(x);
	distance_x->set_size_request(48,-1);
	distance_y->set_size_request(48,-1);
}

void
Widget_Vector::set_value(const synfig::Vector &data)
{
	vector=data;

	if(canvas_){try
	{
		Distance distx(vector[0],Distance::SYSTEM_UNITS),disty(vector[1],Distance::SYSTEM_UNITS);
		distx.convert(App::distance_system,canvas_->rend_desc());
		disty.convert(App::distance_system,canvas_->rend_desc());
		distance_x->set_value(distx);
		distance_y->set_value(disty);
		spinbutton_x->hide();
		spinbutton_y->hide();
	}catch(...) { synfig::error("Widget_Vector::set_value(): Caught something that was thrown"); }}
	else
	{
		spinbutton_x->set_value(vector[0]);
		spinbutton_y->set_value(vector[1]);
		distance_x->hide();
		distance_y->hide();
	}
}

const synfig::Vector &
Widget_Vector::get_value()
{
	if(!canvas_ && spinbutton_x)
	{
		vector[0]=spinbutton_x->get_value();
		vector[1]=spinbutton_y->get_value();
		distance_x->hide();
		distance_y->hide();
	}
	else try
	{
		vector[0]=distance_x->get_value().units(canvas_->rend_desc());
		vector[1]=distance_y->get_value().units(canvas_->rend_desc());
		spinbutton_x->hide();
		spinbutton_y->hide();
	}catch(...) { synfig::error("Widget_Vector::set_value(): Caught something that was thrown"); }
	return vector;
}

void
Widget_Vector::on_value_changed()
{
	signal_value_changed()();
}

void
Widget_Vector::set_canvas(Canvas::LooseHandle x)
{
	canvas_=x;
	if(x)
	{
		if(spinbutton_x)
		{
			spinbutton_x->hide();
			spinbutton_y->hide();
//			delete spinbutton_x;
//			delete spinbutton_y;
		}
		distance_x->show();
		distance_y->show();
	}
	else
	{
		if(spinbutton_x)
		{
			spinbutton_x->show();
			spinbutton_y->show();
		}
		distance_x->hide();
		distance_y->hide();
	}
}

void
Widget_Vector::show_all_vfunc()
{
	if(canvas_)
	{
		distance_x->show();
		distance_y->show();
	}
	else
	{
		spinbutton_x->show();
		spinbutton_y->show();
	}
	show();
}
