// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-
// vim:set sts=4 ts=8:

// Copyright (c) 2001-2008 XORP, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License, Version
// 2.1, June 1999 as published by the Free Software Foundation.
// Redistribution and/or modification of this program under the terms of
// any other version of the GNU Lesser General Public License is not
// permitted.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. For more details,
// see the GNU Lesser General Public License, Version 2.1, a copy of
// which can be found in the XORP LICENSE.lgpl file.
// 
// XORP, Inc, 2953 Bunker Hill Lane, Suite 204, Santa Clara, CA 95054, USA;
// http://xorp.net

#ident "$XORP: xorp/libxorp/eventloop.cc,v 1.42 2008/10/02 21:57:30 bms Exp $"

#include "libxorp_module.h"

#include "libxorp/xorp.h"
#include "libxorp/xlog.h"
#include "libxorp/debug.h"

#include "eventloop.hh"


//
// Number of EventLoop instances.
//
int eventloop_instance_count;

EventLoop::EventLoop()
    : _clock(new SystemClock), _timer_list(_clock), _aggressiveness(0),
#ifdef HOST_OS_WINDOWS
      _win_dispatcher(_clock)
#else
      _selector_list(_clock)
#endif
{
    XLOG_ASSERT(eventloop_instance_count == 0);
    eventloop_instance_count++;
}

EventLoop::~EventLoop()
{
    eventloop_instance_count--;
    XLOG_ASSERT(eventloop_instance_count == 0);
    //
    // XXX: The _clock pointer is invalidated for other EventLoop fields
    // that might be using it.
    //
    delete _clock;
    _clock = NULL;
}

void
EventLoop::run()
{
    const time_t MAX_ALLOWED = 2;
    static time_t last_ev_run = 0;
    static time_t last_warned;
    TimeVal t;

    _timer_list.advance_time();
    _timer_list.current_time(t);

    if (last_ev_run == 0)
	last_ev_run = t.sec();

    time_t now  = t.sec();
    time_t diff = now - last_ev_run;

    if (now - last_warned > 0 && (diff > MAX_ALLOWED)) {
	XLOG_WARNING("%d seconds between calls to EventLoop::run", (int)diff);
	last_warned = now;
    }

    // standard eventloop run
    bool more = do_work(true);

    // Aggressive eventloop runs.  Going through the eventloop from the start is
    // expensive because we need to call time() and select().  If there's
    // additional work to do, do it now rather than going through all the
    // bureaucracy again.  Because we (sometimes) have an outdated time() and
    // select() we could run low priority tasks instead of higher ones simply
    // because we don't know about them yet (need to call time or select to
    // detect them).  For this reason, we only do a few "aggressive" runs.
    // -sorbo.
    int agressiveness = _aggressiveness;
    while (more && agressiveness--)
	more = do_work(false);

    // select() could cause a large delay and will advance_time.  Re-read
    // current time.  Maybe we can get rid of advance_time if timeout to select
    // is small, or get rid of advance_time at the top of event loop.  -sorbo
    _timer_list.current_time(t);
    last_ev_run = t.sec();
}

bool
EventLoop::do_work(bool can_block)
{
    TimeVal t;

    _timer_list.current_time(t);
    _timer_list.get_next_delay(t);

    int timer_priority	  = XorpTask::PRIORITY_INFINITY;
    int selector_priority = XorpTask::PRIORITY_INFINITY;
    int task_priority	  = XorpTask::PRIORITY_INFINITY;

    if (t == TimeVal::ZERO())
	timer_priority = _timer_list.get_expired_priority();

#ifdef HOST_OS_WINDOWS
    if (_win_dispatcher.ready())
	selector_priority = _win_dispatcher.get_ready_priority();
#else
    selector_priority = _selector_list.get_ready_priority(can_block);
#endif

    if (!_task_list.empty())
	task_priority = _task_list.get_runnable_priority();

    debug_msg("Priorities: timer = %d selector = %d task = %d\n",
	      timer_priority, selector_priority, task_priority);

    if ( (timer_priority != XorpTask::PRIORITY_INFINITY)
	 && (timer_priority <= selector_priority)
	 && (timer_priority <= task_priority)) {

	// the most important thing to run next is a timer
	_timer_list.run();

    } else if ( (selector_priority != XorpTask::PRIORITY_INFINITY)
		&& (selector_priority <= task_priority) ) {

	// the most important thing to run next is a selector
#ifdef HOST_OS_WINDOWS
	_win_dispatcher.wait_and_dispatch(t);
#else
	_selector_list.wait_and_dispatch(t);
#endif

    } else if (task_priority != XorpTask::PRIORITY_INFINITY) {

	// the most important thing to run next is a task
	_task_list.run();

    } else {
	if (!can_block)
	    return false;

	// there's nothing immediate to run, so go to sleep until the
	// next selector or timer goes off
#ifdef HOST_OS_WINDOWS
	_win_dispatcher.wait_and_dispatch(t);
#else
	_selector_list.wait_and_dispatch(t);
#endif
	// XXX return false if you want to be conservative.  -sorbo.
    }

    return true;
}

bool
EventLoop::add_ioevent_cb(XorpFd fd, IoEventType type, const IoEventCb& cb,
			  int priority)
{
#ifdef HOST_OS_WINDOWS
    return _win_dispatcher.add_ioevent_cb(fd, type, cb, priority);
#else
    return _selector_list.add_ioevent_cb(fd, type, cb, priority);
#endif
}

bool
EventLoop::remove_ioevent_cb(XorpFd fd, IoEventType type)
{
#ifdef HOST_OS_WINDOWS
    return _win_dispatcher.remove_ioevent_cb(fd, type);
#else
    _selector_list.remove_ioevent_cb(fd, type);
    return true;
#endif
}

size_t
EventLoop::descriptor_count() const
{
#ifdef HOST_OS_WINDOWS
    return _win_dispatcher.descriptor_count();
#else
    return _selector_list.descriptor_count();
#endif
}

XorpTask
EventLoop::new_oneoff_task(const OneoffTaskCallback& cb, int priority,
			   int weight)
{
    return _task_list.new_oneoff_task(cb, priority, weight);
}

XorpTask
EventLoop::new_task(const RepeatedTaskCallback& cb, int priority, int weight)
{
    return _task_list.new_task(cb, priority, weight);
}

void
EventLoop::set_aggressiveness(int num)
{
    _aggressiveness = num;
}
