/* === S Y N F I G ========================================================= */
/*!	\file widget_compselect.cpp
**	\brief Template File
**
**	$Id: widget_compselect.cpp,v 1.1.1.1 2005/01/07 03:34:37 darco Exp $
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

#include <gtkmm/menu.h>
#include "widget_compselect.h"
#include <ETL/stringf>
#include <synfig/valuenode.h>
#include "instance.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;
using namespace studio;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Widget_CompSelect::Widget_CompSelect()
{
	App::signal_instance_created().connect(sigc::mem_fun(*this,&studio::Widget_CompSelect::new_instance));
	App::signal_instance_deleted().connect(sigc::mem_fun(*this,&studio::Widget_CompSelect::delete_instance));
	App::signal_instance_selected().connect(sigc::mem_fun(*this,&studio::Widget_CompSelect::set_selected_instance_signal));

	set_menu(instance_list_menu);
	refresh();
}

Widget_CompSelect::~Widget_CompSelect()
{
}

void
Widget_CompSelect::set_selected_instance_signal(etl::handle<studio::Instance> x)
{
	set_selected_instance(x);
}

void
Widget_CompSelect::set_selected_instance_(etl::handle<studio::Instance> instance)
{
	if(studio::App::shutdown_in_progress)
		return;

	selected_instance=instance;
}

void
Widget_CompSelect::set_selected_instance(etl::loose_handle<studio::Instance> x)
{
	if(studio::App::shutdown_in_progress)
		return;

	// if it's already selected, don't select it again
	if (x==selected_instance)
		return;

	std::list<etl::handle<studio::Instance> >::iterator iter;

	if(x)
	{
		int i;
		for(i=0,iter=studio::App::instance_list.begin();iter!=studio::App::instance_list.end() && ((*iter)!=x);iter++,i++);

		assert(*iter==x);

		set_history(i);
	}
	else
		set_history(0);

	set_selected_instance_(x);	
}

void
Widget_CompSelect::new_instance(etl::handle<studio::Instance> instance)
{
	if(studio::App::shutdown_in_progress)
		return;
	
	assert(instance);
	
	etl::loose_handle<studio::Instance> loose_instance(instance);
	
	instance->synfigapp::Instance::signal_filename_changed().connect(sigc::mem_fun(*this,&Widget_CompSelect::refresh));
	instance->synfigapp::Instance::signal_filename_changed().connect(
		sigc::bind<etl::loose_handle<studio::Instance> >(
			sigc::mem_fun(*this,&Widget_CompSelect::set_selected_instance),
			loose_instance
		)
	);

	{
		std::string name=basename(instance->get_file_name());

		instance_list_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(name,
			sigc::bind<etl::loose_handle<studio::Instance> >(sigc::ptr_fun(&studio::App::set_selected_instance),loose_instance)	));
	}
	
}

void
Widget_CompSelect::delete_instance(etl::handle<studio::Instance> instance)
{
	DEBUGPOINT();
	refresh();

	if(selected_instance==instance)
	{
		set_selected_instance(0);
		set_history(0);
	}
}

void
Widget_CompSelect::refresh()
{
	remove_menu();

	if(!instance_list_menu.items().empty())
		instance_list_menu.items().clear();

	if(studio::App::shutdown_in_progress)
		return;

	std::list<etl::handle<studio::Instance> >::iterator iter;
	for(iter=studio::App::instance_list.begin();iter!=studio::App::instance_list.end();iter++)
	{
		std::string name=basename((*iter)->get_file_name());

		instance_list_menu.items().push_back(Gtk::Menu_Helpers::MenuElem(name,
			sigc::bind<etl::loose_handle<studio::Instance> >(sigc::ptr_fun(&studio::App::set_selected_instance),*iter)	));
	}
	set_menu(instance_list_menu);
}
