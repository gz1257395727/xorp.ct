// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001,2002 International Computer Science Institute
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

// $XORP: xorp/bgp/main.hh,v 1.6 2003/01/24 19:50:10 rizzo Exp $

#ifndef __BGP_MAIN_HH__
#define __BGP_MAIN_HH__

#include "socket.hh"
#include "packet.hh"

#include "peer.hh"
#include "peer_list.hh"
#include "plumbing.hh"
#include "iptuple.hh"
#include "libxipc/xrl_std_router.hh"
#include "libxorp/eventloop.hh"
#include "path_attribute_list.hh"

#include "peer_handler.hh"

class BGPMain {
public:
    BGPMain();
    ~BGPMain();

    /**
     * Set the local configuration.
     *
     * @param as as number.
     *
     * @param id router id.
     */
    void local_config(const uint32_t& as, const IPv4& id);

    /**
     * attach peer to peerlist
     *
     * @param p BGP peer.
     */
    void attach_peer(BGPPeer *p);

    /**
     * detach peer from the peerlist.
     *
     * @param p BGP peer.
     */
    void detach_peer(BGPPeer *p);

    /**
     * Find peer with this iptuple
     *
     * @param search iptuple.
     *
     * @return A pointer to a peer if one is found NULL otherwise.
     */
    BGPPeer *find_peer(const Iptuple& search);

    /**
     * create a new peer and attach it to the peerlist.
     *
     * @param pd BGP peer data.
     *
     * @return true on success
     */
    bool create_peer(BGPPeerData *pd);

    /**
     * delete peer tear down connection and remove for peerlist.
     *
     * @param iptuple iptuple.
     *
     * @return true on success
     */
    bool delete_peer(const Iptuple& iptuple);

    /**
     * enable peer
     *
     * @param iptuple iptuple.
     *
     * @return true on success
     */
    bool enable_peer(const Iptuple& iptuple);

    /**
     * disable peer
     *
     * @param iptuple iptuple.
     *
     * @return true on success
     */
    bool disable_peer(const Iptuple& iptuple);

    bool get_peer_list_start(uint32_t& token);

    bool get_peer_list_next(const uint32_t& token, 
			    IPv4& local_ip, 
			    uint32_t& local_port, 
			    IPv4& peer_ip, 
			    uint32_t& peer_port);

    bool get_peer_id(const Iptuple& iptuple, IPv4& peer_id);
    bool get_peer_status(const Iptuple& iptuple,  uint32_t& peer_state, 
			 uint32_t& admin_status);
    bool get_peer_negotiated_version(const Iptuple& iptuple, 
				     int32_t& neg_version);
    bool get_peer_as(const Iptuple& iptuple,   uint32_t& peer_as);
    bool get_peer_msg_stats(const Iptuple& iptuple, 
			    uint32_t& in_updates, 
			    uint32_t& out_updates, 
			    uint32_t& in_msgs, 
			    uint32_t& out_msgs, 
			    uint16_t& last_error, 
			    uint32_t& in_update_elapsed);
    bool get_peer_established_stats(const Iptuple& iptuple,  
				    uint32_t& transitions, 
				    uint32_t& established_time);
    bool get_peer_timer_config(const Iptuple& iptuple,
			       uint32_t& retry_interval, 
			       uint32_t& hold_time, 
			       uint32_t& keep_alive, 
			       uint32_t& hold_time_configured, 
			       uint32_t& keep_alive_configured, 
			       uint32_t& min_as_origination_interval,
			       uint32_t& min_route_adv_interval);

    bool register_ribname(const string& name);

    void main_loop();
    void terminate() { _exit_loop = true; }
    int create_listener(const Iptuple& iptuple);
    LocalData *get_local_data();
    void start_server(const Iptuple& iptuple);
    void stop_server(const Iptuple& iptuple);
    /**
     * Stop listening for incoming connections.
     */
    void stop_all_servers();

    /**
     * add route
     *
     * Typically called via XRL's to add static routes.
     *
     * @param origin the origin of the path information.
     * @param asum an as number will eventually be a path.
     * @param next_hop the border router that should be used as a
     * detination for the nlri.
     * @param nlri network level reachability information.
     * @param success returns true if the route was sucessfully added.
     */
    bool add_route(const OriginType origin,  const AsNum& as,
		   const IPv4& next_hop, const IPv4Net&	nlri);
    /**
     * delete route
     *
     * Typically called via XRL's to delete static routes.
     *
     * @param nlri network level reachability information.
     * @param success returns true if the route was sucessfully added.
     */
    bool delete_route(const IPv4Net& nlri);

