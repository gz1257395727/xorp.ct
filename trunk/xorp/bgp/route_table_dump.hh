// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2005 International Computer Science Institute
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the XORP LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the XORP LICENSE file; the license in that file is
// legally binding.

// $XORP: xorp/bgp/route_table_dump.hh,v 1.19 2006/02/17 23:34:54 zec Exp $

#ifndef __BGP_ROUTE_TABLE_DUMP_HH__
#define __BGP_ROUTE_TABLE_DUMP_HH__

#include "route_table_base.hh"
#include "peer_handler.hh"
#include "bgp_trie.hh"
#include "dump_iterators.hh"
#include "peer_route_pair.hh"

#define AUDIT_ENABLE
#define AUDIT_LEN 1000

class EventLoop;

template<class A>
class DumpTable : public BGPRouteTable<A>  {
public:
    DumpTable(string tablename,
	      const PeerHandler *peer,
	      const list <const PeerTableInfo<A>*>& peer_list,
	      BGPRouteTable<A> *parent,
	      Safi safi);
    int add_route(const InternalMessage<A> &rtmsg,
		  BGPRouteTable<A> *caller);
    int replace_route(const InternalMessage<A> &old_rtmsg,
		      const InternalMessage<A> &new_rtmsg,
		      BGPRouteTable<A> *caller);
    int delete_route(const InternalMessage<A> &rtmsg, 
		     BGPRouteTable<A> *caller);
    int route_dump(const InternalMessage<A> &rtmsg,
		   BGPRouteTable<A> *caller,
		   const PeerHandler *dump_peer);
    int push(BGPRouteTable<A> *caller);
    const SubnetRoute<A> *lookup_route(const IPNet<A> &net,
				       uint32_t& genid) const;
    void route_used(const SubnetRoute<A>* route, bool in_use);


    RouteTableType type() const {return DUMP_TABLE;}
    string str() const;

    /* mechanisms to implement flow control in the output plumbing */
    bool get_next_message(BGPRouteTable<A> *next_table);

    void initiate_background_dump();
    void suspend_dump();

    /**
     * A peer which is down but still deleting routes when this peer
     * is brought up.
     */
    void peering_is_down(const PeerHandler *peer, uint32_t genid);

    /**
     * A peer which does down while dumping is taking place.
     */
    void peering_went_down(const PeerHandler *peer, uint32_t genid,
			   BGPRouteTable<A> *caller);

    /**
     * A peer that is down and has now completed deleting all its routes.
     */
    void peering_down_complete(const PeerHandler *peer, uint32_t genid,
			       BGPRouteTable<A> *caller);

    /**
     * A peer which was previously down came up.
     */
    void peering_came_up(const PeerHandler *peer, uint32_t genid,
			 BGPRouteTable<A> *caller);

#ifdef AUDIT_ENABLE
    void print_and_clear_audit();
#endif
private:
    /**
     * Called when the dump table has completed its tasks.
     */
    void completed();
    void schedule_unplumb_self();
    void unplumb_self();
    void wakeup_downstream();
    bool do_next_route_dump();
    EventLoop& eventloop() const {return _peer->eventloop();}

    const PeerHandler *_peer;
    DumpIterator<A> _dump_iter;
    bool _output_busy;

    int _dumped;
    bool _dump_active;  // true if the dump is in progress
    bool _dump_chunk_active;  // true if DumpTable has been woken up
			      // to dump one chunk, false if execution
			      // is due to an external event.
    XorpTimer _dump_timer;

    //if we get to the end of the route dump, and some peers that went
    //down during our dump have not yet finished their deletion
    //process, we need to delay unplumbing ourself until they finish
    //to avoid propagating unnecessary deletes downstream.
    bool _waiting_for_deletion_completion;

    //The dump table has done its job and can be removed.
    bool _completed;

#ifdef AUDIT_ENABLE
    //audit trail for debugging - keep a log of last few events, use
    //this as a circular buffer
    string _audit_entry[AUDIT_LEN];
    int _first_audit, _last_audit, _audit_entries;
    void add_audit(const string& log_entry);

#endif

};

#endif // __BGP_ROUTE_TABLE_DUMP_HH__
