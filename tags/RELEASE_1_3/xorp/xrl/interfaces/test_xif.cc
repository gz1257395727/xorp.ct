/*
 * Copyright (c) 2001-2006 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 */

#ident "$XORP: xorp/xrl/interfaces/test_xif.cc,v 1.14 2006/03/16 00:06:21 pavlin Exp $"

#include "test_xif.hh"

bool
XrlTestV1p0Client::send_print_hello_world(
	const char*	dst_xrl_target_name,
	const PrintHelloWorldCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "test/1.0/print_hello_world");
    return _sender->send(x, callback(this, &XrlTestV1p0Client::unmarshall_print_hello_world, cb));
}


/* Unmarshall print_hello_world */
void
XrlTestV1p0Client::unmarshall_print_hello_world(
	const XrlError&	e,
	XrlArgs*	a,
	PrintHelloWorldCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlTestV1p0Client::send_print_hello_world_and_message(
	const char*	dst_xrl_target_name,
	const string&	msg,
	const PrintHelloWorldAndMessageCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "test/1.0/print_hello_world_and_message");
    x.args().add("msg", msg);
    return _sender->send(x, callback(this, &XrlTestV1p0Client::unmarshall_print_hello_world_and_message, cb));
}


/* Unmarshall print_hello_world_and_message */
void
XrlTestV1p0Client::unmarshall_print_hello_world_and_message(
	const XrlError&	e,
	XrlArgs*	a,
	PrintHelloWorldAndMessageCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}

bool
XrlTestV1p0Client::send_get_greeting_count(
	const char*	dst_xrl_target_name,
	const GetGreetingCountCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "test/1.0/get_greeting_count");
    return _sender->send(x, callback(this, &XrlTestV1p0Client::unmarshall_get_greeting_count, cb));
}


/* Unmarshall get_greeting_count */
void
XrlTestV1p0Client::unmarshall_get_greeting_count(
	const XrlError&	e,
	XrlArgs*	a,
	GetGreetingCountCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e, 0);
	return;
    } else if (a && a->size() != 1) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(1));
	cb->dispatch(XrlError::BAD_ARGS(), 0);
	return;
    }
    int32_t num_msgs;
    try {
	a->get("num_msgs", num_msgs);
    } catch (const XrlArgs::XrlAtomNotFound&) {
	XLOG_ERROR("Atom not found");
	cb->dispatch(XrlError::BAD_ARGS(), 0);
	return;
    }
    cb->dispatch(e, &num_msgs);
}

bool
XrlTestV1p0Client::send_get_greeting(
	const char*	dst_xrl_target_name,
	const int32_t&	greeting_num,
	const GetGreetingCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "test/1.0/get_greeting");
    x.args().add("greeting_num", greeting_num);
    return _sender->send(x, callback(this, &XrlTestV1p0Client::unmarshall_get_greeting, cb));
}


/* Unmarshall get_greeting */
void
XrlTestV1p0Client::unmarshall_get_greeting(
	const XrlError&	e,
	XrlArgs*	a,
	GetGreetingCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e, 0);
	return;
    } else if (a && a->size() != 1) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(1));
	cb->dispatch(XrlError::BAD_ARGS(), 0);
	return;
    }
    string greeting;
    try {
	a->get("greeting", greeting);
    } catch (const XrlArgs::XrlAtomNotFound&) {
	XLOG_ERROR("Atom not found");
	cb->dispatch(XrlError::BAD_ARGS(), 0);
	return;
    }
    cb->dispatch(e, &greeting);
}

bool
XrlTestV1p0Client::send_shoot_foot(
	const char*	dst_xrl_target_name,
	const ShootFootCB&	cb
)
{
    Xrl x(dst_xrl_target_name, "test/1.0/shoot_foot");
    return _sender->send(x, callback(this, &XrlTestV1p0Client::unmarshall_shoot_foot, cb));
}


/* Unmarshall shoot_foot */
void
XrlTestV1p0Client::unmarshall_shoot_foot(
	const XrlError&	e,
	XrlArgs*	a,
	ShootFootCB		cb
)
{
    if (e != XrlError::OKAY()) {
	cb->dispatch(e);
	return;
    } else if (a && a->size() != 0) {
	XLOG_ERROR("Wrong number of arguments (%u != %u)", XORP_UINT_CAST(a->size()), XORP_UINT_CAST(0));
	cb->dispatch(XrlError::BAD_ARGS());
	return;
    }
    cb->dispatch(e);
}