    bool get_route_list_start4(uint32_t& token);
    bool get_route_list_start6(uint32_t& token);
    
    bool get_route_list_next4(
			      // Input values, 
			      const uint32_t&	token, 
			      // Output values, 
			      IPv4&	peer_id, 
			      IPv4Net& net, 
			      uint32_t& origin, 
			      vector<uint8_t>& aspath, 
			      IPv4& nexthop, 
			      int32_t& med, 
			      int32_t& localpref, 
			      int32_t& atomic_agg, 
			      vector<uint8_t>& aggregator, 
			      int32_t& calc_localpref, 
			      vector<uint8_t>& attr_unknown,
			      bool& best);
    bool get_route_list_next6(
			      // Input values, 
			      const uint32_t&	token, 
			      // Output values, 
			      IPv4&	peer_id, 
			      IPv6Net& net, 
			      uint32_t& origin, 
			      vector<uint8_t>& aspath, 
			      IPv6& nexthop, 
			      int32_t& med, 
			      int32_t& localpref, 
			      int32_t& atomic_agg, 
			      vector<uint8_t>& aggregator, 
			      int32_t& calc_localpref, 
			      vector<uint8_t>& attr_unknown,
			      bool& best);

    bool rib_client_route_info_changed4(
					// Input values,
					const IPv4&	addr,
					const uint32_t&	prefix_len,
					const IPv4&	nexthop,
					const uint32_t&	metric);

    bool rib_client_route_info_changed6(
					// Input values,
					const IPv6&	addr,
					const uint32_t&	prefix_len,
					const IPv6&	nexthop,
					const uint32_t&	metric);

    bool rib_client_route_info_invalid4(
					// Input values,
					const IPv4&	addr,
					const uint32_t&	prefix_len);

    bool rib_client_route_info_invalid6(
					// Input values,
					const IPv6&	addr,
					const uint32_t&	prefix_len);

    /**
     * set parameter
     *
     * Typically called via XRL's to set which parameters we support
     * per peer.
     *
     * @param iptuple iptuple
     * @param parameter we are setting for this peer.
     */
    bool set_parameter(
				// Input values,
		       const Iptuple& iptuple,
		       const string& parameter);

    /**
     * Originally inserted for testing. However, now used by all the
     * "rib_client_route_info_*" methods.
     */
    BGPPlumbing *plumbing() const { return _plumbing; }

    XrlStdRouter *get_router() { return _xrl_router; }
    EventLoop *get_eventloop() { return &_eventloop; }
protected:
private:
    /**
     * Store the socket descriptor and iptuple together.
     */
    struct Server {
	Server(int fd, Iptuple iptuple) : _serverfd(fd) {
	    _tuples.push_back(iptuple);
	}
	Server(const Server& rhs) {
	    _serverfd = rhs._serverfd;
	    _tuples = rhs._tuples;
	}
	Server operator=(const Server& rhs) {
	    if (&rhs == this)
		return *this;

	    _serverfd = rhs._serverfd;
	    _tuples = rhs._tuples;

	    return *this;
	}
	int _serverfd;
	list <Iptuple> _tuples;
    };

    list<Server> _serverfds;

    /**
     * Callback method called when a connection attempt is made.
     */
    void connect_attempt(int fd, SelectorMask m,
			 struct in_addr laddr, uint16_t lport);

    template <class A>
    void extract_attributes(// Input values, 
			    const PathAttributeList<A>& attributes, 
			    // Output values, 
			    uint32_t& origin, 
			    vector<uint8_t>& aspath, 
			    A& nexthop,
			    int32_t& med, 
			    int32_t& localpref, 
			    int32_t& atomic_agg, 
			    vector<uint8_t>& aggregator, 
			    int32_t& calc_localpref, 
			    vector<uint8_t>& attr_unknown);


    bool _exit_loop;
    BGPPeerList *_peerlist;
    BGPPlumbing *_plumbing;
    RibIpcHandler *_rib_ipc_handler;
    LocalData _local_data;
    XrlStdRouter *_xrl_router;
    static EventLoop _eventloop;
};

#endif // __BGP_MAIN_HH__
