/* === S Y N F I G ========================================================= */
/*!	\file widget_sound.cpp
**	\brief Widget Sound Implementation File
**
**	$Id: widget_sound.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

#include <gtkmm/adjustment.h>

#include <synfig/general.h>
#include <ETL/clock>

#include "widget_sound.h"
#include "audiocontainer.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
//using namespace synfig;

using studio::AudioProfile;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */
	
studio::Widget_Sound::Widget_Sound()
{
}

studio::Widget_Sound::~Widget_Sound()
{
}

void studio::Widget_Sound::set_position(double t)
{
	//synfig::info("Setting position to %.2lf s", t);
	if(adj_timescale && t != adj_timescale->get_value())
	{
		float upper = adj_timescale->get_upper();
		float lower = adj_timescale->get_lower();
		float framesize =  upper - lower;
		
		if(t < lower)
		{
			lower -= ceil((lower-t)/framesize)*framesize;
			upper = lower + framesize;
			adj_timescale->set_lower(lower); adj_timescale->set_upper(upper);
			adj_timescale->set_value(t);
			adj_timescale->changed(); adj_timescale->value_changed(); 
		}else
		if(t > upper)
		{
			lower += ceil((t-upper)/framesize)*framesize;
			upper = lower + framesize;
			adj_timescale->set_lower(lower); adj_timescale->set_upper(upper);
			adj_timescale->set_value(t);
			adj_timescale->changed(); adj_timescale->value_changed();
		}else
		{
			adj_timescale->set_value(t);
			adj_timescale->value_changed();
		}
	}
}

double studio::Widget_Sound::get_position() const
{
	if(adj_timescale)
	{
		return adj_timescale->get_value();
	}
	return 0;
}

bool studio::Widget_Sound::set_profile(etl::handle<AudioProfile>	p)
{
	clear();

	//set the profile
	audioprof = p;
	
	if(!audioprof)
	{
		clear();
		return false;
	}
	
	return true;
}

etl::handle<AudioProfile>	studio::Widget_Sound::get_profile() const
{
	return audioprof;
}

void studio::Widget_Sound::clear()
{
	audioprof.detach();
}

void studio::Widget_Sound::draw()
{
	on_expose_event();
}

bool studio::Widget_Sound::on_expose_event(GdkEventExpose *heh)
{	
	if(!get_window()) return false;	

	//clear the background to dark grey
	Glib::RefPtr<Gdk::GC>	gc = Gdk::GC::create(get_window());
	
	if(!gc) return false;

	{
		Gdk::Rectangle r(0,0,get_width(),get_height());
		get_window()->begin_paint_rect(r);
	}
	Gdk::Color	c("#3f3f3f");	
	gc->set_rgb_fg_color(c);
	gc->set_background(c);

	int w = get_width();
	int baseline = get_height()/2;
	get_window()->draw_rectangle(gc,true,0,0,w,get_height());
	
	//set up the color to be blue
	c.set_rgb_p(0,0.5,1);
	gc->set_rgb_fg_color(c);
	
	//draw the base line
	get_window()->draw_line(gc,0,baseline,w,baseline);
	
	//redraw all the samples from begin to end, but only if we have samples to draw (or there is no space to draw)
	
	//synfig::warning("Ok rendered everything, now must render actual sound wave");
	if(!audioprof || !adj_timescale || !w) 
	{
		get_window()->end_paint();
		return true;
	}
	
	//draw you fool!
	float framesize = adj_timescale->get_upper() - adj_timescale->get_lower();
	if(framesize)
	{
		float delta=0,cum=0;
		
		//position in sample space
		int begin=0,end=0;
		int	cur=0,maxs=0,mins=0;
		
		int	  i=0; //pixel counter
		
		//etl::clock	check; check.reset();
		
		float position = adj_timescale->get_value();
		float samplerate = audioprof->get_samplerate();
		int		posi = 0;		
		//enforce position inside of frame size	
		{
			float offset = audioprof->get_offset();
		
			//clamp begin and end to framesize
			float beginf = adj_timescale->get_lower();
			float endf = adj_timescale->get_upper();
			
			posi = round_to_int((position-beginf)*w/framesize);
			//posi = (int)((position-beginf)*w/framesize);
			
			//calculate in sample space from seconds
			begin = round_to_int((beginf - offset)*samplerate);
			end = round_to_int((endf - offset)*samplerate);
			//begin = (int)((beginf - offset)*samplerate);
			//end = (int)((endf - offset)*samplerate);
		}
		
		delta = (end - begin)/(float)w; //samples per pixel
		
		/*synfig::warning("Rendering a framesize of %f secs from [%d,%d) samples to %d samples, took %f sec", 
						framesize, begin, end, w, check());*/
		
		cur = begin;
		i = 0; cum = 0;
		for(int i=0;i<w;++i)
		{
			//get the maximum of the collected samples
			maxs = 0;
			mins = 0;
			for(;cum < delta; ++cum, ++cur)
			{
				maxs = std::max(maxs,(int)(*audioprof)[cur]);
				mins = std::min(mins,(int)(*audioprof)[cur]);
			}
			cum -= delta;
			
			//draw spike if not needed be
			if(maxs||mins)
			{
				int top = maxs * baseline / 64;
				int bot = mins * baseline / 64;
				
				get_window()->draw_line(gc,i,baseline+bot,i,baseline+top);
			}
		}
		
		//synfig::warning("Drawing audio line");
		c.set_rgb_p(1,0,0);
		gc->set_rgb_fg_color(c);
		get_window()->draw_line(gc,posi,0,posi,get_height());
	}
	get_window()->end_paint();
		
	return true;
}

//--- Handle the single clicking and dragging for scrubbing

bool studio::Widget_Sound::on_motion_notify_event(GdkEventMotion* event)
{
	Gdk::ModifierType	mod = Gdk::ModifierType(event->state);

	//if we are scrubbing
	if(mod & Gdk::BUTTON1_MASK)
	{
		//Can't do this if we don't have a time frame (heheh...)
		if(!adj_timescale) return false;
			
		double beg = adj_timescale->get_lower(), end = adj_timescale->get_upper();
		
		//find event position in time
		double t = beg + event->x * (end-beg) / get_width();

		//signal that we are scrubbing to this new value...
		signal_scrub()(t);
				
		
		// We should be able to just call
		// Widget_Timeslider::on_motion_notify_event(),
		// but that seems to cause the program to halt
		// for some reason. So for now, let's do the job ourselves
		//adj_timescale->set_value(t);
		//adj_timescale->changed();
		//return true;
	}
	
	return Widget_Timeslider::on_motion_notify_event(event);
}

bool studio::Widget_Sound::on_button_press_event(GdkEventButton *event)
{
	//Assume button PRESS
	
	//if we are starting... using left click
	if(event->button == 1)
	{
		if(!adj_timescale) return false;
			
		double beg = adj_timescale->get_lower(), end = adj_timescale->get_upper();
		
		//find event position in time
		double t = beg + event->x * (end-beg) / get_width();

		//signal the attached scrubbing devices...
		signal_start_scrubbing()(t);
				
		return true;
	}
	
	return Widget_Timeslider::on_button_press_event(event);
}

bool studio::Widget_Sound::on_button_release_event(GdkEventButton *event)
{
	//Assume button RELEASE
	
	//if we are ending... using left click
	if(event->button == 1)
	{
		//signal the scrubbing device... to stop
		signal_stop_scrubbing()();
				
		return true;
	}
	
	return Widget_Timeslider::on_button_release_event(event);
}
