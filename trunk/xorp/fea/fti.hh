// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2003 International Computer Science Institute
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the Xorp LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the Xorp LICENSE file; the license in that file is
// legally binding.

// $XORP: xorp/fea/fti.hh,v 1.1.1.1 2002/12/11 23:56:02 hodson Exp $

#ifndef	__FEA_FTI_HH__
#define __FEA_FTI_HH__

#include "config.h"
#include "libxorp/xorp.h"
#include "libxorp/ipv4.hh"
#include "libxorp/ipv6.hh"
#include "libxorp/ipv4net.hh"
#include "libxorp/ipv6net.hh"

/**
 * @short Forwarding Table Entry.
 *
 * Representation of a routing table entry.
 */

template<typename A>
class Fte
{
public:
    Fte() { zero(); }
    Fte(const IPNet<A>&	net,
	const A&	gw,
	const string&	ifname,
	const string&	vifname) :
	_net(net), _gw(gw), _ifname(ifname), _vifname(vifname) {}
    Fte(IPNet<A> net) : _net(net) {}

    inline const IPNet<A>& net() const		{ return _net; }
    inline const A& gateway() const 		{ return _gw; }
    inline const string& ifname() const		{ return _ifname; }
    inline const string& vifname() const	{ return _vifname; }

    /**
     * reset all members
     */
    void zero() {
	_net = IPNet<A>(A::ZERO(), 0);
	_gw = A::ZERO();
	_vifname.erase();
    }

    /**
     * @return true if this is a host route.
     */
    inline bool host_route() const {
 	return _net.prefix_len() == A::addr_bitlen();
    }

    /**
     * @return A string representation of the entry.
     *
     * dst = 127.0.0.1 gateway = 127.0.0.1 netmask = 255.255.255.255 if = lo0
     */
    string str() const {
	return string("net = " + _net.str() + " gateway = " + _gw.str() +
		      " ifname = " + _ifname + " vifname = " + _vifname);
    }

private:
    IPNet<A>	_net;		// Network
    A		_gw; 		// Gateway address
    string	_ifname;
    string	_vifname;
};

typedef Fte<IPv4> Fte4;
typedef Fte<IPv6> Fte6;

/**
 * @short Forwarding Table Interface.
 *
 * Abstract class.
 */
class Fti {
public:

    Fti() : _in_configuration(false) {}

    /**
     * Mandatory virtual destructor in the base class.
     */
    virtual ~Fti() { }

    /**
     * Start a configuration interval.  All modifications to Fti
     * state must be within a marked "configuration" interval.
     *
     * This method provides derived classes with a mechanism to perform
     * any actions necessary before forwarding table modifications can
     * be made.
     *
     * @return true if configuration successfully started.
     */
    virtual bool start_configuration() = 0;

    /**
     * End of configuration interval.
     *
     * This method provides derived classes with a mechanism to
     * perform any actions necessary at the end of a configuration, eg
     * write a file.
     *
     * @return true configuration success pushed down into forwarding table.
     */
    virtual bool end_configuration() = 0;

    /**
     * Add a single routing entry.  Must be within a configuration
     * interval.
     *
     * @param fte.
     *
     * @return true if the deletion suceed.
     */
    virtual bool add_entry4(const Fte4& fte) = 0;

    /**
     * Delete a single routing entry. Must be with a configuration interval.
     *
     * @param fte. Only destination and netmask are used.
     *
     * @return true if the deletion suceed.
     */
    virtual bool delete_entry4(const Fte4& fte) = 0;

    /**
     * Delete all entries in the routing table. Must be within a
     * configuration interval.
     *
     * @return true if the routing table has been emptied.
     */
    virtual bool delete_all_entries4() = 0;

    /**
     * Lookup a route.
     *
     * @param dst address to resolve.
     * @param fte a returned forwarding table entry.
     * @return true if lookup suceeded.
     */
    virtual bool lookup_route4(IPv4 dst, Fte4& fte) = 0;

    /**
     * Lookup entry.
     *
     * Pass in a destination network to get a matching entry back.
     *
     * @return true if lookup suceeded.
     */
    virtual bool lookup_entry4(const IPv4Net& dst, Fte4& fte) = 0;

    /**
     * Add a single routing entry. Must be within a configuration
     * interval.
     *
     * @param tid - the transaction id returned by start_transaction.
     * @param fte table entry to be added.
     *
     * @return true if the deletion suceed.
     */
    virtual bool add_entry6(const Fte6& fte) = 0;

    /**
     * Delete a single routing entry.  Must be within a configuration
     * interval.
     *
     * @param fte table entry to be deleted.  Only destination and
     * netmask are used.
     *
     * @return true if the deletion suceed.
     */
    virtual bool delete_entry6(const Fte6& fte) = 0;

    /**
     * Lookup a route.
     *
     * @param dst address to resolve.
     * @param fte a returned forwarding table entry.
     *
     * @return true if lookup suceeded.
     */
    virtual bool lookup_route6(const IPv6& dst, Fte6& fte) = 0;

    /**
     * Lookup entry.
     *
     * Pass in a destination network to get a matching entry back.
     *
     * @return true if lookup suceeded.
     */
    virtual bool lookup_entry6(const IPv6Net& dst, Fte6& fte) = 0;

    /**
     * Delete all entries in the routing table. Must be within a
     * configuration interval.
     *
     * @return true if the routing table has been emptied.
     */
    virtual bool delete_all_entries6() = 0;

protected:
    /**
     * Mark start of a configuration.
     *
     * @return true if configuration can be marked as started, false otherwise.
     */
    inline bool mark_configuration_start() {
	if (false == _in_configuration) {
	    _in_configuration = true;
	    return true;
	}
	return false;
    }

    /**
     * Mark end of a configuration.
     *
     * @return true if configuration can be marked as ended, false otherwise.
     */
    inline bool mark_configuration_end() {
	if (true == _in_configuration) {
	    _in_configuration = false;
	    return true;
	}
	return false;
    }    
	
    inline bool in_configuration() const { return _in_configuration; }

private:
    bool _in_configuration;
};

class FtiError :  public XorpReasonedException {
public:
    FtiError(const char* file, size_t line, const string init_why)
 	: XorpReasonedException("FtiError", file, line, init_why)
    {}
};

#endif	// __FEA_FTI_HH__
