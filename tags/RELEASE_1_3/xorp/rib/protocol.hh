// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2006 International Computer Science Institute
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

// $XORP: xorp/rib/protocol.hh,v 1.7 2005/03/25 02:54:20 pavlin Exp $

#ifndef __RIB_PROTOCOL_HH__
#define __RIB_PROTOCOL_HH__

#include "libxorp/xorp.h"


enum ProtocolType {
    IGP = 1,		// Interior Gateway Protocol
    EGP = 2		// Exterior Gateway Protocol
};

/**
 * @short Routing protocol information.
 * 
 * Protocol holds information related to a specific routing protocol
 * that is supplying information to the RIB.  
 */
class Protocol {
public:
    /**
     * Protocol constuctor
     *
     * @param name the canonical name for the routing protocol.
     * @param protocol_type the routing protocol type (@ref ProtocolType).
     * @param genid the generation ID for the protocol (if the
     * protocol goes down and comes up, the genid should be
     * incremented).
     */
    Protocol(const string& name, ProtocolType protocol_type, uint32_t genid);

    /**
     * @return the protocol type.
     * @see ProtocolType
     */
    ProtocolType protocol_type() const { return _protocol_type; }

    /**
     * @return the canonical name of the routing protocol.
     */
    const string& name() const { return _name; }

    /**
     * Equality Operator
     * 
     * Two Protocol instances are equal if they match only in name.
     * 
     * @param other the right-hand operand to compare against.
     * @return true if the left-hand Protocol instance is equal to
     * the right-hand protocol instance.
     */
    bool operator==(const Protocol& other) const {
	return (name() == other.name());
    }

    /**
     * Increment the protocol generation ID.
     */
    void increment_genid() { _genid++; }

private:
    string		_name;
    ProtocolType	_protocol_type;	// The protocol type (IGP or EGP)
    uint32_t		_genid;		// The protocol generation ID.
};

#endif // __RIB_PROTOCOL_HH__
